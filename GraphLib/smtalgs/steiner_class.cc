/***************************************************************************
                          steiner_class.cc  -  description
                             -------------------
    begin                : Wed Sep 14 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
 
#include "steiner_class.h"
#include "../treealgs/dijkstra_sp.h"
#include "../treealgs/prim_mst.h"
#include <fstream>
#include "../simtime.h"
//#include "treealgs.h"
 
using namespace boost;
using namespace std;
using namespace TreeAlgorithms;
 
/* -------------------------------------------------------------------------
	
		Initialize(): extracts z-nodes and stores them in a vector/list

------------------------------------------------------------------------- */
void
SteinerHeuristic::Initialize(vertex_descriptorN zsource)
{
    iVertexMapConst idmap 	= get(&VertexProp::id, g);
	vsVertexMapConst vsmap	= get(&VertexProp::vertexState, g);
	
	if(zsource != NO_SOURCE_NODE) ASSERTING(vsmap[zsource] == GROUP_MEMBER);
	
	//cerr << WRITE_FUNCTION << " Input Nodes: ";	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
    {
		if(vsmap[*vi] == GROUP_MEMBER)
		{
			ZVertSet.insert(*vi);
			ZVert.push_back(MyWrapper(idmap[*vi], idmap[*vi], *vi));
			num_zvertices++;
		}
		//cerr << *vi << " " << vsmap[*vi] << endl; //", ";
		
		// -- debug --
		ASSERTING(getOutDegree(*vi, g) > 0);
		// -- debug --
    }
	//cerr << endl;
}

/* -------------------------------------------------------------------------

	RunPrimForEveryZ():	Runs Prim MST for every Z-node in GraphN g

------------------------------------------------------------------------- */
void
SteinerHeuristic::RunPrimForEveryZ()
{
	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		DistanceVector 	zdistance(num_vertices(g));
		ParentVector 	zparent(num_vertices(g));

		//prim_minimum_spanning_tree(g, (*mwit).v, &zparent[0], &zdistance[0], get(&EdgeProp::weight, g), get(vertex_index, g), default_dijkstra_visitor()); 
		myPrimMST(g, *vit, zdistance, zparent, inputT.V);
	
		SPKeeper[*vit] = ShortestPathKeeper(zdistance, zparent, *vit);
	}
}

/* -------------------------------------------------------------------------

	RunDijkstraForEveryZ():	Runs Prim MST for every Z-node in GraphN g

------------------------------------------------------------------------- */
void SteinerHeuristic::RunDijkstraForEveryZ()
{
	ASSERTING(!ZVertSet.empty());

	clock_t start_time = ::clock();

	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		DistanceVector 	zdistance(num_vertices(g));
		ParentVector 	zparent(num_vertices(g));

		myDijkstraSPT_TieBr(g, *vit, zdistance, zparent, inputT.V);
		SPKeeper[*vit] = ShortestPathKeeper(zdistance, zparent, *vit);
	}
	
	overhead_exec_time_ = overhead_exec_time_ + (::clock() - start_time); 	// execution time
}

void SteinerHeuristic::RunDijkstraForEveryZ(const GraphN &g)
{
	clock_t start_time = ::clock();

	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		DistanceVector 	zdistance(num_vertices(g));
		ParentVector 	zparent(num_vertices(g));

		myDijkstraSPT_TieBr(g, *vit, zdistance, zparent, inputT.V);
		SPKeeper[*vit] = ShortestPathKeeper(zdistance, zparent, *vit);
	}
	
	overhead_exec_time_ = overhead_exec_time_ + (::clock() - start_time); 	// execution time
}

void SteinerHeuristic::RunDijkstraForEveryNode()
{
	ASSERTING(!ZVertSet.empty());

	clock_t start_time = ::clock();

	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		DistanceVector 	zdistance(num_vertices(g));
		ParentVector 	zparent(num_vertices(g));

		myDijkstraSPT_TieBr(g, *vit, zdistance, zparent, inputT.V);
		SPKeeper[*vit] = ShortestPathKeeper(zdistance, zparent, *vit);
	}
	
	overhead_exec_time_ = overhead_exec_time_ + (::clock() - start_time); 	// execution time
}

/* -------------------------------------------------------------------------

	CallPrimMST():	Finds parent and distance maps for znode

------------------------------------------------------------------------- */
void
SteinerHeuristic::CallPrimMST(const GraphN &g, vertex_descriptorN znode, ParentVector &zparent, DistanceVector &zdistance)
{	
	//prim_minimum_spanning_tree(g, znode, &zparent[0], &zdistance[0], get(&EdgeProp::weight, g), get(vertex_index, g), default_dijkstra_visitor()); 
	myPrimMST(g, znode, zdistance, zparent);
}

/* -------------------------------------------------------------------------

	findPrimMaps():	Finds parent and distance maps in struct 
		ShortestPathKeeper
	
	TODO: Should be more efficient
------------------------------------------------------------------------- */
void
SteinerHeuristic::findSPMaps(vertex_descriptorN src, ShortestPathKeeper &spk)
{
	spk = SPKeeper[src];
}

ShortestPathKeeper&
SteinerHeuristic::findSPMaps(vertex_descriptorN src)
{
	return SPKeeper[src];
}

