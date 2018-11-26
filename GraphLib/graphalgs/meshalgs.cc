/***************************************************************************
                          meshalgs.cc  -  description
                             -------------------
    begin                : Mon Mar 31 2008
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "meshalgs.h"
#include "../treealgs/treestructure.h"
#include "../simdefs.h"
#include "../treealgs/fheap.h"

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

//typedef TreeAlgorithms::COLOR COLOR;

namespace GraphAlgorithms
{

// dijkstra's shortest path tree
void maxDistance(const GraphN &g, const VertexSet &V, int src, DistanceVector &zdistance, DistanceVector &hops, ParentVector &zparent, double &eccentricity, int &node_in_eccentricity_path)
{
    out_edge_iteratorN oit, oit_end;
    //enum COLOR { WHITE = 0, GRAY, BLACK};
    std::vector<TreeAlgorithms::COLOR> colorVector(num_vertices(g));

    HeapD<FHeap> heapD;
    Heap *heap = heapD.newInstance(num_vertices(g));

    for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
    {
        zdistance[*vit] = (std::numeric_limits<double>::max)();
        zparent[*vit] = *vit;
        hops[*vit] = 0;
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
            int v = target(*oit, g);
            double path_weight = zdistance[u] + g[*oit].weight;

            if((path_weight < zdistance[v]))
            {
                zdistance[v] = path_weight;
                zparent[v] = u;
                hops[v] = hops[u] + 1;

                if(colorVector[v] == WHITE)
                {
                    colorVector[v] = GRAY;
                    heap->insert(v, zdistance[v]);
                }
                else if(colorVector[v] == GRAY)
                {
                    heap->decreaseKey(v, zdistance[v]);
                }

                if(zdistance[v] >= eccentricity)
                {
                    eccentricity = zdistance[v];
                    node_in_eccentricity_path = v;
                }
            }
        }
        colorVector[u] = BLACK;
    }
    delete heap;
}

void maxDistance(const TreeStructure &T, int src, DistanceVector &zdistance, DistanceVector &hops, ParentVector &zparent, double &eccentricity, int &node_in_eccentricity_path)
{
    maxDistance(T.g, T.V, src, zdistance, hops, zparent, eccentricity, node_in_eccentricity_path);
}

// dijkstra's shortest path tree
void maxDistance(const TreeStructure &T, int src, DistanceVector &zdistance, DistanceVector &hops, ParentVector &zparent)
{
    double ecc = 0;
    int node = -1;

    maxDistance(T, src, zdistance, hops, zparent,ecc, node);
}

double eccentricity_distance(const TreeStructure &T, int src)
{
    return eccentricity_distance(T.g, T.V, src);
}

double eccentricity_distance(const GraphN &g, const VertexSet &V, int src)
{
    DistanceVector 	distance(num_vertices(g));
    DistanceVector 	hops(num_vertices(g));
    ParentVector	parent(num_vertices(g));
    double ecc = 0;
    int node = -1;

    maxDistance(g, V, src, distance, hops, parent, ecc, node);

    return ecc;
}

double eccentricity_distance(const GraphN &g, const VertexSet &V, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent)
{
    double ecc = 0;
    int node = -1;

    maxDistance(g, V, src, distance, hops, parent, ecc, node);
    return ecc;
}



// breadth first search tree
void maxDistanceHops(const TreeStructure &T, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent)
{
    const GraphN &g = T.g;
    ASSERTING(T.V.contains(src));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
        hops[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
        parent[*vit] = *vit;
    }

    distance[src] = 0;
    hops[src] = 0;
    parent[src] = src;

    typedef list<int> IntList;
    IntList neighbors;
    neighbors.push_back(src);

    while(!neighbors.empty())
    {
        int v = neighbors.front();
        neighbors.pop_front();

        ASSERTING(T.V.contains(v));
        out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
        {
            int u = target(*oit, g);
            ASSERTING(T.V.contains(u));

            if(hops[u] == (std::numeric_limits<double>::max()))
            {
                hops[u] = hops[v] + 1;
                distance[u] = distance[v] + g[*oit].weight;
                parent[u] = v;
                neighbors.push_back(u);
            }
        }
    }
}


// breadth first search tree
double maxDistanceHops(const TreeStructure &T, int src, DistanceVector &distance)
{
    const GraphN &g = T.g;
    ASSERTING(T.V.contains(src));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
        distance[*vit] = (std::numeric_limits<double>::max)();

    distance[src] = 0;
    double diameter = 0;

    typedef list<int> IntList;
    IntList neighbors;
    neighbors.push_back(src);

    while(!neighbors.empty())
    {
        int v = neighbors.front();
        neighbors.pop_front();

        out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
        {
            int u = target(*oit, g);
            ASSERTING(T.V.contains(u));

            if(distance[u] == (std::numeric_limits<double>::max()))
            {
                distance[u] = distance[v] + 1;
                if(distance[u] > diameter) diameter = distance[u];

                neighbors.push_back(u);
            }
        }
    }
    return diameter;
}

double eccentricity_hops(const TreeStructure &T, int src)
{
    DistanceVector 	distance(num_vertices(T.g));

    return maxDistanceHops(T, src, distance);
}


double diameter_hops(const TreeStructure &T)
{
    double max = 0;
    DistanceVector distance(num_vertices(T.g));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        int v = *vit;
        double d = maxDistanceHops(T, v, distance);
        max = d > max ? d : max;
    }

    return max;
}

double diameter_distance(const TreeStructure &T)
{
    double diameter = 0;
    PathList diameterPath;
    diameter_distance(T, diameterPath, diameter);

    return diameter;
}


void diameter_distance(const TreeStructure &T, PathList &diameterPath, double &diameter)
{
    DistanceVector 	distance(num_vertices(T.g));
    DistanceVector 	hops(num_vertices(T.g));
    ParentVector	parent(num_vertices(T.g));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        double eccentricity = 0;
        int node_in_eccentricity_path = -1;
        int v = *vit;
        maxDistance(T, v, distance, hops, parent, eccentricity, node_in_eccentricity_path);

        if(diameter < eccentricity)
        {
            diameter = eccentricity;
            diameterPath.clear();

            ASSERTING(node_in_eccentricity_path != v);
            int path_node = node_in_eccentricity_path;
            while(path_node != v)
            {
                diameterPath.push_back(path_node);
                path_node = parent[path_node];
            }

            diameterPath.push_back(v);

            //cerr << WRITE_FUNCTION << " diameter : " << diameter << endl;
        }
    }
}

// matrix of the shortest distances between any two vertices.
void distanceMatrix(const TreeStructure &T, DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent)
{
    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        int v = *vit;
        maxDistance(T, v, distance[v], hops[v], parent[v]);
    }
}

void distanceMatrixHops(const TreeStructure &T, DistanceVectorMatrix &distance)
{
    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        int v = *vit;
        maxDistanceHops(T, v, distance[v]);
    }
}

void printAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path)
{
    if(src == targ)
    {
        cerr << src << " ";
        path.push_back(src);
    }
    else if(parent[src][targ] == src)
    {
        cerr << " no path from " << src << " to " << targ << " exists " << endl;
    }
    else
    {
        printAllPairsSP(parent, src, parent[src][targ], path);
        cerr << targ << " ";
        path.push_back(targ);
    }
}

};

/*
double maxDistanceHops(const TreeStructure &T, int src, DistanceVector &distance)
{
    const GraphN &g = T.g;
    ASSERTING(T.V.contains(src));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
        distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance

    distance[src] = 0;
    double depth = 1;                       // next distance in BFS
    int cnt = 0;

    VertexSet neighborSet;
    neighborSet.insert(src);

    while(!neighborSet.empty())
    {
        VertexSet nextNeighbors;
        for(VertexSet::iterator vit = neighborSet.begin(), vit_end = neighborSet.end(); vit != vit_end; ++vit)
        {
            int v = *vit;
            out_edge_iteratorN oit, oit_end;
            ASSERTING(T.V.contains(v));

            for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
            {
                int u = target(*oit, g);
                if(distance[u] == (std::numeric_limits<double>::max)()) // first time reachable?
                {
                    cnt++;
                    distance[u] = depth;
                    nextNeighbors.insert(u);
                }
            }
        }
        // next try set of vertices further away
        neighborSet = nextNeighbors;
        depth++;
    }

    return cnt == T.V.size() ? depth-1 : T.V.size();
}
*/

/*void maxDistance(const TreeStructure &T, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent, double &eccentricity, int &node_in_eccentricity_path)
{
    const GraphN &g = T.g;
    ASSERTING(T.V.contains(src));

    for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
    {
        distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
        hops[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
        parent[*vit] = *vit;
    }

    distance[src] = 0;
    hops[src] = 0;
    parent[src] = src;

    typedef list<int> IntList;
    IntList neighbors;
    neighbors.push_back(src);

    while(!neighbors.empty())
    {
        int v = neighbors.front();
        neighbors.pop_front();

        ASSERTING(T.V.contains(v));
        out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
        {
            int u = target(*oit, g);
            ASSERTING(T.V.contains(u));

            if(distance[u] > distance[v] + g[*oit].weight)
            {
                hops[u] = hops[v] + 1;
                distance[u] = distance[v] + g[*oit].weight;
                parent[u] = v;
                neighbors.push_back(u);

                if(distance[u] >= eccentricity)
                {
                    eccentricity = distance[u];
                    node_in_eccentricity_path = u;
                }
                //cerr << u << " hops " << hops[u] << " distance " << distance[u] << " parent " << parent[u] << endl;
            }
        }
    }
}*/

