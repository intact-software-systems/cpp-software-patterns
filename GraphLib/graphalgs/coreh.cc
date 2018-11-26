/***************************************************************************
                          coreh.cc  -  description
                             -------------------
    begin                : Thu Oct 6 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "coreh.h"
#include <fstream>
#include "../simtime.h"
#include "../treealgs/treealgs.h"
#include "./steiner_set.h"
#include "../graphalgs/meshalgs.h"
#include <cstdlib>
#include <math.h>
#include <cmath>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

namespace GraphAlgorithms
{

std::ostream& operator<<(std::ostream& ostr, const pair<double, double> &p)
{
	ostr << "(" << p.first << "," << p.second << ") " ;

	return ostr;
}

void findWCNodes(const GraphN &g, VertexSet &vertexSet, int k, CoreSelectionAlgo algo)
{
	//cerr << WRITE_FUNCTION << " Finding WCNs " << endl;
	
	switch(algo)
	{
		case MEDIAN_DISTANCE:
			kBestLocatedNodesFromGraph_Median(g, vertexSet, k);
			break;
		case CENTER_SELECTION:
		case AVERAGE_DISTANCE:
			kBestLocatedNodesFromGraph_Average(g, vertexSet, k);
			break;
		case UNIFORM_SELECTION_LAYOUT:
			findWCNodesUniformSpread(g, vertexSet, k);
			break;
		case TAILED_SELECTION:
			findWCNodesTailedSpread(g, vertexSet, k);
			break;
		case BANDWIDTH_SELECTION:
			findWCNodesBandwidth(g, vertexSet, k);
			break;
		default:
			cerr << "No support for " << algo << endl;
			ASSERTING(0);
			break;
	}
}

void findWCNodes(const TreeStructure &T, VertexSet &vertexSet, int k, CoreSelectionAlgo algo)
{
	//cerr << WRITE_FUNCTION << " Finding WCNs " << endl;
	
	switch(algo)
	{
		case MEDIAN_DISTANCE:
			kBestLocatedNodesFromGraph_Median(T, vertexSet, k);
			break;
		case AVERAGE_DISTANCE:
			kBestLocatedNodesFromGraph_Average(T, vertexSet, k);
			break;
		//case UNIFORM_SELECTION_LAYOUT:
		//	findWCNodesUniformSpread(T, vertexSet, k);
		//	break;
		case CENTER_SELECTION:
			kCenterNodes(T.g, T.V, vertexSet, k);
			break;
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
			findkTailedSpreadNotInSets(T.g, T.g, T.V, T.V, VertexSet(), vertexSet, k);
			break;
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION:
			findkTailedSpreadNotInSets_snake(T.g, T.g, T.V, T.V, VertexSet(), vertexSet, k);
			break;
		case TAILED_SELECTION:
			findWCNodesTailedSpread(T, vertexSet, k);
			break;
		case BANDWIDTH_SELECTION:
			findWCNodesBandwidth(T, vertexSet, k);
			break;
		default:
			cerr << WRITE_PRETTY_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
}

int findWCNodes(const TreeStructure &T, CoreSelectionAlgo algo)
{
	//cerr << WRITE_FUNCTION << " Finding WCNs " << endl;
	
	int k = 1;
	VertexSet vertexSet;
	
	switch(algo)
	{
		case MEDIAN_DISTANCE:
			kBestLocatedNodesFromGraph_Median(T, vertexSet, k);
			break;
		case TAILED_SELECTION:
		case AVERAGE_DISTANCE:
			kBestLocatedNodesFromGraph_Average(T, vertexSet, k);
			break;
		case WORST_SELECTION:
			kWorstLocatedNodesFromGraph(T, vertexSet, k);
			break;
		case CENTER_SELECTION:
			kCenterNodes(T.g, T.V, vertexSet, k);
			break;
		//case UNIFORM_SELECTION_LAYOUT:
		//	findWCNodesUniformSpread(T, vertexSet, k);
		//	break;
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION :
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
			findkTailedSpreadNotInSet(T.g, T.g, T.V, VertexSet(), vertexSet, k);
			break;
		//case TAILED_SELECTION:
		//	findWCNodesTailedSpread(T, vertexSet, k);
		//	break;
		case BANDWIDTH_SELECTION:
			findWCNodesBandwidth(T, vertexSet, k);
			break;
		default:
			cerr << WRITE_PRETTY_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}

	if(vertexSet.empty()) return -1;

	return *(vertexSet.begin());
}



int findWCMemberNode(const TreeStructure &T, CoreSelectionAlgo algo)
{
	VertexSet vertexSet;
	//cerr << WRITE_FUNCTION << " input V size : " << T.V.size() << " edges : " << T.Ep.size() << " should be : " <<  (T.V.size() * (T.V.size() - 1))/2 << endl; 

	switch(algo) 
	{
		case MEDIAN_DISTANCE:
			kBestLocatedMemberNodesFromGraph_Median(T, vertexSet, 1);
			break;
		case UNIFORM_SELECTION_LAYOUT :
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION :
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
		case TAILED_SELECTION:
		case AVERAGE_DISTANCE:
			kBestLocatedMemberNodesFromGraph_Average(T, vertexSet, 1);
			break;
		case CENTER_SELECTION:
			kCenterNodes(T.g, T.V, vertexSet, 1);
			break;
		case BANDWIDTH_SELECTION:
			findWCMemberNodesBandwidth(T, vertexSet, 1);
			break;
		default:
			cerr << WRITE_PRETTY_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
	
	if(vertexSet.empty()) return -1;

	return *(vertexSet.begin());
}
/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kWorstLocatedNodesFromGraph(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	multimap<double, int> rankMap;
	double sum_weight = 0;
	
    dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			sum_weight += wmap[*eit];		
		}
	
		if(getOutDegree(g, *vit) > 0) 
			rankMap.insert(pair<double, int>((sum_weight/getOutDegree(*vit, g)), *vit));
		sum_weight = 0;
	}
	
	multimap<double, int>::reverse_iterator mit = rankMap.rbegin(), mit_end = rankMap.rend();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}

	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}
/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
	- finds k nodes including the best which is the graph median
------------------------------------------------------------------------- */
void kBestLocatedNodesFromGraph_Average(const GraphN &g, VertexSet& vertexSet, int k)
{
	multimap<double, int> rankMap;
	double sum_weight = 0;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit,vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			sum_weight += wmap[*eit];		
		}
		
		if(getOutDegree(g, *vit) > 0) 
			rankMap.insert(pair<double, int>((sum_weight/getOutDegree(*vit, g)), *vit));
		sum_weight = 0;
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}

	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}

