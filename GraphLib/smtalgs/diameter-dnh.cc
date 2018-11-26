/***************************************************************************
                          diameter-dnh.cc  -  description
                             -------------------
    begin                : Sat Jul 14 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "diameter-dnh.h"
#include "../treealgs/prim_ottc.h"
#include "../treealgs/prim_rgh_bdmst.h"
#include "../treealgs/prim_mst.h"
#include "../treealgs/dijkstra_sp.h"
#include "../treealgs/treestructure.h"
#include <fstream>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/* -------------------------------------------------------------------------
	
		Algorithm(): Distance Network Heuristic start function

------------------------------------------------------------------------- */
void
DiameterDNH::Algorithm(vertex_descriptorN zsource)
{
	Initialize(zsource);	// identify z-nodes and store them in vector<MyWrapper> ZVert
	if(num_zvertices <= 0)
	{
		cout << "[diameter-DNH::Algorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << WRITE_FUNCTION << " Start " << GlobalSimArgs::getSimTreeAlgo() << "  with zsource " << zsource << " and " << num_zvertices << " z-node(s)" << endl;
	
	// -- Important DNH Algorithm variables --
	GraphN 	dn_graph(num_vertices(g)), temp_sp_graph, final_dnh_graph;
	ShortestPathKeeper 	dng_spk, dnh_mst;
	VertexSet totV;

	// -- Start DNH Algorithm --
	T_dnh.insertVertex(zsource, g);									// -1. -- Add source to the DNH tree
	RunDijkstraForEveryZ();											// 0. -- Find SP for every z-node and store in SPKeeper
	ConstructDNGraph(dn_graph); 					 				// 1. -- Construct a Distance Network from g, solving n shortest path problems.
	bool ret = FindST(ZVertSet, dn_graph, zsource, dng_spk);		// 2. -- Find the Minimum Spanning Tree of the dn_graph.
	if(!ret) return;
	ConstructSPGraph(dn_graph, temp_sp_graph, dng_spk); 			// 3. -- Construct Shortest Path GraphN: Replaces each edge in DNG with a corresponding shortest path -> stored in temp_sp_graph
	ConstructDNHeuristicGraph(final_dnh_graph, temp_sp_graph,totV); // 4. -- Construct DNH-GraphN excluding unused graph-nodes. Exclude: Nodes that are not used as steiner points, or is not a z-node
	ret = FindST(totV, final_dnh_graph, zsource, dnh_mst);			// 5. -- Find Minimum Spanning Tree from resulting dnh_graph 
	if(!ret) return;
	ConstructDNHTree(final_dnh_graph, dnh_mst);						// 6. -- Add edges and vertices to T_dnh

	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_dnh.print();
	//T_dnh.printVertexState(cerr);
	// -- end debug --	
}

/* -------------------------------------------------------------------------

	ConstructDNHTree(): Constructs the T_dnh by extracting the edges and
			vertices from dnh_graph using dnh_mst.zparent. 
		
 --------------------------------------------------------------------------*/
void
DiameterDNH::ConstructDNHTree(GraphN &dnh_graph, const ShortestPathKeeper &dnh_mst)
{
	iVertexMap dnhg_idmap = get(&VertexProp::id, dnh_graph);

	pair<vertex_iteratorN, vertex_iteratorN> vpit;
	for(vpit = vertices(dnh_graph); vpit.first != vpit.second; ++vpit.first)
	{
		vertex_descriptorN v 		= *vpit.first;
		vertex_descriptorN parent_v  = dnh_mst.zparent[dnhg_idmap[v]];
	
		pair<edge_descriptorN, bool> e = edge(v, parent_v, dnh_graph);
		if(e.second) // Is Edge? then add edge and vertex to tree
		{	
			//cerr << "inserting (" << v << "," << parent_v << ")" << endl;
			
			T_dnh.insertVertex(v, g);
			T_dnh.insertVertex(parent_v, g);
			T_dnh.insertEdge(v, parent_v, g);
		}
	}
	// Next: Prune non-terminals of degree 1
	TreeAlgorithms::pruneNonTerminalLeaves(T_dnh);
}


/* -------------------------------------------------------------------------

	ConstructSPGraph(): Constructs a Shortest path graph from the
		MST of the distance network graph
		
	I have:	GraphN dn_graph with distance network graph including original 
		id's of the vertices dng_spk: holding parent and distance 
		information

 --------------------------------------------------------------------------*/
void
DiameterDNH::ConstructSPGraph(const GraphN &dn_graph, GraphN &temp_sp_graph, const ShortestPathKeeper &dng_spk)
{
	for(vector<MyWrapper>::iterator zit = ZVert.begin(), zit_end = ZVert.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN curr_vert = zit->v;
		ShortestPathKeeper &spk = findSPMaps(curr_vert);	// find the shortest path maps for the original vertex_descriptorN from g
		ASSERTING(spk.zid == (int)curr_vert);
		
		// Now:  I have ShortestPathKeeper of original vertex from g which is the same node as 
		//		 the current vertex iterator in dn_graph
		// Next: Find the parent of the current vertex iterator from dn_graph, and link them
		//		 using the ShortestPathKeepr of original vertex from g
		
		vertex_descriptorN traverse_vert = dng_spk.zparent[curr_vert];
		// traverse from parent -> *(vit.first) (dn_graph's current vertex iterator)
		while(traverse_vert != curr_vert) 
		{
			//Next:	add edge traverse_vert spk.zparent[g_traverse_vert] to temp_sp_graph
			pair<edge_descriptorN, bool> newEdge = add_edge(traverse_vert, spk.zparent[traverse_vert], temp_sp_graph);
			ASSERTING(newEdge.second);
			traverse_vert = spk.zparent[traverse_vert];	
		}
	}
}

/* -------------------------------------------------------------------------

	ConstructDNHGraph(): Constructs the DNH Steiner-tree from the
		shortest path graph from ConstructSPGraph()
		
	Now:	I have a temp_sp_graph that is a steiner-tree, but, not the 
			final according to the dnh specifications
	Next:	Copy g to dnh_graph. Delete all the vertices in dnh_graph with 
			no outgoing links in temp_sp_graph. This produces a graph with 
			only the involved z-nodes and steiner-points. Then run MST 
			and get the final dnh-steiner-tree

 --------------------------------------------------------------------------*/
void
DiameterDNH::ConstructDNHeuristicGraph(GraphN &dnh_graph, GraphN &temp_sp_graph, VertexSet &V)
{
	dnh_graph = g;				// copy original graph g to dnh_graph
	
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, temp_sp_graph) <= 0)
			clear_vertex(*vit, dnh_graph);
		else V.insert(*vit);
	}
}


