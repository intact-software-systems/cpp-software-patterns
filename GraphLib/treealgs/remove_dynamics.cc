/***************************************************************************
                          remove_dynamics.cc  -  description
                             -------------------
    begin                : Thu Aug 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
 
#include "remove_dynamics.h"
#include "prim_mst.h"
#include "../simdefs.h"
#include "../graphalgs/coreh.h"
#include "../graphalgs/complete_graph.h"
#include "../graphalgs/graphalgs.h"
#include "../graphstats/recordtree.h"
#include "../simtime.h"
#include <cmath>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
		removeVertex()
			start function for removing a single vertex from a tree
	TODO: - check efficiency of worstCaseDist() vs. wcVertexDistances()
			-> worstCaseDist() is much faster
------------------------------------------------------------------------- */
void 
RemoveDynamics::removeVertex(const DynamicTreeAlgo &treeAlgo)
{
    uint64_t a, b;
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile( 	"push %eax\n"
						"push %ebx\n"
						"push %ecx\n"
						"push %edx\n"
						"cpuid\n"
						"pop %edx\n"
						"pop %ecx\n"
						"pop %ebx\n"
						"pop %eax\n" );
	__asm __volatile(".byte 0x0f,0x31" : "=A" (a));
#endif

	clock_t alg_time = removeVertex_in(treeAlgo);
	
    uint64_t cpu_cycles = 0;
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile(".byte 0x0f,0x31" : "=A" (b));
	cpu_cycles = b - a;
#endif

	// remove unused steiner points
	rvUnusedSP();

	// record stats?
	if(groupInfo_.getMembers().size() > 1)
	{
		StatsContainer SC(groupInfo_.getTreeStructure(), groupInfo_.getPrevTreeStructure(), NO_TREE_ALGO, treeAlgo_, NO_RECONFIG_ALGO, groupInfo_.getGroupId(), alg_time, cpu_cycles);
		recordStats(statout, &SC);
	}
	
	// -- debug --
	if(GlobalSimArgs::removeMeshAlgo() == NO_DYNAMIC_MESH_ALGO)
		checkRemove(actionVert_);
	// -- debug end --
}

clock_t
RemoveDynamics::removeVertex_in(const DynamicTreeAlgo &treeAlgo)
{
	// -- debug --
	//cerr << WRITE_FUNCTION << " Action Vertex " << actionVert_ << endl;
	//groupT_.print();
	ASSERTING(groupT_.V.contains(actionVert_));
	ASSERTING(groupT_.g[actionVert_].vertexState == GROUP_MEMBER);
	// -- debug end --
	
	clock_t exec_time = clock();
	
	if(groupInfo_.getMembers().size() <= 1)
	{
		groupT_.removeVertex(actionVert_);
		groupT_.clearSteiner();
		groupT_.clearEdges();
					
		//cerr << " GetMembers.size() " << groupInfo_.getMembers().size() << " Tree V.size() : " << groupT_.V.size() << endl;

		// -- debug --	
		ASSERTING(groupInfo_.getMembers().size() == groupT_.V.size());
		ASSERTING(groupT_.S.size() == 0);
		ASSERTING(groupT_.E.size() == 0);
		ASSERTING(groupT_.Ep.size() == 0);
		ASSERTING(num_edges(groupT_.g) == 0);
		// -- debug end --
	}	
	else if(getOutDegree(actionVert_, groupT_.g) == 1) // REMOVE_LEAF
	{
		treeAlgo_ = REMOVE_LEAF;
		rvLeaf();
	}
	else // remove algorithm
	{
		ASSERTING(getOutDegree(actionVert_, groupT_.g) > 1);
		treeAlgo_ = treeAlgo; 

		bool again = true;
		while(again)
		{
			again = false;
			try{
				exec_time = clock();
				switch(treeAlgo_) //GlobalSimArgs::removeTreeAlgo())
				{
					case REMOVE_KEEP_AS_NON_MEMBER:
						rvKeepAsSP();
						break;
					case REMOVE_KEEP_WELL_CONNECTED:
						rvKeepWCN();
						break;
					case REMOVE_TRY_REPLACE_MC_NAIVE:
						rvTryReplaceNaive();
						break;
					case REMOVE_TRY_REPLACE_PRUNE_SEARCH_MDDL :
					case REMOVE_TRY_REPLACE_PRUNE_MDDL:
					case REMOVE_TRY_REPLACE_PRUNE_SEARCH_MC:
					case REMOVE_TRY_REPLACE_PRUNE_MC:
						rvTryReplace();
						break;
					case REMOVE_TRY_REPLACE_MDDL_NAIVE:
						if(getOutDegree(actionVert_, groupT_.g) == 2)	rvODTwo();
						else rvTRMDDLN();
						break;
					case REMOVE_MINIMUM_COST_EDGE:
					case REMOVE_DC_MINIMUM_COST_EDGE:
					case REMOVE_BD_MINIMUM_COST_EDGE:
					case REMOVE_BR_MINIMUM_COST_EDGE:
					{
						if(getOutDegree(actionVert_, groupT_.g) == 2)	rvODTwo();
						else rvMCEdge();
						break;
					}
					case REMOVE_SEARCH_MINIMUM_COST_EDGE:
					case REMOVE_DC_SEARCH_MINIMUM_COST_EDGE:
					case REMOVE_BD_SEARCH_MINIMUM_COST_EDGE:
					case REMOVE_BR_SEARCH_MINIMUM_COST_EDGE:	
						rvSearchMCEdge();
						break;
					case REMOVE_MINIMUM_DIAMETER_EDGE:
					case REMOVE_DC_MINIMUM_DIAMETER_EDGE:
					case REMOVE_BD_MINIMUM_DIAMETER_EDGE:
					case REMOVE_BR_MINIMUM_DIAMETER_EDGE:	
					{
						if(getOutDegree(actionVert_, groupT_.g) == 2)	rvODTwo();
						else rvMDEdge();
						break;
					}
					case REMOVE_SEARCH_MINIMUM_DIAMETER_EDGE:
					case REMOVE_DC_SEARCH_MINIMUM_DIAMETER_EDGE:
					case REMOVE_BD_SEARCH_MINIMUM_DIAMETER_EDGE:
					case REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE:	
						rvSearchMDEdge();
						break;
					case REMOVE_SUBGRAPH:
					{
						if(groupInfo_.getMembers().size() < 5)
						{
							treeAlgo_ = GlobalSimArgs::removeTreeBackupAlgo();
							throw true;
						}
				
						VertexSet steinerSet;
						TreeStructure subT;
						rvSubGraph(groupT_, subT, steinerSet);
						//exec_time = 
						subTree(groupT_, subT, steinerSet);
						break;
					}	
					// -- not used --
					case REMOVE_BEST_NEIGHBORHOOD_EDGE:
						rvBNP();
						break;
					case REMOVE_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE:
						rvMDBNP();
						break;
					// -- not used end --
					default:
						cerr << "ERROR! No removeTreeAlgorithm match " << treeAlgo_ << endl;
						ASSERTING(0);
						abort();
						break;
				}
			}
			catch(bool in)
			{
				cerr << WRITE_FUNCTION << " trying again tree algo " << treeAlgo_ << endl;
				//char c = getchar();
				again = in;
			}
		}

		//cerr << WRITE_FUNCTION << " Tree diameter : " << getTreeDiameter(groupT_) << endl;
		//char c = getchar();
	}

	return (clock() - exec_time);
}