/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedNodesFromGraph_Average(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	multimap<double, int> rankMap;
	double sum_weight = 0;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			sum_weight += wmap[*eit];		
		}
		
		if(getOutDegree(g, *vit) > 0) 
			rankMap.insert(pair<double, int>((sum_weight/getOutDegree(*vit, g)), *vit));
		sum_weight = 0;
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}

	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}

/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedMemberNodesFromGraph_Average(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	multimap<double, int> rankMap;
	double sum_weight = 0;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(T.g[*vit].vertexState != GROUP_MEMBER) continue;

		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			int targ = target(*eit, T.g);
			if(T.g[targ].vertexState != GROUP_MEMBER) continue;

			sum_weight += wmap[*eit];		
		}
		
		if(getOutDegree(g, *vit) > 0) 
			rankMap.insert(pair<double, int>((sum_weight/getOutDegree(*vit, g)), *vit));
		sum_weight = 0;
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState == GROUP_MEMBER)
		{
			vertexSet.insert(mit->second);	
			i++;
		}
	}

	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}


/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedNodesFromGraph_Median(const GraphN &g, VertexSet& vertexSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit,vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		multiset<double> weightSet;
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
	
		int median = weightSet.size()/2;	
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= median) 
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
	
	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}

/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedNodesFromGraph_Median(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		multiset<double> weightSet;
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		int median = weightSet.size()/2;
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= median) //(getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
	
	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}

/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedMemberNodesFromGraph_Median(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(T.g[*vit].vertexState != GROUP_MEMBER) continue;
		
		multiset<double> weightSet;
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			int targ = target(*eit, T.g);
			if(T.g[targ].vertexState != GROUP_MEMBER) continue;

			weightSet.insert(wmap[*eit]);
		}
		
		int median = weightSet.size()/2;
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= median) //(getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState != GROUP_MEMBER) continue;

		vertexSet.insert(mit->second);	
		i++;
	}
	
	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}


