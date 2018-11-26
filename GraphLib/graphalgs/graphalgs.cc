/***************************************************************************
                          graphalgs.cc  -  description
                             -------------------
    begin                : Tue Feb 28 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "graphalgs.h"
#include "../boostprop.h"
//#include "../simtime.h"
//#include "../treealgs/treealgs.h"
//#include "../treealgs/dijkstra_sp.h"
//#include "../boostprop.h"
//#include "../network/node_base.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

namespace GraphAlgorithms
{

/*-----------------------------------------------------------------------
						generate
------------------------------------------------------------------------- */
int generateRandomNumber(double max)
{ 
	return (int) (max * rand() / (RAND_MAX + 1.0));
}

int generateSizeOfSteinerSet(int members)
{
	int numToAdd = 0;

	// if fixed then return number
	if(GlobalSimArgs::getSteinerMemberSize() > 0) return GlobalSimArgs::getSteinerMemberSize();

	// else calculate with regards to degree limit and number of members
	if(ADD_CORE_LINKS_OPTIMIZED == GlobalSimArgs::getSimPruneAlgo())
	{
		if(isSteinerAlgo(GlobalSimArgs::getSimTreeAlgo()) || (GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH))
		{			
			numToAdd = (int) (ceil( (double) members/ (double) GlobalSimArgs::getDegreeConstraintSP())); // - 2) * 2) + 2;
			//numToAdd = numToAdd + (int) (ceil( (double) ((numToAdd * 2) + 2)/ (double) GlobalSimArgs::getDegreeConstraintSP()));
			numToAdd = numToAdd + (int) (ceil( (double) (numToAdd + 2)/ (double) GlobalSimArgs::getDegreeConstraintSP()));
		}
		else // not steiner algo
		{
			//cerr << WRITE_FUNCTION << " ERRORRERRORREERRRROROROEOREOREOREORO!!!!!!!! -> wrongly calculated steiner ste size " << endl;
			numToAdd = (int) (ceil( (double) members/ (double) GlobalSimArgs::getDegreeConstraint())); // - 2) * 2) + 2;
			numToAdd = numToAdd + (int) (ceil( (double) (numToAdd + 2)/ (double) GlobalSimArgs::getDegreeConstraint()));
		}
	}
	else // K_BEST_LINKS ...
	{
		if((GlobalSimArgs::getSteinerMemberRatio() > 0) && ((isSteinerAlgo(GlobalSimArgs::getSimTreeAlgo()) || (GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH))))
		{
			numToAdd = int(members * GlobalSimArgs::getSteinerMemberRatio());
		}
		else if(isSteinerAlgo(GlobalSimArgs::getSimTreeAlgo()) || (GlobalSimArgs::getSimGraphAlgo() != COMPLETE_MEMBER_GRAPH))
		{
			//cerr << WRITE_FUNCTION << " ERRORRERRORREERRRROROROEOREOREOREORO!!!!!!!! -> wrongly calculated steiner ste size " << endl;
			numToAdd = (int) (ceil( (double) members/ (double) GlobalSimArgs::getDegreeConstraintSP())); // - 2) * 2) + 2;
			numToAdd = numToAdd + (int) (ceil( (double) (numToAdd + 2)/ (double) GlobalSimArgs::getDegreeConstraintSP()));
			//numToAdd = numToAdd + (int) (ceil( (double) ((numToAdd * 2) + 2)/ (double) GlobalSimArgs::getDegreeConstraintSP()));
		}
		else
		{
			numToAdd = (int) (ceil( (double) members/ (double) GlobalSimArgs::getDegreeConstraint())); // - 2) * 2) + 2;
			numToAdd = numToAdd + (int) (ceil( (double) (numToAdd + 2)/ (double) GlobalSimArgs::getDegreeConstraint()));
		}
	}
	//cerr << WRITE_FUNCTION << " num sp to add " << numToAdd << endl;
	
	if(numToAdd <= 1) numToAdd = 2;
	return numToAdd;
}

int generateSizeOfSteinerSet(int members, int steiner)
{
	return max((generateSizeOfSteinerSet(members) - steiner), 0);
}