/*-----------------------------------------------------------------------
					remove unused steiner points
------------------------------------------------------------------------- */
void
RemoveDynamics::rvUnusedSP()
{
	//dumpTreeStructure(groupT_);
	//cerr << WRITE_FUNCTION << "steiner set " << groupT_.S << endl;
	VertexSet remV;
	
	VertexSet::iterator vit = groupT_.S.begin(), vit_end = groupT_.S.end();
	for( ; vit != vit_end; ++vit)
	{
		vertex_descriptorN sp = *vit;
		//cerr << sp << ": " << endl;

		// -- debug --
		ASSERTING(groupT_.g[sp].vertexState == STEINER_POINT);
		// -- debug end --

		if(getOutDegree(sp, groupT_.g) == 0)
		{
			//cerr << "Outdegree 0 " << endl;
			remV.insert(sp);
		}
		else if(getOutDegree(sp, groupT_.g) == 1)
		{
			//cerr << "Removing sp: " << sp << endl;
			//cerr << "Removing edge(s): " ; 
			out_edge_iteratorN oit, oit_end;
			while(getOutDegree(sp, groupT_.g) == 1 && groupT_.g[sp].vertexState == STEINER_POINT) 
			{
                boost::tuples::tie(oit, oit_end) = out_edges(sp, groupT_.g);
				remV.insert(sp);
				sp = target(*oit, groupT_.g);
				//cerr << *oit << " " ;
				groupT_.removeEdge(*oit);
			}
			//cerr << endl;
		}
	}

	// new steiner set
	vit = remV.begin();
	vit_end = remV.end();
	for( ; vit != vit_end; ++vit)
	{
		ASSERTING(groupT_.g[*vit].vertexState == STEINER_POINT);
		groupT_.removeVertex(*vit);
	}
}

/*-----------------------------------------------------------------------
					remove vertex functions
------------------------------------------------------------------------- */
void
RemoveDynamics::rvLeaf()
{
	ASSERTING(getOutDegree(actionVert_, groupT_.g) == 1);
	
	out_edge_iteratorN oit, oit_end;
    boost::tuples::tie(oit, oit_end) = out_edges(actionVert_, groupT_.g);
	
	// remove edge/vertex	
	groupT_.removeVertex(actionVert_); // clears out_edges -> clear_vertex(actionVert_, g);
	//groupT_.removeEdge(*oit);

	// -- debug --
 	//cerr << WRITE_FUNCTION << " actionVert_ " << actionVert_ << endl;
	//dumpGraph(groupT_.g);
	//dumpTreeStructure(groupT_);
	//char c = getchar();
}

/*-----------------------------------------------------------------------
			remove vertex:
				out degree == 2
------------------------------------------------------------------------- */
void 
RemoveDynamics::rvODTwo()
{
	int prev_v = -1, next_v = -1;
	out_edge_iteratorN oit, oit_end;
    boost::tuples::tie(oit, oit_end) = out_edges(actionVert_, groupT_.g);

	// find prev_v and next_v
	prev_v = target(*oit, groupT_.g);
	++oit;
	ASSERTING(oit != oit_end);
	next_v = target(*oit, groupT_.g);
	++oit;
	ASSERTING(oit == oit_end);

	ASSERTING(prev_v != next_v);
	ASSERTING(prev_v > -1 && next_v > -1);
	
	groupT_.removeVertex(actionVert_); 
	groupT_.insertEdge(prev_v, next_v, globalG_);
}

/*-----------------------------------------------------------------------
			remove vertex:
			   		keep as steiner point
------------------------------------------------------------------------- */
void
RemoveDynamics::rvKeepAsSP()
{
	if(getOutDegree(actionVert_, groupT_.g) > 2)
	{		
		groupT_.makeSteiner(actionVert_);
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo(); 
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 1)
	{
		rvLeaf();  
	}
}

void
RemoveDynamics::rvKeepWCN()
{
	if(getOutDegree(actionVert_, groupT_.g) > 2)
	{	
		if(groupT_.isWCN(actionVert_)) groupT_.makeSteiner(actionVert_);
		else rvTryReplaceNaive();
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo(); 
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 1)
	{
		rvLeaf();  
	}
}