/*-----------------------------------------------------------------------
	Finds the k best located nodes in graph g according to weight
------------------------------------------------------------------------- */
void kBestLocatedNodesFromGraphNotInSet_Median(const TreeStructure &T, VertexSet& vertexSet, const VertexSet& inSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(inSet.contains(*vit)) continue;
		multiset<double> weightSet;
	
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		int median = weightSet.size()/2;
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= median) 
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
	
	//cerr << WRITE_FUNCTION << ": " << vertexSet << endl;
}
/*-----------------------------------------------------------------------
		Finds k WCNs tailed selection from the node layout:
			- Divides rectangle to rectangles and choose
			  nodes from the center of the rectangles
------------------------------------------------------------------------- */
void findWCNodesTailedSpread(const GraphN &g, VertexSet& vertexSet, int k)
{
	//cerr << WRITE_FUNCTION << endl;
	multimap<int, Rectangle> mapRect;
	multimap<double, int> arrCoord;
	VertexSet vSet, V;

	// first round : arrange all vertices according to xpos
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0) 
		{
			V.insert(*vit);
			vSet.insert(*vit);
			arrCoord.insert(pair<double, int>(g[*vit].xpos, *vit));
		}
	}
	
	Rectangle rect(pair<double, double>(1000,0), pair<double, double>(0, 1000), vSet, arrCoord, true);
	
	// mapRect.size() == 1
	mapRect.insert(pair<int, Rectangle>(vSet.size(), rect));
	
	while((int)vertexSet.size() < k)
	{
		VertexSet split;
		multimap<double, int>::iterator it, it_end;
		int newWCN;
		
		Rectangle max = (--(mapRect.end()))->second;
	   	findClosestVertXY(vertexSet, max, split, newWCN);
		vertexSet.insert(newWCN);

		// -- debug --
		//cerr << "MapRect size " << mapRect.size()  << endl;
		//for(multimap<int, Rectangle>::iterator mmit = mapRect.begin(), mmit_end = mapRect.end(); mmit != mmit_end; ++mmit) cerr << " sz " << mmit->first ;
		//cerr << endl;
	    //cerr << " max Coordinates : " << max.C1 << max.C2 << " arrCoord.size() " << max.arrCoord.size() << " vSet.size() " << max.vSet.size() << endl;
		//cerr << "Inserting WCN " << newWCN << endl;
		//cerr << "Split.size() " << split.size() << endl;
		//cerr << "max.arrX " << max.arrX << endl;
		// -- debug end --

		Rectangle newRect1, newRect2;

		bool newArrX;
	    if(max.arrX) newArrX = false;
		else newArrX = true;
		
		ASSERTING(max.arrX != newArrX);

		// Init new rectangle
		newRect1.vSet = split - vertexSet;
		newRect1.arrX = newArrX;
		if(max.arrX) 
		{
			newRect1.C1 = pair<double, double>(g[newWCN].xpos, max.C1.second);
			newRect1.C2 = max.C2;
		}
		else // !max.arrX
		{
			newRect1.C2 = pair<double, double>(max.C2.first, g[newWCN].ypos);
			newRect1.C1 = max.C1;
		}
		makeSortedCoord(g, newRect1); 
		// Init end

		// -- debug --
		//VertexSet debugSet;
		//for( it = newRect1.arrCoord.begin(), it_end = newRect1.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//if(debugSet == newRect1.vSet) cerr << "Diff " << debugSet - newRect1.vSet << newRect1.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect1.vSet);
		// -- debug end --
		
		// Init new rectangle
		newRect2.vSet = max.vSet - split;
		newRect2.arrX = newArrX;
		if(max.arrX) 
		{
			newRect2.C2 = pair<double, double>(g[newWCN].xpos, max.C2.second);
			newRect2.C1 = max.C1;
		}
		else // !max.arrX
		{
			newRect2.C1 = pair<double, double>(max.C1.first, g[newWCN].ypos);
			newRect2.C2 = max.C2;
		}
		makeSortedCoord(g, newRect2); 
		// Init end

		// -- debug --
		//debugSet.clear();
		//for( it = newRect2.arrCoord.begin(), it_end = newRect2.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//if(debugSet != newRect2.vSet) //cerr << "Diff " << debugSet - newRect2.vSet << newRect2.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect2.vSet);
		//cerr << "New Rect1: " << newRect1.C1 << newRect1.C2 << endl;
		//cerr << "New Rect2: " << newRect2.C1 << newRect2.C2 << endl;
		//cerr << "Rect1 arrCoord.size() " << newRect1.arrCoord.size() << " vSet.size() " << newRect1.vSet.size() << endl;
		//cerr << "Rect2 arrCoord.size() " << newRect2.arrCoord.size() << " vSet.size() " << newRect2.vSet.size() << endl;
		//cerr << "Current steiner set size() " << vertexSet.size() << endl;
		//char d = getchar();
		// -- debug end --

		removeRectangle(mapRect, max);

		mapRect.insert(pair<int, Rectangle>(newRect1.vSet.size(), newRect1));
		mapRect.insert(pair<int, Rectangle>(newRect2.vSet.size(), newRect2));
	}

	//cerr << WRITE_FUNCTION << " steiner set " << vertexSet << endl;
	//char c = getchar();
}