int generateSizeNonSteiner(int members)
{
	// if fixed then return number
	if(GlobalSimArgs::getSteinerMemberSize() > 0) return GlobalSimArgs::getSteinerMemberSize();

	int numToAdd = (int) (ceil( (double) members/ (double) GlobalSimArgs::getDegreeConstraint())); // - 2) * 2) + 2;
	numToAdd = numToAdd + (int) (ceil( (double) ((numToAdd * 2) + 2)/ (double) GlobalSimArgs::getDegreeConstraint()));
	return numToAdd;
}

double generateDelayLimit(int members)
{
	double delayLimit = (members + generateSizeOfSteinerSet(members) ) * 0.75;
	ASSERTING(delayLimit > 0);
	
	return delayLimit; 
}

/*-----------------------------------------------------------------------
Observation: Minimum number of edges to avoid partitioning a graph is:
	partitions = 2;
	(|V|/|partitions|) = n;
	nC2 = |e|; // w/ 2 partitions
	|min_edges| = |e| + 1; // w/ no partitions
------------------------------------------------------------------------- */

int generateMinEdgeCountNoPartition(int num_vertices)
{
	int partitions = 2; 		// minimum partitions in a graph
	
	int n = (int) (num_vertices/partitions) + 1;
	cerr << "n " << n << endl;
	
	int r = 2;
	int nfac = n;
	for(int i = nfac-1; i > 0; i--)
		nfac = nfac * i;
	
	cerr << "nfac " << nfac << endl;
	
	int nminrfac = (n-r);
	for(int i = nminrfac - 1; i > 0; i--)
		nminrfac = nminrfac * i;
	
	cerr << "nminrfac " << nminrfac << endl;
	
	int e = nfac/(r * nminrfac);
	
	cerr << WRITE_FUNCTION  << num_vertices << " vertices needs " << (e + 1) << " edges " << endl;
	return (e + 1);
	
	//int numberV = generateSizeOfSteinerSet(groupInfo.vertexMap_.size()) + groupInfo.vertexMap_.size();
	//int minEdgeCount = generateMinEdgeCountNoPartition(numberV);
	//double kfloat = (double) (minEdgeCount/numberV);
	//double integer;
	//double frac = modf( (double) kfloat, &integer);
	//double k = ceil(kfloat);
	//cerr << "minEdgeCount " << minEdgeCount << " kfloat " << kfloat << " frac " << frac << " integer " << integer << " k " << k << endl;
	//char c = getchar();
}


/*-----------------------------------------------------------------------
	Produces a graph tha only has vertices with out edges, and,
	the vertex/edge index != vertex/edge id
------------------------------------------------------------------------- */
void densifyGraph(const GraphN &g, GraphN& dg)
{
	//cerr << WRITE_FUNCTION << endl; print_graph(g_in, get(&VertexProp::id, g_in));

	// Old sparse Graph
	//iVertexMap idmap_g 		= get(&VertexProp::id, &g);
	//vsVertexMap vsmap_g     = get(&VertexProp::vertexState, &g);
		
	// new Dense Graph
	iVertexMap 	idmap_dg	= get(&VertexProp::id, dg);
    vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    
	// -- Adding vertices and properties --
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
    {
		if(getOutDegree(g, *vit) > 0)
    	{
    		vertex_descriptorN u = add_vertex(g[*vit], dg);
    		//cerr << "Added: "  << u << " id: " << idmap_dg[u] << " VertexState: " << dg[u].vertexState << endl;
    		//ASSERTING(dg[u].vertexState == GROUP_MEMBER || dg[u].vertexState == STEINER_POINT);
    	}
    }
    
    // -- Adding edges and properties --
    boost::tuples::tie(vit_in, vit_in_end) = vertices(dg);
    boost::tuples::tie(vit, vit_end) = vertices(dg);
    for(  ; vit != vit_end; ++vit)
    {
    	//cerr << "u: " << *vit ;
   		for( vit_in = vit; vit_in != vit_in_end; ++vit_in)
		{
			if(*vit != *vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(idmap_dg[*vit], idmap_dg[*vit_in], g);
				if(ep.second)
				{
					ASSERTING(ep.second);
					pair<edge_descriptorN, bool> e = add_edge(*vit, *vit_in, dg);
					ASSERTING(e.second);
					dg[e.first] = g[ep.first];
			
					// -- debug --
					//cerr << " v: " << *vit_in << endl;
					//cerr << "Found e(g): "  << ep.first << " (" << g[ep.first].id.first << "," << g[ep.first].id.second << ")" << endl;					
					//cerr << "Added e(dg): "  << e.first << " (" << dg[e.first].id.first << "," << dg[e.first].id.second << ")" << endl;					
					// -- debug end --
				}
			}
		}
    }
	
	// -- debug --
	cerr << WRITE_FUNCTION << endl; 
	//print_graph(dg, get(&VertexProp::id, dg));
	//dumpGraph(dg);
	checkGraph(dg);	
	//char c = getchar();
}