void
SteinerHeuristic::dumpSteinerTree(const TreeStructure &T)
{
    iVertexMapConst idmap 	= get(&VertexProp::id, g);
	
	cerr << "#V = " << T.V.size() << " #E = " << T.E.size() << " #Ep = " << T.Ep.size() << endl;
		
	VertexSet_cit vit;
	cerr << "V (index):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << *vit ;
		
	cerr << " E (index):";
	EdgeList_cit eit;
	for(eit = T.E.begin(); eit != T.E.end(); ++eit)
	{
		cerr << " " << *eit ;
	}
	cerr << endl;

	cerr << "V (id):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << idmap[*vit] ;
	
	cerr << " E (id): ";
	EdgePair::const_iterator epit;
	for(epit = T.Ep.begin(); epit != T.Ep.end(); ++epit)
		cerr << " " << *epit ;
	cerr << endl;
}

void
SteinerHeuristic::dumpSteinerTree_full(const TreeStructure &T)
{
    iVertexMapConst idmap 	= get(&VertexProp::id, g);
	dEdgeMapConst wmap 		= get(&EdgeProp::weight, g);

	cerr << "#V = " << T.V.size() << " #E = " << T.E.size() << " #Ep = " << T.Ep.size() << endl;
			
	VertexSet_cit vit;
	cerr << "V (index):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << *vit ;
		
	cerr << " E (index):";
	EdgeList_cit eit;
	for(eit = T.E.begin(); eit != T.E.end(); ++eit)
	{
		cerr << " (" << *eit << " w: " << wmap[*eit] << ")";
	}
	cerr << endl;

	cerr << "V (id):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << idmap[*vit] ;
	
	cerr << " E (id): ";
	EdgePair::const_iterator epit;
	for(epit = T.Ep.begin(); epit != T.Ep.end(); ++epit)
	{
		pair<edge_descriptorN, bool> e = edge(epit->first, epit->second, T.g);
		assert(e.second);
		cerr << " (" << *epit << " w: " << wmap[e.first] << ")" ;
	}
	cerr << endl;
}

void
SteinerHeuristic::dumpDijkstra(const ParentVector &p, const DistanceVector &distances)
{
	cerr << "[SH::dumpDijkstra] Parent/Distance Map : "<< endl;
	for (size_t i = 0; i < p.size(); ++i)
    {
    	if (p[i] != i)
    	  cerr << "parent[" << i << "] = " << p[i] << " Dist: " << distances[i] << endl;
    	else
    	  cerr << "parent[" << i << "] = no parent" << endl;
    }
}



void
SteinerHeuristic::dumpGraph(const GraphN &g)
{
    iVertexMapConst 	idmap = get(&VertexProp::id, g);
    dEdgeMapConst 		wm = get(&EdgeProp::weight, g);
	eidEdgeMapConst 	eidmap = get(&EdgeProp::id, g);

    cerr << "vertices(index) = ";
    pair<vertex_iteratorN, vertex_iteratorN> vp;
    for(vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		cerr << *vp.first << ", "; 
    }
	cerr << endl;
	
    cerr << "edges(index) = ";
	graph_traits<GraphN>::edge_iterator ei, ei_end;
    for(boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
    	cerr << "(" << *ei << " w: " << wm[*ei] << ") "; 
    }
    cerr << endl;

    cerr << "vertices(id) = ";
    for(vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		cerr << idmap[*vp.first] << ", ";
    }
    cerr << endl;
    	
    cerr << "edges(id) = ";
    for(boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
		cerr << "(" << eidmap[*ei] << " w: " << wm[*ei] << ") "; 
	}
    cerr << endl;
}

/*
void
ShortestPathHeuristic::PriorityQueueUnsigned(vertex_descriptorN zsource)
{
    iVertexMap idmap 	= get(&VertexProp::id, g);
	iVertexMap zmap 	= get(&VertexProp::znode, g);
	iVertexMap stmap	= get(&VertexProp::steiner, g);

	//	priority_queue<unsigned, vector<unsigned>, CompareVert> ZQueue;
	priority_queue<unsigned, vector<unsigned>, greater<unsigned> > ZQueue;

	// Adding vertex properties
	graph_traits<GraphN>::vertex_iteratorN vi, vi_end;
		
	cout << "[PriorityQueueUnsigned] Source : " << idmap[zsource] << endl;
	
    for(tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
		if(zmap[*vi] == true) // && idmap[*vi] != id_zsource)
		{
			unsigned int id = idmap[*vi];
			ZQueue.push(id);
			cout << "[PriorityQueueUnsigned] Pushed: " << id << endl;
		}
    }

	while(!ZQueue.empty()) {
		cout << ZQueue.top() << " ";
		ZQueue.pop();		
	}
	cout << endl;
}

void
ShortestPathHeuristic::RelaxedHeap(vertex_descriptorN zsource)
{
    iVertexMap idmap 	= get(&VertexProp::id, g);
	iVertexMap zmap 	= get(&VertexProp::znode, g);
	iVertexMap stmap	= get(&VertexProp::steiner, g);

	//relaxed heap
	relaxed_heap<vertex_descriptorN> heap(num_vertices(g));
	
	// Adding vertex properties
	graph_traits<GraphN>::vertex_iteratorN vi, vi_end;

    for(tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
		if(zmap[*vi] == true)// && idmap[*vi] != id_zsource)
		{
			heap.push(*vi);
			cout << "[RelaxedHeap] Pushed: " << get(&VertexProp::id, g)[*vi] << endl;
		}
    }

	vertex_descriptorN vtop;
	
	//for(size_t i = 0; i != heap.size(); i++)
	//{
		vtop = heap.top();
		int id = get(&VertexProp::id, g)[vtop];
		cout << "[RelaxedHeap] Popped: " << id << endl;
		//heap.pop();
		//heap.dump_tree();
		heap.remove(vtop);
	//}
}
*/

 