/*-----------------------------------------------------------------------
		Finds k WCNs tailed selection from the node layout:
			- Divides rectangle to rectangles and choose
			  nodes from the center of the rectangles
------------------------------------------------------------------------- */
void findWCNodesTailedSpread(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	//cerr << WRITE_FUNCTION << " Tree : " << T << " k " << k << endl;
	multimap<int, Rectangle> mapRect;
	multimap<double, int> arrCoord;
	VertexSet vSet;

	// first round : arrange all vertices according to xpos
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		vSet.insert(*vit);
		arrCoord.insert(pair<double, int>(g[*vit].xpos, *vit));
	}
	
	Rectangle rect(pair<double, double>(1000,0), pair<double, double>(0, 1000), vSet, arrCoord, true);
	
	// mapRect.size() == 1
	mapRect.insert(pair<int, Rectangle>(vSet.size(), rect));
	
	while((int)vertexSet.size() < k)
	{
		VertexSet split;
		multimap<double, int>::iterator it, it_end;
		int newWCN;
		
		Rectangle max = (--(mapRect.end()))->second;
	   	findClosestVertXY(vertexSet, max, split, newWCN);
		vertexSet.insert(newWCN);

		// -- debug --
		//cerr << "MapRect size " << mapRect.size()  << endl;
		//for(multimap<int, Rectangle>::iterator mmit = mapRect.begin(), mmit_end = mapRect.end(); mmit != mmit_end; ++mmit) cerr << " sz " << mmit->first ;
		//cerr << endl;
	    //cerr << " max Coordinates : " << max.C1 << max.C2 << " arrCoord.size() " << max.arrCoord.size() << " vSet.size() " << max.vSet.size() << endl;
		//cerr << "Inserting WCN " << newWCN << endl;
		//cerr << "Split.size() " << split.size() << endl;
		//cerr << "max.arrX " << max.arrX << endl;
		// -- debug end --

		Rectangle newRect1, newRect2;

		bool newArrX;
	    if(max.arrX) newArrX = false;
		else newArrX = true;
		
		ASSERTING(max.arrX != newArrX);

		// Init new rectangle
		newRect1.vSet = split - vertexSet;
		newRect1.arrX = newArrX;
		if(max.arrX) 
		{
			newRect1.C1 = pair<double, double>(g[newWCN].xpos, max.C1.second);
			newRect1.C2 = max.C2;
		}
		else // !max.arrX
		{
			newRect1.C2 = pair<double, double>(max.C2.first, g[newWCN].ypos);
			newRect1.C1 = max.C1;
		}
		makeSortedCoord(g, newRect1); 
		// Init end

		// -- debug --
		//VertexSet debugSet;
		//for( it = newRect1.arrCoord.begin(), it_end = newRect1.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//if(debugSet != newRect1.vSet) cerr << "Diff " << debugSet - newRect1.vSet << newRect1.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect1.vSet);
		// -- debug end --
		
		// Init new rectangle
		newRect2.vSet = max.vSet - split;
		newRect2.arrX = newArrX;
		if(max.arrX) 
		{
			newRect2.C2 = pair<double, double>(g[newWCN].xpos, max.C2.second);
			newRect2.C1 = max.C1;
		}
		else // !max.arrX
		{
			newRect2.C1 = pair<double, double>(max.C1.first, g[newWCN].ypos);
			newRect2.C2 = max.C2;
		}
		makeSortedCoord(g, newRect2); 
		// Init end

		// -- debug --
		//debugSet.clear();
		//for( it = newRect2.arrCoord.begin(), it_end = newRect2.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//if(debugSet != newRect2.vSet) cerr << "Diff " << debugSet - newRect2.vSet << newRect2.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect2.vSet);
		//cerr << "New Rect1: " << newRect1.C1 << newRect1.C2 << endl;
		//cerr << "New Rect2: " << newRect2.C1 << newRect2.C2 << endl;
		//cerr << "Rect1 arrCoord.size() " << newRect1.arrCoord.size() << " vSet.size() " << newRect1.vSet.size() << endl;
		//cerr << "Rect2 arrCoord.size() " << newRect2.arrCoord.size() << " vSet.size() " << newRect2.vSet.size() << endl;
		//cerr << "Current steiner set size() " << vertexSet.size() << endl;
		//char d = getchar();
		// -- debug end --

		removeRectangle(mapRect, max);

		mapRect.insert(pair<int, Rectangle>(newRect1.vSet.size(), newRect1));
		mapRect.insert(pair<int, Rectangle>(newRect2.vSet.size(), newRect2));
	}

	//cerr << WRITE_FUNCTION << " steiner set " << vertexSet << endl;
	//char c = getchar();
}

void findClosestVertXY(const Rectangle &rect, VertexSet &split, int &newWCN) 
{
	//cerr << WRITE_FUNCTION << " arrCoord.size() " << rect.arrCoord.size() << endl;
	
	pair<double, double> rangeC;
	if(rect.arrX) // sorted in X direction
	{
		rangeC.first = min(rect.C1.first, rect.C2.first);
		rangeC.second = max(rect.C1.first, rect.C2.first);
		//cerr << " Find X-Range: " << rangeC << endl;
	}
	else // sorted in Y direction
	{
		rangeC.first = min(rect.C1.second, rect.C2.second);
		rangeC.second = max(rect.C1.second, rect.C2.second);
		//cerr << " Find Y-Range: " << rangeC << endl;
	}
	
	ASSERTING(rangeC.first != rangeC.second);
	ASSERTING(rangeC.first < rangeC.second);
	
	double middleC = (rangeC.first + rangeC.second)/2;
	//cerr << "Middle is " << middleC << endl;
	int rounds = 0;
	int split_vertex = -1;
	multimap<double, int>::const_iterator it, it_end, split_it;
	for( it = rect.arrCoord.begin(), it_end = rect.arrCoord.end(); it != it_end; ++it, rounds++)
	{
		//cerr << " it " << it->first << " v: " << it->second << endl;
		split.insert(it->second);
		ASSERTING(it->first >= rangeC.first && it->first <= rangeC.second);
		if(it->first >= middleC)
		{
			split_it = it;
			double diffSplit = abs(split_it->first - middleC);
			double diffPrev = abs((--it)->first - middleC);
			if(diffPrev < diffSplit) 
			{
				split.erase(split_it->second);
				split_it = it;
			}
			//cerr << "Found split " << split_it->first << " v: " << split_it->second << " Position " << rounds << endl;
			split_vertex = split_it->second;
			break;
		}	
	}

	// all nodes below middle?
	//if(it == rect.arrCoord.end()) split_it = --it;
	//if(split_vertex < 0) split_it = --it;
	if(split_vertex < 0) split_vertex = (--it)->second;

	//ASSERTING(it != rect.arrCoord.end());
	//ASSERTING(split_it == it || split_it == ++it);
	ASSERTING(split_vertex > -1);

	newWCN = split_vertex;
	//cerr << WRITE_FUNCTION << " Found WCN : " << newWCN << endl;
}