/*-----------------------------------------------------------------------
			remove vertex:
		   		try to remove, replace or keep
------------------------------------------------------------------------- */
void
RemoveDynamics::rvTryReplaceNaive()
{
	//cerr << WRITE_FUNCTION << endl;
	
	if(getOutDegree(actionVert_, groupT_.g) > 2)
	{
		int sp_vert = -1;
		VertexSet adjacentSet;
		getNeighbors(groupT_, actionVert_, adjacentSet);
		//cerr << "AdjacentSet " << adjacentSet << endl;

		// collect the best connected steiner point to the neighbors of actionVert_ (adjacentSet)
		VertexSet newSPSet;
		findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, newSPSet, 1, GlobalSimArgs::getSimWCNAlgo());

		if(!newSPSet.empty()) sp_vert = *(newSPSet.begin());
	
		// compare: keep actionVert_, replace actionVert_ with sp_vert, removing actionVert_ and no sp_vert
		int inter_v = -1;
		double sumEdges = (numeric_limits<double>::max)(), sumEdgesSP = 0, sumEdgesAV = 0;
		VertexSet::iterator vit = adjacentSet.begin(), vit_end = adjacentSet.end();
		for( ; vit != vit_end; ++vit)
		{
			// degree is getOutDegree(*vit) - link to ActionVert + number of adjacent vertices to ActionVert minus link to *vit
			int degree_vit = (getOutDegree(*vit, groupT_.g) - 1) + (adjacentSet.size() - 1);
					
			if(degree_vit < getDegreeConstraint(globalG_, *vit))
			{
				double tempSumEdges = 0;
				VertexSet::iterator vit_in = adjacentSet.begin(), vit_in_end = adjacentSet.end();
				for( ; vit_in != vit_in_end; ++vit_in)
				{
					if(*vit == *vit_in) continue;
				
					// calculate using *vit as intersection
					pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, globalG_);
					ASSERTING(ep.second);
					tempSumEdges = tempSumEdges + globalG_[ep.first].weight;
				}
				if(sumEdges > tempSumEdges)
				{
					sumEdges = tempSumEdges;
					inter_v = *vit;
				}
			}

			// calculate using sp_vert as intersection
			if(sp_vert > -1)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, sp_vert, globalG_);
				ASSERTING(ep.second);
				sumEdgesSP = sumEdgesSP + globalG_[ep.first].weight;
			}
			
			//if(GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH)
			//{
				// calculate continue using actionVert_ as steiner point intersection
				pair<edge_descriptorN, bool> ep = edge(*vit, actionVert_, globalG_);
				ASSERTING(ep.second);
				sumEdgesAV = sumEdgesAV + globalG_[ep.first].weight;			
			//}
		}
		//cerr << " inter_v " << inter_v << " sumEdges " << sumEdges << " sp_vert " << sp_vert << " sumEdgesSP " << sumEdgesSP << " actionVert_ " << actionVert_ << " sumEdgesAV " << sumEdgesAV << endl;
		
		if(sumEdges < sumEdgesAV && sumEdges < sumEdgesSP && inter_v > -1)
		{
			// do nothing
		}		
		else if(sumEdgesSP < sumEdges && sumEdgesSP < sumEdgesAV && sp_vert > -1) // use steiner point as intersection 
		{
			inter_v = sp_vert;
			//cerr << " using steiner point " << endl;
			groupT_.insertVertex(inter_v, STEINER_POINT, globalG_[inter_v].vertexFunction);
		}
		//else //if(sumEdgesAV < sumEdges && sumEdgesAV < sumEdgesSP) // use actionVert_ as intersection
		else //if(GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH)
		{
			inter_v = actionVert_;
			//cerr << " using actionVert_ " << endl;
			groupT_.makeSteiner(actionVert_);
			return;
		}
		//else
		//{
		//	cerr << WRITE_FUNCTION << " Error, couldn't reconnect group tree!!" << endl;
		//	ASSERTING(0);
		//	abort();
		//}
			
		groupT_.removeVertex(actionVert_);
		groupT_.insertEdges(inter_v, adjacentSet - inter_v, globalG_);
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo();
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 1)
	{
		rvLeaf();
	}

	//dumpTreeStructure(groupT_);
	//char c = getchar();
}

/*-----------------------------------------------------------------------
			remove vertex:
		   		try to remove, replace or keep
------------------------------------------------------------------------- */
void 
RemoveDynamics::rvTRMDDLN()
{
	if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo();
		return;
	}

	// variables
 	double mcpDiameter = (numeric_limits<double>::max)();
	double spDiameter = (numeric_limits<double>::max)();
	double avDiameter = (numeric_limits<double>::max)();
	int mcpVert = -1;
	
	// Find WCN
	int sp_vert = -1;
	VertexSet adjacentSet, steinerSet;
	getNeighbors(groupT_, actionVert_, adjacentSet);
	//findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, steinerSet, 1, GlobalSimArgs::getSimWCNAlgo());
	//if(!steinerSet.empty()) 
	//{
	//	sp_vert = *(steinerSet.begin());
	//	ASSERTING(!groupT_.V.contains(sp_vert));
	//}
	
	// variables
	int avVertOD = getOutDegree(groupT_.g, actionVert_);
	//int spVertDL = 0;
	int checkVert = actionVert_;
	//if(sp_vert > -1) spVertDL = getDegreeConstraint(globalG_, sp_vert);

	//cerr << WRITE_FUNCTION << " adjacent set " << adjacentSet <<  " sp vert " << sp_vert << endl;
	// 1st: Check using a neighbor (mcpVert) as intersection
	int adjSz = adjacentSet.size();
	for(VertexSet::iterator vit = adjacentSet.begin(), vit_end = adjacentSet.end(); vit != vit_end; ++vit)
	{
		// worst case distance from *vit to any node in the tree
		int OD = (getOutDegree(groupT_.g, *vit) - 1) + (adjSz - 1);
		if(OD < getDegreeConstraint(globalG_, *vit))
		{
			groupT_.removeEdges(checkVert, adjacentSet - checkVert, globalG_);
			groupT_.insertEdges(*vit, adjacentSet - *vit, globalG_);
			
			checkVert = *vit;
			double vert_diameter = getTreeDiameter(groupT_, *vit);
			if(vert_diameter < mcpDiameter) 
			{
				mcpDiameter = vert_diameter;
				mcpVert = *vit;
			}
		}
	}
	//cerr << "mcpVert " << mcpVert << " diameter " << mcpDiameter << endl;
	
	//if(GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH)
	//{
		findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, steinerSet, 1, GlobalSimArgs::getSimWCNAlgo());
		if(!steinerSet.empty()) 
		{
			sp_vert = *(steinerSet.begin());
			ASSERTING(!groupT_.V.contains(sp_vert));
		}
		int spVertDL = 0;
		if(sp_vert > -1) spVertDL = getDegreeConstraint(globalG_, sp_vert);

		// 2nd: Check using sp_vert as intersection
		if(avVertOD <= spVertDL && sp_vert > -1)
		{
			groupT_.removeEdges(checkVert, adjacentSet - checkVert, globalG_);
			groupT_.insertEdges(sp_vert, adjacentSet, globalG_);

			spDiameter = getTreeDiameter(groupT_, sp_vert);
			checkVert = sp_vert;
			//cerr << "sp_vert " << sp_vert << " diameter " << spDiameter << endl;
		}
	
		// 3rd: Check using actionVert_ as intersection
		groupT_.removeEdges(checkVert, adjacentSet - checkVert, globalG_);
		groupT_.insertEdges(actionVert_, adjacentSet, globalG_);

		checkVert = actionVert_;
		avDiameter = getTreeDiameter(groupT_, actionVert_);
		//cerr << "actionVert_ " << actionVert_ << " diameter " << avDiameter << endl;
	//}

	// Now: clean up after checking
	groupT_.removeEdges(checkVert, adjacentSet - checkVert, globalG_);
	if(sp_vert > -1) groupT_.removeVertex(sp_vert);

	// Finally: Which diameter is the smallest?
	if(mcpDiameter < spDiameter && mcpDiameter < avDiameter && mcpVert > -1) 
	{
		groupT_.removeVertex(actionVert_); 
		groupT_.insertEdges(mcpVert, adjacentSet - mcpVert, globalG_);
		//cerr << "-> use MDDL edge mcpVert" << endl;
	}
	else if(spDiameter < avDiameter && spDiameter < mcpDiameter && sp_vert > -1)
	{
		groupT_.removeVertex(actionVert_); 
		// use sp_vert 
		groupT_.insertVertex(sp_vert, STEINER_POINT, globalG_[actionVert_].vertexFunction);
		groupT_.insertEdges(sp_vert, adjacentSet, globalG_);
		//cerr << "-> use sp_vert " << sp_vert << " as intersection" << endl;
	}
	//else //if(avDiameter < spDiameter && avDiameter < mcpDiameter) 
	else //if(GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH)
	{
		groupT_.makeSteiner(actionVert_);
		// use actionVert_
		groupT_.insertEdges(actionVert_, adjacentSet, globalG_);
		//cerr << "-> use actionVert_ as intersection" << endl;
	}
	//else
	//{
	//	cerr << WRITE_FUNCTION << " Error, couldn't reconnect group tree!!" << endl;
	//	ASSERTING(0);
	//	abort();
	//}

	//cerr << WRITE_FUNCTION << "Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
}

