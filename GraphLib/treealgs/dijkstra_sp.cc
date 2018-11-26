/***************************************************************************
                          dijkstra_sp.h  -  description
                             -------------------
    begin                : Mon Mar 20 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dijkstra_sp.h"
#include "treealgs.h"
#include "../simdefs.h"
#include "treestructure.h"
#include "fheap.h"

using namespace boost;
using namespace std;

typedef TreeAlgorithms::COLOR COLOR;

namespace TreeAlgorithms
{

void myDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent)
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;

    //enum COLOR { WHITE = 0, GRAY, BLACK};
    std::vector<COLOR> colorVector(num_vertices(g));
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zparent[*vit] = *vit;
		colorVector[*vit] = WHITE;
	}

	colorVector[src] = GRAY;
	zdistance[src] = 0;

	heap->insert(src, 0.0);
	while(heap->nItems() > 0)
	{
		int u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			int targ = target(*oit, g);
			ASSERTING(targ != u);
			if(((g[*oit].weight + zdistance[u]) < zdistance[targ]) && colorVector[targ] != BLACK)
			{
				zdistance[targ] = (g[*oit].weight + zdistance[u]);
				zparent[targ] = u;
				if(colorVector[targ] == WHITE)
				{
					colorVector[targ] = GRAY;
					heap->insert(targ, zdistance[targ]);
				}
				else if(colorVector[targ] == GRAY)
				{
					heap->decreaseKey(targ, zdistance[targ]);
				}
			}
		}
		colorVector[u] = BLACK;
	}	

	delete heap;
}

void myDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes)
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	
    //enum COLOR { WHITE = 0, GRAY, BLACK};
    //COLOR colorVector[num_vertices(g)];
    std::vector<COLOR> colorVector(num_vertices(g));

	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zparent[*vit] = *vit;
		colorVector[*vit] = WHITE;
	}

	colorVector[src] = GRAY;
	zdistance[src] = 0;

	heap->insert(src, 0.0);
	while(heap->nItems() > 0)
	{
		int u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			int targ = target(*oit, g);
			ASSERTING(targ != u);
			if(((g[*oit].weight + zdistance[u]) < zdistance[targ]) && colorVector[targ] != BLACK)
			{
				zdistance[targ] = (g[*oit].weight + zdistance[u]);
				zparent[targ] = u;
				if(colorVector[targ] == WHITE)
				{
					colorVector[targ] = GRAY;
					heap->insert(targ, zdistance[targ]);
				}
				else if(colorVector[targ] == GRAY)
				{
					heap->decreaseKey(targ, zdistance[targ]);
				}
			}
		}
		colorVector[u] = BLACK;
	}	

	delete heap;
}

bool myDBDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes)
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	
    //enum COLOR { WHITE = 0, GRAY, BLACK};
    //COLOR colorVector[num_vertices(g)];
    std::vector<COLOR> colorVector(num_vertices(g));
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));

	// -- new --
	vector<int> outdegree(num_vertices(g));
	vector<int> degbounds(num_vertices(g));
	vector<VertexSet> undiscovered(num_vertices(g));
	VertexSet intreeV;
	typedef multimap<double, int> WeightMap;
	typedef map<int, WeightMap> ParentWeightMap;
	ParentWeightMap		outedges;
	WeightMap::iterator wit, wit_end;
	// -- new end --
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zparent[*vit] = *vit;
		colorVector[*vit] = WHITE;

		// -- new --
		outdegree[*vit] = 0;
		degbounds[*vit] = getDegreeConstraint(g, *vit);
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			undiscovered[*vit].insert(target(*oit, g));
			outedges[*vit].insert(pair<double, int>(g[*oit].weight, target(*oit, g)));
		}
		// -- new end --
	}

	colorVector[src] = GRAY;
	zdistance[src] = 0;

	// -- new --
	intreeV.insert(src);
	// -- new end --
	
	heap->insert(src, 0.0);
	
	while(intreeV.size() <  treeNodes.size())
	{
		while(heap->nItems() > 0)
		{
			int u = heap->deleteMin();
			//cerr << "Popped : " << u << endl;
			//for(tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
			for(wit = outedges[u].begin(), wit_end = outedges[u].end(); wit != wit_end; ++wit)
			{
				int targ = wit->second; //target(*oit, g);
				ASSERTING(targ != u);
				
				pair<edge_descriptorN, bool> ep = edge(u, targ, g);
				ASSERTING(ep.second);
				edge_descriptorN e = ep.first;			
				if( (g[e].weight + zdistance[u] < zdistance[targ]) && (outdegree[u] < degbounds[u]) && (colorVector[targ] != BLACK) ) // -> new
				{
					// -- new --
					int parentv = zparent[targ];
					outdegree[parentv] = max(0, outdegree[parentv] - 1);
					outdegree[targ] = max(0, outdegree[targ] - 1);
					// reducing degree constraint -> insert to heap again to rediscover undiscovered nodes
					if(colorVector[parentv] == BLACK && undiscovered[parentv].size())
					{
						//cerr << "re-inserting " << parentv << " to heap. Items: " << heap->nItems() << " distance " << zdistance[parentv] << endl;
						heap->insert(parentv, (std::numeric_limits<double>::max)()); //zdistance[parentv]);
					}
					undiscovered[u].erase(targ);
					undiscovered[targ].erase(u);
					//cerr << "Old Parent outdegree " << zparent[targ] << " = " << outdegree[zparent[targ]] << endl;
					// -- new end --
	
					zdistance[targ] = (g[e].weight + zdistance[u]);
					zparent[targ] = u;
	
					// -- new --
					outdegree[u] = outdegree[u] + 1;
					outdegree[targ] = outdegree[targ] + 1;
					intreeV.insert(targ);
					// -- debug --
					//cerr << "New Parent outdegree " << u << " = " << outdegree[u] << endl;
					//cerr << "New Vertex outdegree " << targ << " = " << outdegree[targ] << endl;
					// -- debug end --
					// -- new end --
	
					if(colorVector[targ] == WHITE)
					{
						colorVector[targ] = GRAY;
						heap->insert(targ, zdistance[targ]);
					}
					else if(colorVector[targ] == GRAY)
					{
						heap->decreaseKey(targ, zdistance[targ]);
					}
				}// -- new
				else if( !( (g[e].weight + zdistance[u] < zdistance[targ]) && (colorVector[targ] != BLACK) ) ) // -> new
				{
					undiscovered[u].erase(targ);
					undiscovered[targ].erase(u);
				}
				else
				{
					ASSERTING((g[e].weight + zdistance[u] < zdistance[targ]) || (colorVector[targ] == BLACK));
				}
				//cerr << "undiscovered " << u << " : " << undiscovered[u] << endl;
				// -- new end --
			}
			
			colorVector[u] = BLACK;
		}	
	
		VertexSet leftV = treeNodes - intreeV;
		if(leftV.empty()) break;
		if(GlobalSimArgs::getRelaxDegree() == false) break;
				
		//cerr << "Vertices left " << leftV << endl;
		VertexSet incV = leftV;
		for(VertexSet::iterator vit = leftV.begin(), vit_end = leftV.end(); vit != vit_end; ++vit)
		{
			int u = *vit;
            for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
			{
				int targ = target(*oit, g);
				if(!incV.contains(targ)) // && !leftV.contains(targ)) 
				{
					degbounds[targ]++;
					heap->insert(targ,zdistance[targ]);
				}
				incV.insert(targ);				
			}
		}
		
		if(incV.empty()) break;
		if((leftV - incV).empty()) break;
		//cerr << "Increased degree bounds " << endl ; //for: " << incV << endl;
		//char c = getchar();
	}
	
	// -- debug --
	if(GlobalSimArgs::getRelaxDegree()) ASSERTING(intreeV == treeNodes);
	// -- debug end --

	delete heap;

	if(treeNodes.size() != intreeV.size()) return false;

	return true;
}

void myDijkstraSPT_TieBr(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes)
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;

    //enum COLOR { WHITE = 0, GRAY, BLACK};
    //COLOR colorVector[num_vertices(g)];
    std::vector<COLOR> colorVector(num_vertices(g));
	DistanceVector zhop(num_vertices(g));
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zparent[*vit] = *vit;
		colorVector[*vit] = WHITE;
		zhop[*vit] = (std::numeric_limits<double>::max)();
	}

	colorVector[src] = GRAY;
	zdistance[src] = 0;
	zhop[src] = 0;

	heap->insert(src, 0.0);
	while(heap->nItems() > 0)
	{
		int u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			int targ = target(*oit, g);
			ASSERTING(targ != u);
			if(((g[*oit].weight + zdistance[u]) <= zdistance[targ]) && colorVector[targ] != BLACK)
			{
				if(1 + zhop[u] < zhop[targ])
				{
					zdistance[targ] = (g[*oit].weight + zdistance[u]);
					zparent[targ] = u;
					zhop[targ] = 1 + zhop[u];
						
					if(colorVector[targ] == WHITE)
					{
						colorVector[targ] = GRAY;
						heap->insert(targ, zdistance[targ]);
					}
					else if(colorVector[targ] == GRAY)
					{
						heap->decreaseKey(targ, zdistance[targ]);
					}
				}
			}
		}
		colorVector[u] = BLACK;
	}	

	delete heap;
}


}; // namespace TreeAlgorithms