void findClosestVertXY(const VertexSet &V, const Rectangle &rect, VertexSet &split, int &newWCN) 
{
	//cerr << WRITE_FUNCTION << " arrCoord.size() " << rect.arrCoord.size() << endl;
	
	pair<double, double> rangeC;
	if(rect.arrX) // sorted in X direction
	{
		rangeC.first = min(rect.C1.first, rect.C2.first);
		rangeC.second = max(rect.C1.first, rect.C2.first);
		//cerr << " Find X-Range: " << rangeC << endl;
	}
	else // sorted in Y direction
	{
		rangeC.first = min(rect.C1.second, rect.C2.second);
		rangeC.second = max(rect.C1.second, rect.C2.second);
		//cerr << " Find Y-Range: " << rangeC << endl;
	}
	
	ASSERTING(rangeC.first != rangeC.second);
	ASSERTING(rangeC.first < rangeC.second);
	
	double middleC = (rangeC.first + rangeC.second)/2;
	//cerr << "Middle is " << middleC << endl;
	int rounds = 0;
	int split_vertex = -1;
	multimap<double, int>::const_iterator it, it_end, split_it;
	for( it = rect.arrCoord.begin(), it_end = rect.arrCoord.end(); it != it_end; ++it, rounds++)
	{
		//cerr << " it " << it->first << " v: " << it->second << endl;
		if(V.contains(it->second)) continue;
		
		split.insert(it->second);
		ASSERTING(it->first >= rangeC.first && it->first <= rangeC.second);
		if(it->first >= middleC)
		{
			split_it = it;
			double diffSplit = abs(split_it->first - middleC);
			double diffPrev = abs((--it)->first - middleC);
			if(diffPrev < diffSplit) 
			{
				split.erase(split_it->second);
				split_it = it;
			}
			//cerr << "Found split " << split_it->first << " v: " << split_it->second << " Position " << rounds << endl;
			split_vertex = split_it->second;
			break;
		}	
	}

	// all nodes below middle?
	//if(it == rect.arrCoord.end()) split_it = --it;
	if(split_vertex < 0) split_vertex = (--it)->second;

	//ASSERTING(it != rect.arrCoord.end());
	//ASSERTING(split_it == it || split_it == ++it);
	ASSERTING(split_vertex > -1);

	newWCN = split_vertex;
	//cerr << WRITE_FUNCTION << " Found WCN : " << newWCN << endl;
}


void makeSortedCoord(const GraphN &g, Rectangle &rect) 
{
	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	pair<double, double> rangeC;
	if(rect.arrX) // sorted in X direction
	{
		rangeC.first = min(rect.C1.first, rect.C2.first);
		rangeC.second = max(rect.C1.first, rect.C2.first);
		//cerr << " Arrange X-Direction: " << rangeC << endl;
	}
	else // sorted in Y direction
	{
		rangeC.first = min(rect.C1.second, rect.C2.second);
		rangeC.second = max(rect.C1.second, rect.C2.second);
		//cerr << " Arrange Y-Direction: " << rangeC << endl;
	}
	ASSERTING(rangeC.first != rangeC.second);
	ASSERTING(rangeC.first < rangeC.second);
	// -- debug end --

	VertexSet::iterator vit, vit_end;
	for(vit = rect.vSet.begin(), vit_end = rect.vSet.end(); vit != vit_end; ++vit)
	{
		if(rect.arrX) // arrange X direction
		{
			//cerr << "v: " << *vit << " X: " << g[*vit].xpos ; //<< endl;
			//cerr << " x >= rangeC.first " << g[*vit].xpos << " >= " <<  rangeC.first << "&& " << " x <= rangeC.second " <<  g[*vit].xpos << " <= " << rangeC.second << endl;
			rect.arrCoord.insert(pair<double, int>(g[*vit].xpos, *vit));
			ASSERTING(g[*vit].xpos >= rangeC.first && g[*vit].xpos <= rangeC.second);
		}
		else // arrange Y direction
		{
			//cerr << "v: " << *vit << " Y: " << g[*vit].ypos << endl;
			rect.arrCoord.insert(pair<double, int>(g[*vit].ypos, *vit));
			ASSERTING(g[*vit].ypos >= rangeC.first && g[*vit].ypos <= rangeC.second);
		}
	}
}

void removeRectangle(multimap<int, Rectangle> &mapRect, const Rectangle &max)
{
	bool found = false;
	multimap<int, Rectangle>::iterator mmit, mmit_end;
	for(mmit = mapRect.begin(), mmit_end = mapRect.end(); mmit != mmit_end; ++mmit)
	{
		if(mmit->second.C1 == max.C1 && mmit->second.C2 == max.C2)
		{
			mapRect.erase(mmit);
			found = true;
			break;
		}
	}
	ASSERTING(found);
}