/*-----------------------------------------------------------------------
			remove vertex:
		   		try to remove, replace or keep
------------------------------------------------------------------------- */
void 
RemoveDynamics::rvTryReplace()
{
	//cerr << WRITE_FUNCTION << endl;
	if(getOutDegree(actionVert_, groupT_.g) > 2)
	{
		// variables 
		VertexSet 	adjacentSet;	 // only group members
		VertexSet 	adjacentSetSP;	 // only steiner points
		VertexSet	newSPSet;
		EdgeList	remEdges;
		
		getNeighborsOptimizeSP(groupT_, actionVert_, actionVert_, adjacentSet, adjacentSetSP, remEdges);
		adjacentSet = adjacentSet - actionVert_;

		for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
			groupT_.removeEdge(*eit);
				
		int numNewSP = (int) ceil((double) (adjacentSet.size()/ (double)GlobalSimArgs::getDegreeConstraintSP())) - adjacentSetSP.size();
		if(numNewSP > 0) 
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, newSPSet, numNewSP, GlobalSimArgs::getSimWCNAlgo());
			//findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V, adjacentSet + adjacentSetSP + groupT_.S, newSPSet, numNewSP, GlobalSimArgs::getSimWCNAlgo());
	
		// -- debug --
		//cerr << "Adjacentset " << adjacentSet << " adjacent sp " << adjacentSetSP << " new sp " << newSPSet << endl;
		// -- debug end --
		
		VertexSet testVertices = adjacentSet + adjacentSetSP + newSPSet	;
		map<int, double> mapSumEdges;
		VertexSet::iterator vit = adjacentSet.begin(), vit_end = adjacentSet.end(), vit_in, vit_in_end;
		for( ; vit != vit_end; ++vit)
		{
			for(vit_in = testVertices.begin(), vit_in_end = testVertices.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vit == *vit_in) continue;

				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, globalG_);
				ASSERTING(ep.second);
				mapSumEdges[*vit_in] = mapSumEdges[*vit_in] + globalG_[ep.first].weight;
			}
		}
		
		// next: identify the intersection vertices
		multimap<double, int> mapConnectV; // rank nodes
		for(vit = testVertices.begin(), vit_end = testVertices.end(); vit != vit_end; ++vit)
			mapConnectV.insert(pair<double, int>(mapSumEdges[*vit], *vit));
			
		int sumODCapacity = 0;
		VertexSet connectV;
		multimap<double, int>::iterator mmit = mapConnectV.begin(), mmit_end = mapConnectV.end();
		for( ; mmit != mmit_end; ++mmit)
		{
			int ODCapacity = (getDegreeConstraint(globalG_, mmit->second) - getOutDegree(groupT_.g, mmit->second));
		//	cerr << " V: " << mmit->second << " ODCapacity " << ODCapacity << endl;
			if(ODCapacity > 1)
			{
				connectV.insert(mmit->second);
				sumODCapacity += ODCapacity;
			}

			// TODO: it is actually ((connectV - adjacentSet).size() - 2) * 2 + 2, but but...
			int connectInterconnectOD = ((connectV.size() - 2) * 2) + 2; 	
			if(sumODCapacity >= (adjacentSet.size() + connectInterconnectOD) ) break; 
		}
		
		//ASSERTING(connectV.size() > 0);
		
		// now: connectV is the absolute minimum and best set of vertices that we want to connect to adjacentSet
		// remove unused SPs and actionVert_
		// cerr << " remove unused SPs " ;
		VertexSet remV = adjacentSetSP - connectV;
		for(vit = remV.begin(), vit_end = remV.end(); vit != vit_end; ++vit)
		{
			//cerr << *vit << "," ;
			ASSERTING(groupT_.g[*vit].vertexState == STEINER_POINT);
			groupT_.removeVertex(*vit);
		}
		//cerr << endl;
		groupT_.removeVertex(actionVert_);
	
		//cerr << "Intersection vertices " << connectV << endl;
		newSPSet = newSPSet - remV;
		//cerr << "New sps " << newSPSet << endl;
		for(vit = newSPSet.begin(), vit_end = newSPSet.end(); vit != vit_end; ++vit)
		{
			ASSERTING(!groupT_.V.contains(*vit));
			groupT_.insertVertex(*vit, STEINER_POINT, globalG_[*vit].vertexFunction);
		}
		
		VertexSet totConnect = connectV + adjacentSet;
		OutDegreeMap adjacentMap;
		for(VertexSet::iterator vit = totConnect.begin(), vit_end = totConnect.end(); vit != vit_end; ++vit)
			adjacentMap.insert(pair<int, vertex_descriptorN>(getOutDegree(groupT_.g, *vit), *vit));
			//adjacentMap.insert(pair<int, vertex_descriptorN>(getAvOutDegree(globalG_, groupT_, *vit), *vit));

		// connect again
		VertexSet connectedV;
		switch(treeAlgo_)
		{	
			case REMOVE_TRY_REPLACE_PRUNE_SEARCH_MDDL :
					connectSearchMDE(groupT_, adjacentMap, connectedV);
					break;
			case REMOVE_TRY_REPLACE_PRUNE_MDDL :
					connectMDE(groupT_, adjacentMap, connectedV);
					break;
			case REMOVE_TRY_REPLACE_PRUNE_SEARCH_MC :
				{
					OutDegreeMap::iterator big = adjacentMap.begin();
					connectedV.insert(big->second); 
					adjacentMap.erase(big); // pretty much always a leaf node!!!!!!
					connectSearchMCE(groupT_, adjacentMap, connectedV);
					break;
				}
			case REMOVE_TRY_REPLACE_PRUNE_MC :
				{
					OutDegreeMap::iterator big = adjacentMap.begin();
					connectedV.insert(big->second); 
					adjacentMap.erase(big); // pretty much always a leaf node!!!!!!
					connectMCE(groupT_, adjacentMap, connectedV);
					break;
				}
			default :
				throw("ERROR! remove algo does not fit here!!! ");
				connectMCE(groupT_, adjacentMap, connectedV);
				break;
		}

		// check if steiner point(s) became leaves
		for(VertexSet::iterator vit = connectV.begin(), vit_end = connectV.end(); vit != vit_end; ++vit)
			if(getOutDegree(*vit, groupT_.g) <= 1 && groupT_.g[*vit].vertexState != GROUP_MEMBER) 
				groupT_.removeVertex(*vit);
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo();
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 1)
	{
		rvLeaf();
	}
}