/* -------------------------------------------------------------------------

	ConstructDNGraph(): Constructs a Distance Network GraphN from g 
		-> 	solving n shortest path problems (already done in 
			RunPrimForEveryZ)

 --------------------------------------------------------------------------*/
void
DiameterDNH::ConstructDNGraph(GraphN &dn_graph)
{
    dEdgeMap dng_weightmap   = get(&EdgeProp::weight, dn_graph);
	eidEdgeMap dng_edgeidmap = get(&EdgeProp::id, dn_graph);
	
	vector<MyWrapper>::iterator zit, zit_end, zit_dest, zit_dest_end;
	for(zit = ZVert.begin(), zit_end = ZVert.end(); zit != zit_end; ++zit)		// iterates through the z-nodes in g
	{
		ShortestPathKeeper &spk = findSPMaps(zit->v);
		dn_graph[zit->v] = g[zit->v];

		for(zit_dest = zit, zit_dest_end = ZVert.end(); zit_dest != zit_dest_end; ++zit_dest)		// add links from zit to zit_dest
		{	
			if(zit->v == zit_dest->v) continue;
			dn_graph[zit_dest->v] = g[zit_dest->v];
			
			ASSERTING(! ( isEdge(dn_graph, zit->v, zit_dest->v) ) );
				
			pair<edge_descriptorN, bool> newEdge = add_edge(zit->v, zit_dest->v, dn_graph);
			ASSERTING(newEdge.second);
						
			dn_graph[newEdge.first].weight 	= spk.zdistance[(*zit_dest).v];
			dn_graph[newEdge.first].id 		= EdgeId(zit->v, zit_dest->v);
		}
	}
}