void densifyGraph(const TreeStructure &T, GraphN& dg)
{
	// -- Adding vertices and properties --
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		if(getOutDegree(T.g, *vit) > 0)
    		vertex_descriptorN u = add_vertex(T.g[*vit], dg);
    
    // -- Adding edges and properties --
	iVertexMap 	idmap_dg	= get(&VertexProp::id, dg);
	vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    
    boost::tuples::tie(vit_in, vit_in_end) = vertices(dg);
    boost::tuples::tie(vit, vit_end) = vertices(dg);
    for(  ; vit != vit_end; ++vit)
    {
    	//cerr << "u: " << *vit ;
   		for( vit_in = vit; vit_in != vit_in_end; ++vit_in)
		{
			if(*vit != *vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(idmap_dg[*vit], idmap_dg[*vit_in], T.g);
				if(ep.second)
				{
					ASSERTING(ep.second);
					pair<edge_descriptorN, bool> e = add_edge(*vit, *vit_in, dg);
					ASSERTING(e.second);
					dg[e.first] = T.g[ep.first];
			
					// -- debug --
					//cerr << " v: " << *vit_in << endl;
					//cerr << "Found e(T.g): "  << ep.first << " (" << T.g[ep.first].id.first << "," << T.g[ep.first].id.second << ")" << endl;					
					//cerr << "Added e(dg): "  << e.first << " (" << dg[e.first].id.first << "," << dg[e.first].id.second << ")" << endl;					
					// -- debug end --
				}
			}
		}
    }
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl; 
	//dumpGraph(dg);
	checkGraph(dg);	
}