/*-----------------------------------------------------------------------
		Finds k WCNs uniformly spread out on the node layout:
			- start point is center (X, Y)
			- finds 8 nodes around each center (X, Y)
------------------------------------------------------------------------- */
void findWCNodesUniformSpread(const GraphN &g, VertexSet& vertexSet, int k)
{
	double X = 1000, Y = 1000;
	recurseGraph(g, vertexSet, k, 0, X/2, Y/2);
	//cerr << WRITE_FUNCTION << " steiner set " << vertexSet << endl;
	//char c = getchar();
}

void recurseGraph(const GraphN &g, VertexSet &vertexSet, int k, int recursed, double X, double Y)
{
	if((int)vertexSet.size() >= k) return;

	int vert = findClosestVertXY(g, X, Y);
	vertexSet.insert(vert);

	double half = 0.5, onehalf = 1.5, numWCNEachRound = 41;
	if((numWCNEachRound * recursed) < k)
	{
		// split to eight
		recursed = recursed + 1;
		recurseGraph(g, vertexSet, k, recursed, X*half, Y);
		recurseGraph(g, vertexSet, k, recursed, X, Y*half);
		recurseGraph(g, vertexSet, k, recursed, X*half, Y*half);
		recurseGraph(g, vertexSet, k, recursed, X*half, Y*onehalf);
		recurseGraph(g, vertexSet, k, recursed, X*onehalf, Y*half);
		recurseGraph(g, vertexSet, k, recursed, X, Y*onehalf);
		recurseGraph(g, vertexSet, k, recursed, X*onehalf, Y);
		recurseGraph(g, vertexSet, k, recursed, X*onehalf, Y*onehalf);
	}
}

int findClosestVertXY(const GraphN &g, double X, double Y)
{
	double bestDeviation = (numeric_limits<double>::max)();
	int closestV = -1;
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0) 
		{
			//cerr << *vit << " x: " << g[*vit].xpos << " y: " << g[*vit].ypos << endl;
			double temp_deviation = abs(X - g[*vit].xpos) + abs(Y - g[*vit].ypos);
			if(bestDeviation > temp_deviation)
			{
				closestV = *vit;
				bestDeviation = temp_deviation;
				//cerr << " bestDev " << bestDeviation << " Node: " << closestV << endl;
			}
		}
	}
	
	ASSERTING(closestV >= 0);
	return closestV;
}

/*-----------------------------------------------------------------------
		Finds k WCNs with best sum of outgoing bandwidth
------------------------------------------------------------------------- */
void findWCNodesBandwidth(const GraphN &g, VertexSet& vertexSet, int k)
{
	multimap<double, int> bwMap;
	
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		double sumBW = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
			sumBW = sumBW + g[*oit].bw;
		
		if(getOutDegree(g, *vit) > 0) 
			bwMap.insert(pair<double, int>(sumBW, *vit));
	}
	
	multimap<double, int>::iterator mit = bwMap.begin(), mit_end = bwMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
}

/*-----------------------------------------------------------------------
		Finds k WCNs with best sum of outgoing bandwidth
------------------------------------------------------------------------- */
void findWCNodesBandwidth(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	multimap<double, int> bwMap;
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		double sumBW = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
			sumBW = sumBW + g[*oit].bw;
			
		bwMap.insert(pair<double, int>(sumBW, *vit));
	}
	
	multimap<double, int>::iterator mit = bwMap.begin(), mit_end = bwMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
}

/*-----------------------------------------------------------------------
		Finds k WCNs with best sum of outgoing bandwidth
------------------------------------------------------------------------- */
void findWCMemberNodesBandwidth(const TreeStructure &T, VertexSet& vertexSet, int k)
{
	const GraphN &g = T.g;
	multimap<double, int> bwMap;
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(T.g[*vit].vertexState != GROUP_MEMBER) continue;

		double sumBW = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			int targ = target(*oit, T.g);
			if(T.g[targ].vertexState != GROUP_MEMBER) continue;

			sumBW = sumBW + g[*oit].bw;
		}
			
		bwMap.insert(pair<double, int>(sumBW, *vit));
	}
	
	multimap<double, int>::iterator mit = bwMap.begin(), mit_end = bwMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
}


/*-----------------------------------------------------------------------
	
		BELOW:  
			Functions to find "best" nodes from graphs, steinerSet etc.
				
------------------------------------------------------------------------- */
/*-----------------------------------------------------------------------
		Finds the best located node in graph g according to weight
			- Median
------------------------------------------------------------------------- */
int	findBestLocatedMemberNode(const TreeStructure &T)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int bestNode = -1;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState == GROUP_MEMBER)
		{
			bestNode = T.g[mit->second].id;
			break;
		}
	}
	
	ASSERTING(rankMap.size() > 0);
	ASSERTING(bestNode > -1);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
	
	return bestNode;	
}	

void findBestLocatedMemberNode(const TreeStructure &T, const VertexSet &exeptV, VertexSet &newV, int k)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int added = 0;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState == GROUP_MEMBER && !exeptV.contains(mit->second))
		{
			newV.insert(mit->second);
			added++;
		}
		if(added >= k) break;
	}
	
	ASSERTING(rankMap.size() > 0);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
}