/*-----------------------------------------------------------------------
		rvMCEdge() : removes edge, connects tree using MCEs
------------------------------------------------------------------------- */
void
RemoveDynamics::rvMCEdge()
{
	VertexSet actVertNeighbors, connectedV;
	OutDegreeMap adjacentMap;
	getNeighbors(groupT_, actionVert_, actVertNeighbors, adjacentMap);

	// feasibility test
	int ODAvNeighbors = getAvOutDegree(globalG_, groupT_, actVertNeighbors) + actVertNeighbors.size(); 
	int MinODNeeded = getMinODNeedConnectV(actVertNeighbors.size());
	
	if(ODAvNeighbors >= MinODNeeded)
	{
		OutDegreeMap::iterator big = adjacentMap.begin();
		connectedV.insert(big->second); 
		adjacentMap.erase(big);
		
		if(treeAlgo_ == REMOVE_DC_MINIMUM_COST_EDGE || treeAlgo_ == REMOVE_BR_MINIMUM_COST_EDGE)
		{
			ParentVector parent(num_vertices(groupT_.g));
			DistanceVector 	distance(num_vertices(groupT_.g));
			int src = GraphAlgorithms::findMDNode(groupT_.V - actionVert_, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
			
			groupT_.removeVertex(actionVert_);
			if(treeAlgo_ == REMOVE_BR_MINIMUM_COST_EDGE) 
				connectBDMCE(groupT_, adjacentMap, connectedV, distance, GlobalSimArgs::getDiameterBound()/2);
			else 
				connectDCMCE(groupT_, adjacentMap, connectedV, distance);
		}
		else if(treeAlgo_ == REMOVE_BD_MINIMUM_COST_EDGE)
		{
			dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
			double path_cost = (numeric_limits<double>::max)();
			int targ = -1;
			TreeDiameter TD;
		
			int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
			groupT_.removeVertex(actionVert_); 	
			connectBDMCE(groupT_, adjacentMap, connectedV, TD.distance, GlobalSimArgs::getDiameterBound());
		}
		else 
		{
			// remove vertex from tree, and clear out edges 
			groupT_.removeVertex(actionVert_); 	
			connectMCE(groupT_, adjacentMap, connectedV);
		}
	}
	else
	{
		//if(	treeAlgo_ == REMOVE_DC_MINIMUM_COST_EDGE || 
		//	treeAlgo_ == REMOVE_BR_MINIMUM_COST_EDGE ||
		//	treeAlgo_ == REMOVE_BD_MINIMUM_COST_EDGE)
		//{
		treeAlgo_ = REMOVE_TRY_REPLACE_MDDL_NAIVE;
		//}
		//else treeAlgo_ = GlobalSimArgs::removeTreeBackupAlgo();
	
		//cerr << "-------------- USING BACKUP TREEALGO ---------------    " << treeAlgo_ << endl;

		throw true;
	}	
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(groupT_);
	// -- debug end --
}

/*-----------------------------------------------------------------------
		remove vertex:
			creating n subtrees, connect two and two trees in 
		a greedy fashion, idea from SMT algorithms
------------------------------------------------------------------------- */
void
RemoveDynamics::rvSearchMCEdge()
{
	VertexSet actVertNeighbors, connectedV;
	OutDegreeMap adjacentMap;
	getNeighbors(groupT_, actionVert_, actVertNeighbors, adjacentMap);

	// feasibility test
	int ODAvNeighbors = getAvOutDegree(globalG_, groupT_, actVertNeighbors) + actVertNeighbors.size(); 
	int MinODNeeded = getMinODNeedConnectV(actVertNeighbors.size());
	
	if(ODAvNeighbors >= MinODNeeded)
	{
		OutDegreeMap::iterator big = adjacentMap.begin();
		connectedV.insert(big->second); 
		adjacentMap.erase(big);

		if(treeAlgo_ == REMOVE_DC_SEARCH_MINIMUM_COST_EDGE || treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_COST_EDGE)
		{
			ParentVector parent(num_vertices(groupT_.g));
			DistanceVector 	distance(num_vertices(groupT_.g));
			int src = GraphAlgorithms::findMDNode(groupT_.V - actionVert_, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
			
			groupT_.removeVertex(actionVert_);
			if(treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_COST_EDGE) 
				connectBDSearchMCE(groupT_,adjacentMap, connectedV, distance, GlobalSimArgs::getDiameterBound()/2);
			else 
				connectDCSearchMCE(groupT_,adjacentMap, connectedV, distance);
		}
		else if(treeAlgo_ == REMOVE_BD_SEARCH_MINIMUM_COST_EDGE)
		{
			dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
			double path_cost = (numeric_limits<double>::max)();
			int targ = -1;
			TreeDiameter TD;
		
			int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
			groupT_.removeVertex(actionVert_); 	

			connectBDSearchMCE(groupT_, adjacentMap, connectedV, TD.distance, GlobalSimArgs::getDiameterBound());
		}
		else 
		{
			groupT_.removeVertex(actionVert_); 	
			connectSearchMCE(groupT_,adjacentMap, connectedV);
		}
	}
	else
	{

		//if(	treeAlgo_ == REMOVE_DC_SEARCH_MINIMUM_COST_EDGE || 
		//	treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_COST_EDGE ||
		//	treeAlgo_ == REMOVE_BD_SEARCH_MINIMUM_COST_EDGE)
		//{
		treeAlgo_ = REMOVE_TRY_REPLACE_MDDL_NAIVE;
		//}
		//else treeAlgo_ = GlobalSimArgs::removeTreeBackupAlgo();
	
		//cerr << "-------------- USING BACKUP TREEALGO ---------------   " << treeAlgo_ << endl;

		throw true;
	}	
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(groupT_);
	// -- debug end --
}

/*-----------------------------------------------------------------------
			remove vertex:
				connect neighbors using minimum diameter edge 
------------------------------------------------------------------------- */
void
RemoveDynamics::rvMDEdge()
{
	VertexSet actVertNeighbors, connectedV;
	OutDegreeMap adjacentMap;
	getNeighbors(groupT_, actionVert_, actVertNeighbors, adjacentMap);

	// feasibility test
	int ODAvNeighbors = getAvOutDegree(globalG_, groupT_, actVertNeighbors) + actVertNeighbors.size(); 
	int MinODNeeded = getMinODNeedConnectV(actVertNeighbors.size());
	
	if(ODAvNeighbors >= MinODNeeded)
	{
		if(treeAlgo_ == REMOVE_DC_MINIMUM_DIAMETER_EDGE || treeAlgo_ == REMOVE_BR_MINIMUM_DIAMETER_EDGE)
		{
			OutDegreeMap::iterator big = adjacentMap.begin();
			connectedV.insert(big->second); 
			adjacentMap.erase(big);

			ParentVector parent(num_vertices(groupT_.g));
			DistanceVector 	distance(num_vertices(groupT_.g));
			int src = GraphAlgorithms::findMDNode(groupT_.V - actionVert_, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
			
			groupT_.removeVertex(actionVert_); 	
			if(treeAlgo_ == REMOVE_BR_MINIMUM_DIAMETER_EDGE) 
				connectBDMDE(groupT_,adjacentMap, connectedV, distance, GlobalSimArgs::getDiameterBound()/2);
			else
				connectDCMDE(groupT_,adjacentMap, connectedV, distance);
		}
		else if(treeAlgo_ == REMOVE_BD_MINIMUM_DIAMETER_EDGE)
		{
			OutDegreeMap::iterator big = adjacentMap.begin();
			connectedV.insert(big->second); 
			adjacentMap.erase(big);

			dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
			double path_cost = (numeric_limits<double>::max)();
			int targ = -1;
			TreeDiameter TD;
		
			int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
			groupT_.removeVertex(actionVert_); 	

			connectBDMDE(groupT_,adjacentMap, connectedV, TD.distance, GlobalSimArgs::getDiameterBound());	
		}
		else 
		{
			groupT_.removeVertex(actionVert_); 	
			connectMDE(groupT_,adjacentMap, connectedV);
		}
	}
	else
	{
		//if(	treeAlgo_ == REMOVE_DC_MINIMUM_DIAMETER_EDGE || 
		//	treeAlgo_ == REMOVE_BR_MINIMUM_DIAMETER_EDGE ||
		//	treeAlgo_ == REMOVE_BD_MINIMUM_DIAMETER_EDGE ||
		//	treeAlgo_ == REMOVE_MINIMUM_DIAMETER_EDGE)
		//{
		treeAlgo_ = REMOVE_TRY_REPLACE_MDDL_NAIVE;
		//}
		//else treeAlgo_ = GlobalSimArgs::removeTreeBackupAlgo();
	
		//cerr << "-------------- USING BACKUP TREEALGO ---------------   " << treeAlgo_ << endl;

		throw true;
	}	
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(groupT_);
	// -- debug end --
}

/*-----------------------------------------------------------------------
		remove vertex:
			connect subtrees using minimum diameter edge
------------------------------------------------------------------------- */
void
RemoveDynamics::rvSearchMDEdge()
{
	VertexSet actVertNeighbors, connectedV;
	OutDegreeMap adjacentMap;
	getNeighbors(groupT_, actionVert_, actVertNeighbors, adjacentMap);

	// feasibility test
	int ODAvNeighbors = getAvOutDegree(globalG_, groupT_, actVertNeighbors) + actVertNeighbors.size(); 
	int MinODNeeded = getMinODNeedConnectV(actVertNeighbors.size());
	
	if(ODAvNeighbors >= MinODNeeded)
	{
		if(treeAlgo_ == REMOVE_DC_SEARCH_MINIMUM_DIAMETER_EDGE || treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE)
		{
			OutDegreeMap::iterator big = adjacentMap.begin();
			connectedV.insert(big->second); 
			adjacentMap.erase(big);

			ParentVector parent(num_vertices(groupT_.g));
			DistanceVector 	distance(num_vertices(groupT_.g));
			int src = GraphAlgorithms::findMDNode(groupT_.V - actionVert_, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
			
			groupT_.removeVertex(actionVert_); 	
			if(treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE) 
				connectBDSearchMDE(groupT_,adjacentMap, connectedV, distance, GlobalSimArgs::getDiameterBound()/2);
			else 
				connectDCSearchMDE(groupT_,adjacentMap, connectedV, distance);
		}
		else if(treeAlgo_ == REMOVE_BD_SEARCH_MINIMUM_DIAMETER_EDGE)
		{
			OutDegreeMap::iterator big = adjacentMap.begin();
			connectedV.insert(big->second); 
			adjacentMap.erase(big);

			dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
			double path_cost = (numeric_limits<double>::max)();
			int targ = -1;
			TreeDiameter TD;
		
			int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
			ASSERTING(src > -1);
			TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
			groupT_.removeVertex(actionVert_); 	

			connectBDSearchMDE(groupT_,adjacentMap, connectedV, TD.distance, GlobalSimArgs::getDiameterBound());
		}
		else 
		{
			groupT_.removeVertex(actionVert_); 	
			connectSearchMDE(groupT_,adjacentMap, connectedV);
		}
	}
	else
	{
		//if(	treeAlgo_ == REMOVE_DC_SEARCH_MINIMUM_DIAMETER_EDGE || 
		//	treeAlgo_ == REMOVE_BR_SEARCH_MINIMUM_DIAMETER_EDGE ||
		//	treeAlgo_ == REMOVE_BD_SEARCH_MINIMUM_DIAMETER_EDGE)
		//{
		treeAlgo_ = REMOVE_TRY_REPLACE_MDDL_NAIVE;
		//}
		//else treeAlgo_ = GlobalSimArgs::removeTreeBackupAlgo();
	
		//cerr << "-------------- USING BACKUP TREEALGO ---------------    "  << treeAlgo_ << endl;

		throw true;
	}	
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(groupT_);
	// -- debug end --
}


/*-----------------------------------------------------------------------
		remove vertex:
			creating a subgraph including the neighbors of the
		vertex that is removed.
TODO: investigate further, how am I adding steiner points?
------------------------------------------------------------------------- */
void
RemoveDynamics::rvSubGraph(TreeStructure& groupT, TreeStructure& subT, VertexSet& steinerSet)
{
	EdgeList remEdges;
	VertexSet subVertices;
	subGraphRingSP(subVertices, steinerSet, actionVert_, actionVert_, actionVert_, remEdges, 2);
	subVertices = subVertices - actionVert_;

	// update groupT
	for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
		groupT.removeEdge(*eit);	
	
	groupT.removeVertex(actionVert_);

	// complete member graph
	CompleteGraph CG(subVertices, steinerSet, groupInfo_, actionVert_);
	CG.compGraph(subT, globalG_, fifoSteiner_, false);
}

/* ----------------------------------------------------------------------

					
						BELOW: Unused

				
  ----------------------------------------------------------------------- */
void
RemoveDynamics::rvReplaceWithSP()
{
	if(getOutDegree(actionVert_, groupT_.g) > 2)
	{		
		VertexSet adjacentSet;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(actionVert_, groupT_.g); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = target(*oit, groupT_.g);
			assert(targ != actionVert_);
			adjacentSet.insert(targ);
		}
		
		VertexSet newSPSet;
		findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, newSPSet, 1, GlobalSimArgs::getSimWCNAlgo());

		
		if(newSPSet.size() == 0) // all available steiner points are already in tree
		{
			groupT_.makeSteiner(actionVert_);
		}
		else // replace actionVert_ with sp_vert
		{
			ASSERTING(newSPSet.size() == 1);
			groupT_.removeVertex(actionVert_);
			vertex_descriptorN sp_vert = *(newSPSet.begin());
			groupT_.insertVertex(sp_vert, STEINER_POINT, globalG_[sp_vert].vertexFunction);
			
			VertexSet::iterator vit = adjacentSet.begin(), vit_end = adjacentSet.end();
			for( ; vit != vit_end; ++vit)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, sp_vert, groupT_.g);
				assert(!ep.second);
				groupT_.insertEdge(*vit, sp_vert, globalG_);
				groupT_.removeEdge(*vit, actionVert_);
			}
		}		
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 2)
	{
		rvODTwo();
	}
	else if(getOutDegree(actionVert_, groupT_.g) == 1)
	{
		rvLeaf();
	}
}