/*-----------------------------------------------------------------------
	Produces a graph that may have vertices with no out-edges, and,
	that has vertex/edge index == vertex/edge id
------------------------------------------------------------------------- */
void sparsifyGraph(const GraphN &g, GraphN& dg)
{
	//cerr << WRITE_FUNCTION << endl; print_graph(g_in, get(&VertexProp::id, g_in));

	// Old dense Graph
	iVertexMapConst idmap_g 		= get(&VertexProp::id, g);
	vsVertexMapConst vsmap_g     = get(&VertexProp::vertexState, g);
		
	// new sparse Graph
	iVertexMap 	idmap_dg	= get(&VertexProp::id, dg);
    vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    
    // -- Adding vertices, edges and properties --
    boost::tuples::tie(vit_in, vit_in_end) = vertices(g);
    boost::tuples::tie(vit, vit_end) = vertices(g);
    for(  ; vit != vit_end; ++vit)
    {
    	//cerr << "u: " << *vit ;
   		for( vit_in = vit; vit_in != vit_in_end; ++vit_in)
		{
			if(*vit != *vit_in)
			{
				//pair<edge_descriptorN, bool> ep = edge(idmap_dg[*vit], idmap_dg[*vit_in], g);
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(ep.second)
				{
					ASSERTING(ep.second);
					pair<edge_descriptorN, bool> e = add_edge(idmap_g[*vit], idmap_g[*vit_in], dg);
					ASSERTING(e.second);
					dg[e.first] = g[ep.first];
			
					// adding vertex properties
					dg[idmap_g[*vit]] = g[*vit];
					dg[idmap_g[*vit_in]] = g[*vit_in];
			
					// -- debug --
					//cerr << " v: " << *vit_in << endl;
					//cerr << "Found v(g): " << *vit << ", " << *vit_in << endl;
					//cerr << "Added v(dg): " << idmap_g[*vit] << ", " << idmap_g[*vit_in] << endl;
					//cerr << "Found e(g): "  << ep.first << " (" << g[ep.first].id.first << "," << g[ep.first].id.second << ")" << endl;
					//cerr << "Added e(dg): "  << e.first << " (" << dg[e.first].id.first << "," << dg[e.first].id.second << ")" << endl;					
					// -- debug end --
				}
			}
		}
    }
	
	// -- debug --
	//cerr << WRITE_FUNCTION << endl; 
	//print_graph(dg, get(&VertexProp::id, dg));
	//dumpGraph(dg);
	checkGraph(dg);	
	//char c = getchar();
}
/*-----------------------------------------------------------------------
				checks the validity of the graph
------------------------------------------------------------------------- */
void checkGraph(const GraphN &g)
{

#ifndef NDEBUG

	ASSERTING(num_edges(g) > 0);
	ASSERTING(num_vertices(g) > 1);

	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0)
		{
			if(g[*vit].vertexFunction == NO_FUNCTION || g[*vit].vertexState == VERTEX_STATE_ERROR)
			{
				cerr << WRITE_FUNCTION << "Node : " << *vit << " " << g[*vit] << endl;
				//dumpGraph(g); 
			}
			ASSERTING(g[*vit].vertexState != VERTEX_STATE_ERROR);
			ASSERTING(g[*vit].vertexFunction != NO_FUNCTION);
			ASSERTING(g[*vit].id > -1);

			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
			{
				if(g[*oit].weight <= 0 ||
					g[*oit].cost <= 0 ||
					g[*oit].delay <= 0 ||
					g[*oit].id.first == g[*oit].id.second)
					//g[*oit].weight > GlobalSimArgs::maxEdgeDelay_)
				{

					vertex_descriptorN src = source(*oit, g), targ = target(*oit, g);
					pair<edge_descriptorN, bool> ep = edge(src, targ, g);
					if(ep.second) cerr << "(" << src << "," << targ << ") edge prop : " << g[ep.first] << endl;

					//dumpGraph(g); 
					cerr << WRITE_FUNCTION << "node: " << *vit << g[*vit] << " edge : " << *oit << " : " <<  g[*oit] << endl;
					cerr << WRITE_FUNCTION << " out degree : " << getOutDegree(*vit, g) << " edges : " << endl;
			
					out_edge_iteratorN oit_in, oit_in_end;
                    for(boost::tuples::tie(oit_in, oit_in_end) = out_edges(*vit, g); oit_in != oit_in_end; ++oit_in)
						cerr << "(" << *oit_in << "," << g[*oit_in] << ") " ;

					cerr << endl;
				}
				ASSERTING(g[*oit].weight > 0);
				ASSERTING(g[*oit].cost  > 0);
				ASSERTING(g[*oit].delay > 0);
				ASSERTING(g[*oit].id.first != g[*oit].id.second);
				ASSERTING(g[*oit].weight <= GlobalSimArgs::maxEdgeDelay_);
			}

			/*if(g[*vit].obj != NULL)
			{
				NodeBase *nob = g[*vit].obj;
				if(nob->getId() != *vit)
				{
					cerr << WRITE_FUNCTION << "Node : " << g[*vit] << endl;
				}

				ASSERTING(nob->getId() == *vit);
			}*/
		}
		else if(getOutDegree(g, *vit) <= 0)
		{
			if(g[*vit].vertexState == STEINER_POINT || g[*vit].vertexState == GROUP_MEMBER)
			{
				cerr << WRITE_FUNCTION << "Node : " << g[*vit] << endl;
				//dumpGraph(g); 
			}			
			ASSERTING(g[*vit].vertexState != STEINER_POINT);
			ASSERTING(g[*vit].vertexState != GROUP_MEMBER);
		}
	}
	
	checkIsPartitioned(g);
	//checkMaxDistance(g);