void findBestLocatedMemberNodeConnectedToV(const TreeStructure &T, const VertexSet &connectedToV, VertexSet &newV, int k)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int added = 0;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState == GROUP_MEMBER && !connectedToV.contains(mit->second))
		{	
			bool isConnected = false;
			for(VertexSet::iterator vit = connectedToV.begin(), vit_end = connectedToV.end(); vit != vit_end; ++vit)
			{
				if(T.isEdge(mit->second, *vit)) 
				{
					isConnected = true;
					break;
				}
			}
			if(isConnected)
			{
				newV.insert(mit->second);
				added++;
			}
		}
		if(added >= k) break;
	}
	
	ASSERTING(rankMap.size() > 0);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
}

void findBestLocatedMemberNodeConnectedToV(const GraphN &g, const VertexSet &V, const VertexSet &connectedToV, VertexSet &newV, int k)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int added = 0;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(g[mit->second].vertexState == GROUP_MEMBER && !connectedToV.contains(mit->second))
		{	
			bool isConnected = false;
			for(VertexSet::iterator vit = connectedToV.begin(), vit_end = connectedToV.end(); vit != vit_end; ++vit)
			{
				if(isEdge(g, mit->second, *vit)) 
				{
					isConnected = true;
					break;
				}
			}
			if(isConnected)
			{
				newV.insert(mit->second);
				added++;
			}
		}
		if(added >= k) break;
	}
	
	ASSERTING(rankMap.size() > 0);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
}


/*-----------------------------------------------------------------------
		Finds the best located member node in graph g according to weight
			- Median
------------------------------------------------------------------------- */
int	findBestLocatedMemberNodeDegreeBound(const TreeStructure &T)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int bestNode = -1;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(T.g[mit->second].vertexState == GROUP_MEMBER && getOutDegree(mit->second, T.g) < getDegreeConstraint(T.g, mit->second))
		{
			bestNode = T.g[mit->second].id;
			break;
		}
	}
	
	ASSERTING(rankMap.size() > 0);
	ASSERTING(bestNode > -1);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
	
	return bestNode;	
}	

/*-----------------------------------------------------------------------
		Finds the best located node in graph g according to weight
			- Median
------------------------------------------------------------------------- */
int	findBestLocatedNodeDegreeBound(const TreeStructure &T)
{
	multimap<double, int> rankMap;
	multiset<double> weightSet;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, T.g);
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, T.g); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
		}
		
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= (getOutDegree(*vit, T.g)/2))
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
		weightSet.clear();
	}
	
	int bestNode = -1;
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end; ++mit)
	{
		if(getOutDegree(mit->second, T.g) < getDegreeConstraint(T.g, mit->second))
		{
			bestNode = T.g[mit->second].id;
			break;
		}
	}
	
	ASSERTING(rankMap.size() > 0);
	ASSERTING(bestNode > -1);
	
	//cerr << WRITE_FUNCTION << " : " << bestNode << " median w: " << mit->first << endl;	
	//char c = getchar();
	
	return bestNode;	
}	

/*-----------------------------------------------------------------------
	Finds the node with minimum worst case diameter graph g according 
	to weight
------------------------------------------------------------------------- */
int	findMDDLNode(const VertexSet &V, const GraphN &g)
{
	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		//cerr << *vit << ": getOutDegree " << getOutDegree(*vit, g) << endl;	
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}
	return centerVertex;
}

int	findDCMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double dist = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseCost(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLNode(V, g);
		
	return centerVertex;
}

int	findBRMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double radius = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && radius <= (GlobalSimArgs::getDiameterBound()/2))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLNode(V, g);
		
	return centerVertex;
}

int	findBDMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double diameter = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && diameter <= GlobalSimArgs::getDiameterBound())
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLNode(V, g);
		
	return centerVertex;
}

int	findMDNode(const VertexSet &V, const GraphN &g)
{
	return findCenterNode(V,g);

	/*double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		// worst case distance from *vit to any node in the tree
		double worstcase_dist = 0;
		if(getOutDegree(*vit, g) > 0)
			worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
		if(path_cost > worstcase_dist) 	
		{
			path_cost = worstcase_dist;	
			centerVertex = *vit;
		}
	}
	return centerVertex;*/
}

int findCenterNode(const VertexSet &V, const GraphN &g)
{
	PathList tree_path;
	double tree_diameter = TreeAlgorithms::getTreeDiameter(g, V, tree_path);

	if(tree_path.size() <= 1)
	{
		ASSERTING(!V.empty());
		ASSERTING(V.size() <= 1);
		return *(V.begin());
	}
	
	double radius = 0;
	int center = -1;
	for(PathList::iterator vit = tree_path.begin(), vit_end = tree_path.end(); vit != vit_end; )
	{
		// iterate
		int src = *vit;
		vit++;
		if(vit == vit_end) break;
		int targ = *vit;
		ASSERTING(TreeAlgorithms::isEdge(g, src, targ));

		pair<edge_descriptorN, bool> ep = edge(src, targ, g);
		ASSERTING(ep.second);
		
		if(radius + g[ep.first].weight >= tree_diameter/2)
		{
			if(!V.contains(src))
			{
				center = targ;
				break;
			}
			else if(!V.contains(targ))
			{
				center = src;
				break;
			}
				
			// center is either src or targ
			double src_center = abs((radius - tree_diameter/2));
			double targ_center = abs((radius + g[ep.first].weight) - tree_diameter/2);
			ASSERTING(src_center >= 0 && targ_center >= 0);
			
			if(src_center <= targ_center) center = src;
			else center = targ;
			
			break;
		}	
		radius = radius + g[ep.first].weight;
	}
	ASSERTING(center > -1);
	
	return center;
}