/*-----------------------------------------------------------------------
		remove vertex:
 			use when multiple out, creating n subtrees, connect two 
		and two trees in a greedy fashion, idea from SMT algorithms
------------------------------------------------------------------------- */
void
RemoveDynamics::rvBNP()
{
	VertexSet actVertNeighbors;
	getNeighbors(groupT_, actionVert_, actVertNeighbors);

	// remove vertex from tree, and clear out edges -> do not call remove_edge, boost graph edge implementation issue
	groupT_.removeVertex(actionVert_); 	// clears out_edges -> clear_vertex(actionVert_, g);
	
	// now: I have (actVertNeighbors.size()) subtrees
	// next: connect them using BNP idea
	multimap<double, subVertInfo> edgeRankMap;
	VertexSet connectedVerts;
	
	while(connectedVerts.size() < (actVertNeighbors.size() - 1))
	{
	
	VertexSet::iterator vit = actVertNeighbors.begin(), vit_end = actVertNeighbors.end();
	for( ; vit != vit_end; ++vit)
	{
		vertex_descriptorN actVert = *vit;
		if(connectedVerts.contains(actVert)) continue;
		
		VertexSet::iterator vit_in = actVertNeighbors.begin(), vit_in_end = actVertNeighbors.end();
		for( ; vit_in != vit_in_end; ++vit_in)
		{
			vertex_descriptorN subVert = *vit_in;

			if(subVert == actVert) continue;
			if(connectedVerts.contains(*vit_in)) continue; // already connected these components
		
			double neighborhoodRank = MAXIMUM_WEIGHT;
			int numNeighbors = 0;
			edge_descriptorN new_edge;
		
			int vert_degree = getOutDegree(actVert, groupT_.g) + getOutDegree(subVert, groupT_.g);
			int degree_constraint = min(getDegreeConstraint(globalG_, actVert), getDegreeConstraint(globalG_, subVert));

			// calculate for subVert
			if(vert_degree < degree_constraint)
			{
				calcNeighborHood(subVert, subVert, subVert, neighborhoodRank, numNeighbors, 2); 
				pair<edge_descriptorN, bool> ep = edge(actVert, subVert, globalG_);
				ASSERTING(ep.second);
				new_edge = ep.first;
			
				if(numNeighbors == 0) neighborhoodRank = globalG_[new_edge].weight; 
				else neighborhoodRank = (neighborhoodRank + globalG_[new_edge].weight)/numNeighbors;
			}
			
			// -- debug --
			//cerr << WRITE_FUNCTION << " ActVert " << actVert << " SubVert " << subVert << " worst case distance " << worstcase_dist << endl;
			//cerr << " 1st SubVert : neighborhoodRank " << neighborhoodRank << " numNeighbors " << numNeighbors << " new_edge " << new_edge << endl;
			// -- debug end --
 
			// calc for subVert subTree
			if(getOutDegree(subVert, groupT_.g) > 0) 
				findBNP(groupT_.g, actVert, subVert, subVert, neighborhoodRank, numNeighbors, new_edge); // for rest of subVert tree
			
			if(neighborhoodRank < MAXIMUM_WEIGHT)	
				edgeRankMap.insert(pair<double, subVertInfo>(neighborhoodRank, subVertInfo(new_edge, subVert, actVert)));

			//cerr << "Inserted: edgeWeight " << neighborhoodRank << " edge " << new_edge << endl;
			//cerr << "vert_degree " << vert_degree << " degree_constraint " << degree_constraint << endl;
		}
	}
	if(edgeRankMap.size())
	{
		multimap<double, subVertInfo>::iterator mmit = edgeRankMap.begin();
		subVertInfo svInfo = mmit->second;
		edge_descriptorN new_edge = svInfo.new_edge;
		double neighborhoodRank = mmit->first;
		//cerr << "Connected: NeighborhoodRank " << neighborhoodRank << " new_edge " << new_edge << endl;
	
		connectedVerts.insert(svInfo.actVert);
		edgeRankMap.clear();
		groupT_.insertEdge(new_edge, globalG_);
	}
	} // end while
	
	// remove old edges
	
	// -- debug --
	//dumpTreeStructure(groupT_);
	//char c = getchar();
	// -- debug end --
}