#endif

}

void checkGraphEqualSP(const GraphN &g)
{
#ifndef NDEBUG
	ASSERTING(num_edges(g) > 0);
	ASSERTING(num_vertices(g) > 1);

	vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{			
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			double shortest_path_weight = g[*oit].weight;
			unsigned int targ = target(*oit, g);

            for(boost::tuples::tie(vit_in, vit_in_end) = vertices(g); vit_in != vit_in_end; ++vit_in)
			{	
				if(*vit_in == *vit) continue;
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				ASSERTING(ep.second);
	
				if(*vit_in == targ) continue;
				
				pair<edge_descriptorN, bool> ep_in = edge(*vit_in, targ, g);
				ASSERTING(ep_in.second);
				
				// (*vit -> targ) == (*vit -> *vit_in -> targ)?
				double indirect_path_weight = g[ep.first].weight + g[ep_in.first].weight;
				if(shortest_path_weight == indirect_path_weight)
				{
					cerr << WRITE_PRETTY_FUNCTION << " shortest_path_weight : " << shortest_path_weight << " == indirect_path_weight " << indirect_path_weight << " Edge : " << g[*oit] << " == path: " << g[ep.first] << " " << g[ep_in.first] << endl;
				}
				else if(shortest_path_weight > indirect_path_weight)
				{
					cerr << WRITE_PRETTY_FUNCTION << "ERROR!! shortest_path_weight : " << shortest_path_weight << " > indirect_path_weight " << indirect_path_weight << " Edge : " << g[*oit] << " == path: " << g[ep.first] << " " << g[ep_in.first] << endl;
				}

			}
		}
	}
#endif
}

int cntSPViolations(const GraphN &g)
{
	ASSERTING(num_edges(g) > 0);
	ASSERTING(num_vertices(g) > 1);

	int num_of_violations = 0;
	
	vertex_iteratorN vit, vit_end, vit_in, vit_in_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{			
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			double shortest_path_weight = (double)g[*oit].weight;
			unsigned int targ = target(*oit, g);

            //for(boost::tuples::tie(vit_in, vit_in_end) = vertices(g); vit_in != vit_in_end; ++vit_in)
			for(vit_in = vit, vit_in_end = vit_end; vit_in != vit_in_end; ++vit_in)
			{	
				if(*vit_in == *vit) continue;
				if(*vit_in == targ) continue;
				
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				pair<edge_descriptorN, bool> ep_in = edge(*vit_in, targ, g);
				if(!ep_in.second) continue;
				
				// (*vit -> targ) == (*vit -> *vit_in -> targ)?
				double indirect_path_weight = (double) g[ep.first].weight + (double) g[ep_in.first].weight;
				
				//if((float) indirect_path_weight < (float) shortest_path_weight)
				if(shortest_path_weight - indirect_path_weight > 0.0001 && shortest_path_weight < 2)
				{
					double diff = (float)indirect_path_weight - (float)shortest_path_weight;

					num_of_violations++;
					//cerr << WRITE_PRETTY_FUNCTION << "not sp edge: " << *oit << " sp weight : " << shortest_path_weight << " > indirect " << indirect_path_weight << " Edge : " << g[*oit] << " > path: " << g[ep.first] << " " << g[ep_in.first] << endl;
					//printf(" shortest_path_weight : %f indirect_path_weight %f\n", shortest_path_weight,  indirect_path_weight);
					//ASSERTING( indirect_path_weight < shortest_path_weight );
					//ASSERTING( indirect_path_weight == shortest_path_weight );

					break;
				}
			}
		}
	}

	return num_of_violations;
}

int cntInvalidEdgeWeights(const GraphN &g, int invalid_weight)
{
	int counter = 0;
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(g); eit !=eit_end; ++eit)
		if(g[*eit].weight >= invalid_weight) counter++;
		
	return counter;
}