/*-----------------------------------------------------------------------
	Finds the node with minimum worst case diameter graph g according 
	to weight
------------------------------------------------------------------------- */
int	findMDDLMemberNode(const VertexSet &V, const GraphN &g)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexState != GROUP_MEMBER) continue;
		
		//cerr << *vit << ": getOutDegree " << getOutDegree(*vit, g) << endl;	
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	return centerVertex;
}

int	findMDDLWCNode(const VertexSet &V, const GraphN &g)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexFunction != WELL_CONNECTED) continue;
		
		//cerr << *vit << ": getOutDegree " << getOutDegree(*vit, g) << endl;	
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	return centerVertex;
}

int	findDCMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexFunction != WELL_CONNECTED) continue;
		
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double dist = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseCost(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLWCNode(V, g);
		
	return centerVertex;
}

int	findBRMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexFunction != WELL_CONNECTED) continue;
		
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double radius = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && radius <= (GlobalSimArgs::getDiameterBound()/2))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLWCNode(V, g);
		
	return centerVertex;
}

int	findBDMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexFunction != WELL_CONNECTED) continue;
					
		pair<edge_descriptorN, bool> ep = edge(*vit, dest, global);
		ASSERTING(ep.second);
		double diameter = distance[*vit] + global[ep.first].weight;
		
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit) && diameter <= GlobalSimArgs::getDiameterBound())
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	if(centerVertex < 0) centerVertex = findMDDLWCNode(V, g);
		
	return centerVertex;
}

/*-----------------------------------------------------------------------
	Finds the node with minimum worst case diameter graph g according 
	to weight
------------------------------------------------------------------------- */
int	findMDDLSteinerNode(const VertexSet &S, const GraphN &g)
{
 	double path_cost = (numeric_limits<double>::max)();
	int centerVertex = -1;
	
	//cerr << WRITE_FUNCTION << endl;
	
	VertexSet::iterator vit, vit_end;
	for(vit = S.begin(), vit_end = S.end(); vit != vit_end; ++vit)
	{
		if(g[*vit].vertexState != STEINER_POINT) continue;
		
		//cerr << *vit << ": getOutDegree " << getOutDegree(*vit, g) << endl;	
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(*vit, g) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(*vit, g) > 0)
				worstCaseDist(*vit, *vit, *vit, g, worstcase_dist);
		
			if(path_cost > worstcase_dist) 	
			{
				path_cost = worstcase_dist;	
				centerVertex = *vit;
			}
		}
	}

	return centerVertex;
}

void kCenterNodes(const GraphN &g, const VertexSet &V, VertexSet &coreSet, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapEccId;
	
	// -> full mesh of shortest paths
	if(num_edges(g) == ( (V.size() * (V.size() - 1))/2) )
	{
		for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
		{
			// worst case distance from *vit to any node in the mesh
			double worst_case_distance = 0;
			for(VertexSet::iterator vit_in = vit, vit_in_end = vit_end; vit_in != vit_in_end; ++vit_in)
			{
				if(*vit == *vit_in) continue;

				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(wmap[ep.first] > worst_case_distance) 
					worst_case_distance = wmap[ep.first];
			}

			mapEccId.insert(pair<double, int>(worst_case_distance, *vit));
		}

		if(!mapEccId.empty())
		{
			int i = 0;
			for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
			{
				int targ = it->second;
				coreSet.insert(targ);
			}
		}
		//cerr << WRITE_FUNCTION << " core Set : " << coreSet << endl;
		return;
	}

	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		// worst case distance from *vit to any node in the mesh
		double worstcase_dist = 0;
		if(getOutDegree(g, *vit) > 0)
			worstcase_dist = GraphAlgorithms::eccentricity_distance(g, V, *vit);
	
		mapEccId.insert(pair<double, int>(worstcase_dist, *vit));
	}

	ASSERTING(!mapEccId.empty());

	if(!mapEccId.empty())
	{
		int i = 0;
		for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
		{
			int targ = it->second;
			coreSet.insert(targ);
		}
	}
}

void kDLCenterNodes(const GraphN &g, const TreeStructure &T, VertexSet &coreSet, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapEccId;
	
	VertexSet::const_iterator vit, vit_end;
	for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		// worst case distance from *vit to any node in the mesh
		if(getOutDegree(T.g, *vit) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(T.g, *vit) > 0)
				worstCaseDist(*vit, *vit, *vit, T, worstcase_dist);

			mapEccId.insert(pair<double, int>(worstcase_dist, *vit));
		}
	}

	if(!mapEccId.empty())
	{
		int i = 0;
		for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
		{
			int targ = it->second;
			coreSet.insert(targ);
		}
	}
}


}; // namespace GraphAlgorithms