/*-----------------------------------------------------------------------
		remove vertex:
 			use when multiple out, creating n subtrees, connect two 
		and two trees in a greedy fashion, idea from SMT algorithms
------------------------------------------------------------------------- */
void
RemoveDynamics::rvMDBNP()
{
	VertexSet actVertNeighbors;
	getNeighbors(groupT_, actionVert_, actVertNeighbors);

	// remove vertex from tree, and clear out edges -> do not call remove_edge, boost graph edge implementation issue
	groupT_.removeVertex(actionVert_); 	// clears out_edges -> clear_vertex(actionVert_, g);
	
	// now: I have (actVertNeighbors.size()) subtrees
	// next: connect them using BNP idea
	multimap<double, subVertInfo> edgeRankMap;
	VertexSet connectedVerts;
	
	while(connectedVerts.size() < (actVertNeighbors.size() - 1))
	{
	
	VertexSet::iterator vit = actVertNeighbors.begin(), vit_end = actVertNeighbors.end();
	for( ; vit != vit_end; ++vit)
	{
		vertex_descriptorN actVert = *vit;
		if(connectedVerts.contains(actVert)) continue;
		
		VertexSet::iterator vit_in = actVertNeighbors.begin(), vit_in_end = actVertNeighbors.end();
		for( ; vit_in != vit_in_end; ++vit_in)
		{
			vertex_descriptorN subVert = *vit_in;

			if(subVert == actVert) continue;
			if(connectedVerts.contains(*vit_in)) continue; // already connected these components
		
			double neighborhoodRank = MAXIMUM_WEIGHT;
			int numNeighbors = 0;
			edge_descriptorN new_edge;
		
			int vert_degree = getOutDegree(actVert, groupT_.g) + getOutDegree(subVert, groupT_.g);
			int degree_constraint = min(getDegreeConstraint(globalG_, actVert), getDegreeConstraint(globalG_, subVert));

			// calculate for subVert
			if(vert_degree < degree_constraint)
			{
				// calculate for subVert
				calcNeighborHood(subVert, subVert, subVert, neighborhoodRank, numNeighbors, 2); 
				pair<edge_descriptorN, bool> ep = edge(actVert, subVert, globalG_);
				ASSERTING(ep.second);
				new_edge = ep.first;

				double worstcase_dist = 0;
				if(getOutDegree(subVert, groupT_.g) > 0)	
				{
					worstCaseDist(subVert, subVert, subVert, groupT_.g, worstcase_dist);
					//cerr << "worst case distance " << worstcase_dist << endl;
				}
				if(numNeighbors == 0) neighborhoodRank = globalG_[new_edge].weight + worstcase_dist;
				else neighborhoodRank = (neighborhoodRank + globalG_[new_edge].weight + worstcase_dist)/numNeighbors;
			}
	
			// -- debug --
			//cerr << WRITE_FUNCTION << " ActVert " << actVert << " SubVert " << subVert << endl; // " worst case distance " << worstcase_dist << endl;
			//cerr << " 1st SubVert : neighborhoodRank " << neighborhoodRank << " numNeighbors " << numNeighbors << " new_edge " << new_edge << endl;
			// -- debug end --
 
			// calc for subVert subTree
			if(getOutDegree(subVert, groupT_.g) > 0) 
				findMDBNP(groupT_.g, actVert, subVert, subVert, neighborhoodRank, numNeighbors, new_edge); // for rest of subVert tree

			if(neighborhoodRank < MAXIMUM_WEIGHT)
				edgeRankMap.insert(pair<double, subVertInfo>(neighborhoodRank, subVertInfo(new_edge, subVert, actVert)));
	
			//cerr << "Inserted: edgeWeight " << neighborhoodRank << " edge " << new_edge << endl;
			//cerr << "vert_degree " << vert_degree << " degree_constraint " << degree_constraint << endl;
		}
	}

		if(edgeRankMap.size())
		{
			multimap<double, subVertInfo>::iterator mmit = edgeRankMap.begin();
			subVertInfo svInfo = mmit->second;
			edge_descriptorN new_edge = svInfo.new_edge;
			double neighborhoodRank = mmit->first;
			//cerr << "Connected: NeighborhoodRank " << neighborhoodRank << " new_edge " << new_edge << endl;
	
			connectedVerts.insert(svInfo.actVert);
			edgeRankMap.clear();
			groupT_.insertEdge(new_edge, globalG_);
		}
	} // end while
	
	// remove old edges
	
	// -- debug --
	//dumpTreeStructure(groupT_);
	//char c = getchar();
	// -- debug end --
}