int cntInvalidEdgeWeights(const GraphN &g, const VertexSet &invalidV, int invalid_weight)
{
	int counter = 0;
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(g); eit !=eit_end; ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		if(invalidV.contains(src) || invalidV.contains(targ)) continue;

		if(g[*eit].weight >= invalid_weight) counter++;
	}
		
	return counter;
}


int cntInvalidVertices(const GraphN &g, VertexSet &invalidVertices, int invalid_weight)
{
	int counter = 0;
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		int cnt_invalid = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			if(g[*oit].weight >= invalid_weight)
				cnt_invalid++;
		}

		if(cnt_invalid >= (out_degree(*vit, g) - 4)) 
		{
			counter++;
			invalidVertices.insert(*vit);
		}
	}

	return counter;
}

/*-----------------------------------------------------------------------
	connects a partitioned graph through shortest paths
------------------------------------------------------------------------- */
void connectPartitionedGraph(const TreeStructure &inputT, TreeStructure &newT)
{
	// ensure graph is connected
	VertexSet partV;
	while(findPartition(newT.g, partV))
	{
		bool inserted_edge = false;
		for(VertexSet::const_iterator vi = partV.begin(), vi_end = partV.end(); vi != vi_end; ++vi)
		{
			int src = *vi;
			std::multimap<double, edge_descriptorN> addEdges;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(src, inputT.g); oit != oit_end; ++oit)
				addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
	
			for(std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end(); mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(src != targ);

				if(partV.contains(targ)) continue;
	
				if(newT.isEdge(src, targ) == false) 
				{
					newT.insertEdge(src, targ, inputT.g);
					inserted_edge = true;
					break;
				}
			}

			if(inserted_edge == true) break;
		}
		
		partV.clear();
	}
}

/*-----------------------------------------------------------------------
	finds out whether the graph is partitioned
------------------------------------------------------------------------- */
bool findPartition(const GraphN &g, VertexSet &partV)
{
	vector<bool> visited(num_vertices(g));
	
	// init
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		visited[*vit] = false;	
	}
	
	// start walk 
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0)
		{
			walkGraph(g, *vit, visited);
			break;
		}
	}
	
	// check
	bool partition = false;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(g[*vit].id > -1 && visited[*vit] == false)
		{
			//if(!partition) cerr << WRITE_FUNCTION << " Partition (" << *vit ;
			//else cerr << ", " << *vit;

			partV.insert(*vit);
			partition = true;
		}
	}
	
	//if(partition) cerr << ")" << endl;
	
	return partition;
}

/*-----------------------------------------------------------------------
	finds out whether the graph is partitioned
------------------------------------------------------------------------- */
bool checkIsPartitioned(const GraphN &g)
{
	vector<bool> visited(num_vertices(g));
	
	// init
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		visited[*vit] = false;	
	}
	
	// start walk 
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0)
		{
			walkGraph(g, *vit, visited);
			break;
		}
	}
	
	// check
	bool connected = true;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		//if(getOutDegree(*vit, g) > 0)
		//{
			if(g[*vit].id > -1 && visited[*vit] == false)
			{
				if(connected) cerr << WRITE_FUNCTION << " Partition (" << *vit ;
				else cerr << ", " << *vit;
				
				connected = false;
			}
			//ASSERTING(visited[*vit] == true);
		//}
	}
	
	if(!connected)
		cerr << ")" << endl;
	
	return connected;
}

void walkGraph(const GraphN &g, vertex_descriptorN u, vector<bool> &visited)
{
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
	{
		vertex_descriptorN v = target(*oit, g);
		ASSERTING(u != v);
		if(visited[v] == false)
		{
			visited[v] = true;
			walkGraph(g, v, visited);	
		}		
	}		
}

void checkMaxDistance(const GraphN &g)
{
#ifndef NDEBUG
	double maxDistance = 0;
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
		
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			if(wmap[*oit] > maxDistance) maxDistance = wmap[*oit];
			//if(g[*oit].weight > maxDistance) maxDistance = g[*oit].weight;

		}
	}	
	
	//cerr << "Max Distance " << maxDistance << endl;
	GlobalSimArgs::maxEdgeDelay_ = maxDistance;
#endif // NDEBUG
}



}; // namespace GraphAlgorithms