/* -------------------------------------------------------------------------

	FindMST(): Finds the Minimum Spanning Tree for input_graph, with
		zsource as src. Stores zparent, and zdistance in spk. 

 --------------------------------------------------------------------------*/
bool
DiameterDNH::FindST(const VertexSet &V, GraphN &input_graph, vertex_descriptorN zsource, ShortestPathKeeper &spk)
{
	spk.zdistance 	= DistanceVector(num_vertices(input_graph));
	spk.zparent   	= ParentVector(num_vertices(input_graph));
	spk.zid 		= zsource;
	
	//cerr << WRITE_FUNCTION << GlobalSimArgs::getTreeAlgo() << endl;
	//TreeAlgorithms::dumpGraph(input_graph);
	
	switch(GlobalSimArgs::getSimTreeAlgo())
	{
		case DISTANCE_NETWORK_HEURISTIC:
			TreeAlgorithms::myPrimMST(input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			return true;
			break;
		case DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC:
			return TreeAlgorithms::myDBPrimMST(input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case DEGREE_LIMITED_SHORTEST_PATH_DISTANCE_NETWORK_HEURISTIC :
			return TreeAlgorithms::myDBDijkstraSPT(input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case BOUNDED_DIAMETER_RGH_DISTANCE_NETWORK_HEURISTIC :
			return RGH(GlobalSimArgs::getDiameterBound(), input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_DISTANCE_NETWORK_HEURISTIC :
			return dlRGH(GlobalSimArgs::getDiameterBound(), input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case BOUNDED_DIAMETER_DISTANCE_NETWORK_HEURISTIC:
			return OTTC(GlobalSimArgs::getDiameterBound(), input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case BOUNDED_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC:
			return dlOTTC(GlobalSimArgs::getDiameterBound(), input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case MINIMUM_DIAMETER_DISTANCE_NETWORK_HEURISTIC:
			return mdOTTC(input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
		case MINIMUM_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC:
			return mddlOTTC(input_graph, spk.zid, spk.zdistance, spk.zparent, V);
			break;
	}

	return false;
}

/* -------------------------------------------------------------------------

	DNHTest(): Unused: Only used while debugging DNH

 --------------------------------------------------------------------------*/
/*
void
DiameterDNH::DNHTest(GraphN &dn_graph)
{
 	// -- Adding edge properties --
    dEdgeMap weightmap = get(&EdgeProp::weight, dn_graph);
    
	edge_descriptorN e = add_edge(0,1, dn_graph).first;
	weightmap[e] = 4; 		
	e = add_edge(0,2, dn_graph).first;
	weightmap[e] = 5; 		
	e = add_edge(1,2, dn_graph).first;
	weightmap[e] = 7; 		
   
	// -- Adding vertex properties --
    iVertexMap idmap = get(&VertexProp::id, dn_graph);
	graph_traits<GraphN>::vertex_iterator vi, vi_end;
	int i = 0;
    for(tie(vi, vi_end) = vertices(dn_graph); vi != vi_end; ++vi)
    {
		idmap[*vi] = i;
		i++;
    }
}
*/


/* -------------------------------------------------------------------------

	ConstructSPGraph(): Constructs a Shortest path graph from the
		MST of the distance network graph
		
	I have:	GraphN dn_graph with distance network graph including original 
		id's of the vertices dng_spk: holding parent and distance 
		information

 --------------------------------------------------------------------------*/
/*void
DiameterDNH::ConstructSPGraph(const GraphN &dn_graph, GraphN &temp_sp_graph, const ShortestPathKeeper &dng_spk)
{
	typedef property_map<GraphN, vertex_index_t>::type IndexMap;
    IndexMap g_index 			= get(vertex_index, g);
    iVertexMapConst g_idmap		= get(&VertexProp::id, g);

    IndexMap dng_index 			= get(vertex_index, dn_graph);
	iVertexMapConst dng_idmap 	= get(&VertexProp::id, dn_graph);
	
	pair<vertex_iteratorN, vertex_iteratorN> vit;
	for(vit = vertices(dn_graph); vit.first != vit.second; ++vit.first)
	{
		vertex_descriptorN dng_curr_vert = *vit.first;					// for easier read
		int id_dng_curr_vert 			= dng_idmap[dng_curr_vert];		// get original id of dng vertex
		vertex_descriptorN g_curr_vert 	= g_idmap[id_dng_curr_vert];	// get original vertex_descriptorN from g

		ShortestPathKeeper &spk = findSPMaps(g_curr_vert);	// find the shortest path maps for the original vertex_descriptorN from g
		ASSERTING(spk.zid == g_curr_vert);
		
		// Now: 		I have ShortestPathKeeper of original vertex from g which is the same node as 
		//				the current vertex iterator in dn_graph
		// Next: 		Find the parent of the current vertex iterator from dn_graph, and link them
		//				using the ShortestPathKeepr of original vertex from g
		
		vertex_descriptorN parent_dng_curr_vert	= dng_spk.zparent[dng_curr_vert];
		int id_parent_dng_curr_vert				= dng_idmap[parent_dng_curr_vert];
		vertex_descriptorN g_traverse_vert 		= g_idmap[id_parent_dng_curr_vert];
		
		while(g_idmap[g_traverse_vert] != g_idmap[g_curr_vert])	// traverse from parent_dng_curr_vert -> dng_curr_vert
		{
			//Next:		add edge g_traverse_vert spk.zparent[g_traverse_vert] to dn_graph
			pair<edge_descriptorN, bool> newEdge = add_edge(g_idmap[g_traverse_vert], g_idmap[spk.zparent[g_traverse_vert]], temp_sp_graph);
			ASSERTING(newEdge.second);
			g_traverse_vert = spk.zparent[g_traverse_vert];	
		}
	}
}*/
/* -------------------------------------------------------------------------

	ConstructDNGraph(): Constructs a Distance Network GraphN from g 
		-> 	solving n shortest path problems (already done in 
			RunPrimForEveryZ)

 --------------------------------------------------------------------------*/
/*void
DiameterDNH::ConstructDNGraph(GraphN &dn_graph, vertex_descriptorN gz_source, vertex_descriptorN &dngz_source)
{
    iVertexMapConst idmap 	= get(&VertexProp::id, g);

    dEdgeMap DNG_weightmap   = get(&EdgeProp::weight, dn_graph);
	eidEdgeMap DNG_edgeidmap = get(&EdgeProp::id, dn_graph);
	iVertexMap DNG_idmap 	 = get(&VertexProp::id, dn_graph);
	ShortestPathKeeper		   spk;
	
    IndexMap DNG_index = get(vertex_index, dn_graph);
	pair<edge_descriptorN, bool> newEdge;
	
	pair<vertex_iteratorN, vertex_iteratorN> vitp_src = vertices(dn_graph), vitp_dest = vertices(dn_graph); 
	vector<MyWrapper>::iterator zit, zit_end, zit_dest;
	for(zit = ZVert.begin(), zit_end = ZVert.end(); zit != zit_end; ++zit)		// iterates through the z-nodes in g
	{
		ShortestPathKeeper &spk = findSPMaps(zit->v);		//findSPMaps((*zit).v, spk);
		DNG_idmap[*vitp_src.first] = idmap[(*zit).v];		// give original node-id to dn_graph

		// remember z-source in dn_graph
		if(idmap[(*zit).v] == idmap[gz_source]) dngz_source = *vitp_src.first;

		vitp_dest = vertices(dn_graph);		
		for(zit_dest = ZVert.begin(); zit_dest != ZVert.end(); ++zit_dest)		// add links from zit to zit_dest
		{	
			if( !(idmap[(*zit_dest).v] == idmap[(*zit).v]) && 	// no link to itself
				!(((edge(*vitp_src.first, *vitp_dest.first,dn_graph).second) || (edge(*vitp_dest.first, *vitp_src.first, dn_graph).second)))) // no new edge if it already exist
			{
				DNG_idmap[*vitp_dest.first] 	= idmap[(*zit_dest).v];
				newEdge 						= add_edge(*vitp_src.first, *vitp_dest.first, dn_graph);				
				DNG_weightmap[newEdge.first] 	= spk.zdistance[(*zit_dest).v];
				DNG_edgeidmap[newEdge.first]    = EdgeId(zit->v, zit_dest->v);
				
				ASSERTING(spk.zdistance[(*zit_dest).v]);
				ASSERTING(newEdge.second);
			}
			++vitp_dest.first;
		}
		++vitp_src.first;
	}
}*/

