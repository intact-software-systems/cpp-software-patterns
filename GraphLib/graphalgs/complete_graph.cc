/***************************************************************************
                          complete_graph.cc  -  description
                             -------------------
    begin                : Wed Mar 08 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "complete_graph.h"
#include "graphalgs.h"
#include "steiner_set.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace GraphAlgorithms;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
	compGraph():
		start function for computing complete Graphs
----------------------------------------------------------------------- */
void
CompleteGraph::compGraph(TreeStructure &inputT, const GraphN &global_g, SteinerPoints& fifoSteiner, bool update)
{
	GraphAlgo algo = GlobalSimArgs::getSimGraphAlgo();
	int numSteiner = 0;
	VertexSet steinerSet;

	//cerr << WRITE_FUNCTION << algo << endl;

	// 1. If SMT -> find WCNs to add
	if(isSteinerAlgo(GlobalSimArgs::getSimTreeAlgo()) || (algo != COMPLETE_MEMBER_GRAPH))
	{	
		//VertexSet prevMembers = groupInfo_.getPrevTreeStructure().V - groupInfo_.getPrevTreeStructure().S;
		if(algo == COMPLETE_MEMBER_GRAPH_NEW_STEINER)
		{
			numSteiner = generateSizeOfSteinerSet(groupMembers_.size()); 
			//if(numSteiner) findWCNodes(global_g, groupInfo_.getPrevTreeStructure().g, fifoSteiner.steinerSet_, groupMembers_, steinerSet, numSteiner);
			if(numSteiner) 
			{
				switch(GlobalSimArgs::getSimWCNAlgo())
				{
					case WALK_SELECTION: 		
					case UNICAST_WALK_SELECTION: 
						ASSERTING(update);
						findWCNodes(global_g, inputT.g, fifoSteiner.steinerSet_, groupMembers_, groupMembers_ + newMember_, steinerSet, numSteiner, GlobalSimArgs::getSimWCNAlgo());
						break;
					default:
						findWCNodes(global_g, groupInfo_.getPrevTreeStructure().g, fifoSteiner.steinerSet_, groupMembers_, groupMembers_ + newMember_, steinerSet, numSteiner, GlobalSimArgs::getSimWCNAlgo());
						break;				
				}
			}
		}
		else if(algo == COMPLETE_MEMBER_GRAPH_KEEP_STEINER)
		{
			numSteiner = generateSizeOfSteinerSet(groupMembers_.size()) - groupInfo_.getSteinerPoints().size();
			//cerr << " num steiner " << numSteiner << endl;
			if(numSteiner) 
			{
				switch(GlobalSimArgs::getSimWCNAlgo())
				{
					case WALK_SELECTION:
					case UNICAST_WALK_SELECTION:
						ASSERTING(update);
						findWCNodes(global_g, inputT.g, fifoSteiner.steinerSet_, groupMembers_, groupMembers_ + steinerPoints_ + newMember_, steinerSet, numSteiner, GlobalSimArgs::getSimWCNAlgo());
						break;
					default:
						findWCNodes(global_g, groupInfo_.getPrevTreeStructure().g, fifoSteiner.steinerSet_, groupMembers_,  groupMembers_ + steinerPoints_ + newMember_, steinerSet, numSteiner, GlobalSimArgs::getSimWCNAlgo());
						break;				
				}
			}
			
			steinerSet = steinerSet + steinerPoints_;
		}

		// -- hack to test core selection variations -- 
		inputT.C = steinerSet;
	}
	// else -> adding no WCNs
	
	if(update)
	{
		//cerr << WRITE_FUNCTION << " steiner points " << steinerSet << " newMember_ " << newMember_ << " is in group? " << " members: " << groupMembers_ << endl;
		ASSERTING(!(steinerSet.contains(newMember_) && groupMembers_.contains(newMember_)));
	}
	
	// 2. create complete graph
	if(algo == COMPLETE_MEMBER_GRAPH) // || (!isSteinerAlgo(GlobalSimArgs::getSimTreeAlgo())))
	{
		if(!update) createCMGraph(inputT, global_g, groupMembers_); 
		else updateCMTree(inputT, global_g, groupMembers_, newMember_);
	}
	else if(algo == COMPLETE_MEMBER_GRAPH_KEEP_STEINER  || algo == COMPLETE_MEMBER_GRAPH_NEW_STEINER)
	{
		if(!update) createCMGraphSteinerSet(inputT, global_g, groupMembers_, steinerSet);
		else updateCMGraphSteinerSet(inputT, global_g, groupMembers_, steinerSet);
	}
	else if(algo == COMPLETE_GRAPH)
	{
		if(!update)	createCGraph(inputT, global_g, groupMembers_); 
		else updateCTree(inputT, global_g, groupMembers_, newMember_);
	}
	else // error NO_GRAPH_ALGO
	{
		ASSERTING(algo != NO_GRAPH_ALGO); exit(0);
	}

	if(!(inputT.Ep.size() == ((inputT.V.size() * (inputT.V.size() - 1))/ 2)))
	{
		//cerr << WRITE_FUNCTION << " dumping tree : " << inputT << endl;
		//dumpGraph(inputT.g);
		cerr << WRITE_FUNCTION << " num edges : " << num_edges(inputT.g) << " num edges Ep : " << inputT.Ep.size() << " num vertices(g) : " << num_vertices(inputT.g) << " num vertices(V) " << inputT.V.size() << " should be edges : " <<  ((inputT.V.size() * (inputT.V.size() - 1))/ 2) << endl;
	}

	ASSERTING(num_edges(inputT.g) == ((inputT.V.size() * (inputT.V.size() - 1))/ 2));
}

/*-----------------------------------------------------------------------
	createCGraph():		
		receives a global complete graph and marks group 
	members.
----------------------------------------------------------------------- */
void
CompleteGraph::createCGraph(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers)
{
	vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(groupMembers.contains(*vit)) inputT.insertVertex(*vit, g[*vit], GROUP_MEMBER); 
		else inputT.insertVertex(*vit, g[*vit], STEINER_POINT_AVAILABLE);
	}

	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(g); eit != eit_end; ++eit) inputT.insertEdge(*eit, g);
}

void
CompleteGraph::updateCTree(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, int new_v)
{
	if(groupMembers.count(new_v)) inputT.makeMember(new_v);
	else inputT.makeSteinerAvailable(new_v); 
}

/*-----------------------------------------------------------------------
	createCMGraph():		
		receives a global complete graph, creates a new
	complete graph with only group members.
----------------------------------------------------------------------- */
void
CompleteGraph::createCMGraph(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers)
{
	inputT.clear();
	for(VertexSet::iterator it = groupMembers.begin(), it_end = groupMembers.end(); it != it_end; ++it)
	{
		inputT.insertVertex(*it, g[*it], GROUP_MEMBER); 
		for(VertexSet::const_iterator it_in = it, it_in_end = groupMembers.end(); it_in != it_in_end; ++it_in)
		{
			int src = *it;
			int targ = *it_in;
			if(src == targ) continue;

			inputT.insertEdge(src, targ, g);
		}
	}
}

void
CompleteGraph::updateCMTree(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, int new_v)
{
	ASSERTING(new_v > -1);
	if(groupMembers.count(new_v))
	{
		if(inputT.V.contains(new_v))
		{
			ASSERTING(inputT.g[new_v].vertexState == STEINER_POINT || inputT.g[new_v].vertexState == STEINER_POINT_AVAILABLE);
			inputT.makeMember(new_v);
		}
		else inputT.insertVertex(new_v, g[new_v], GROUP_MEMBER);  

		for(VertexSet::const_iterator it = groupMembers.begin(), it_end = groupMembers.end(); it != it_end; ++it)
			if(*it != new_v) 
				inputT.insertEdge(*it, new_v, g);
	}
	else inputT.removeVertex(new_v); 
}

/*-----------------------------------------------------------------------
	createCMGraphSteinerSet():		
		receives a global complete graph, creates a new
	complete graph with group members and k set steiner points
----------------------------------------------------------------------- */
void
CompleteGraph::createCMGraphSteinerSet(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, VertexSet &steinerSet) 
{
	VertexSet totalV = steinerSet + groupMembers;
	inputT.clear();
	//cerr << WRITE_FUNCTION << " members : " << groupMembers << " steiners : " << steinerSet << endl;

	for(VertexSet::iterator it = totalV.begin(), it_end = totalV.end(); it != it_end; ++it)
	{
		int v = *it;
		inputT.insertVertex(v, g[v], GROUP_MEMBER); 

		for(VertexSet::iterator it_in = it, it_in_end = totalV.end(); it_in != it_in_end; ++it_in)
		{
			int u = *it_in;
			if(u == v) continue;

			ASSERTING(v != u);
			inputT.insertEdge(v, u, g);
		}
	}
	
	for(VertexSet::iterator it = steinerSet.begin(), it_end = steinerSet.end(); it != it_end; ++it)
		inputT.makeSteinerAvailable(*it);
}

void
CompleteGraph::updateCMGraphSteinerSet(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, VertexSet &steinerSet) 
{
	updateCMTree(inputT, g, groupMembers, newMember_);

	// remove old steiner points
	VertexSet remSP = inputT.S - steinerSet - newMember_;	
	//cerr << WRITE_FUNCTION << " removing " << remSP << " input S " << inputT.S << " steiner set " << steinerSet << " input V " << inputT.V << endl;
	for(VertexSet::iterator it = remSP.begin(), it_end = remSP.end(); it != it_end; ++it)
	{
		ASSERTING(inputT.g[*it].vertexState == STEINER_POINT || inputT.g[*it].vertexState == STEINER_POINT_AVAILABLE);
		inputT.removeVertex(*it);
	}
	
	VertexSet totalV = steinerSet; 
	VertexSet connectV = steinerSet + groupMembers;
	for(VertexSet::iterator it = totalV.begin(), it_end = totalV.end(); it != it_end; ++it)
	{
		int vi = *it;
		connectV.erase(vi);
		inputT.insertVertex(vi, g[vi], STEINER_POINT_AVAILABLE);

		for(VertexSet::iterator it_in = connectV.begin(), it_in_end = connectV.end(); it_in != it_in_end; ++it_in)
			inputT.insertEdge(vi, *it_in, g);
	}
	
	//cerr << " inputT S " << inputT.S << " steiner Set " << steinerSet << endl;
	ASSERTING(inputT.S == steinerSet);
}


