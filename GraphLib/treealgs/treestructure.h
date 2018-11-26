/***************************************************************************
                          treestructure.h  -  description
                             -------------------
    begin                : Tue Jan 31 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_TREE_STRUCTURE_KHV
#define GRAPHALGO_TREE_STRUCTURE_KHV

#include<memory>

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <queue>
#include <map>
#include <assert.h>
#include <stdlib.h>
//#include <boost/shared_ptr.hpp>
#include "../boostprop.h"
#include "../network/vertex_set.h"

//using namespace std;
using namespace boost;

namespace TreeAlgorithms
{
/*-----------------------------------------------------------------------
    EdgeList -
                Extends std::list
------------------------------------------------------------------------- */
struct EdgePair : public std::list<EdgeId>
{
    EdgePair() : std::list<EdgeId>() {}

    EdgePair(const EdgePair& orig) : std::list<EdgeId>(orig) {}

    EdgePair(EdgeId orig ) : std::list<EdgeId>( )
    {
        std::list<EdgeId>::push_back(orig);
    }

    inline void removeEdge(const EdgeId &e)
    {
        iterator it = this->begin(), it_end = this->end();
        for( ; it != it_end; ++it)
        {
            if(*it == e)
            {
                this->erase(it);
                break;
            }
        }
    }

    inline void removeEdge(int src, int targ)
    {
        EdgeId e(std::min(src, targ), std::max(src, targ));
        iterator it = this->begin(), it_end = this->end();
        for( ; it != it_end; ++it)
        {
            if(*it == e)
            {
                this->erase(it);
                break;
            }
        }
    }

    inline void insertEdge(int src, int targ)
    {
        EdgeId id;
        id.first = std::min(src, targ);
        id.second = std::max(src, targ);
        std::list<EdgeId>::push_back(id);
    }
};

std::ostream& operator<<(std::ostream& ostr, const std::pair<int, int> id);
int getDegreeConstraint(const GraphN &g, int u);
int getDegreeConstraint(int u, const GraphN &g);
int getDegreeConstraint(const GraphN &g, int u, int leverage);
int getOutDegree(const GraphN &g, int u);
int getOutDegree(int u, const GraphN &g);
bool isVertex(const GraphN &g, int u);
bool isEdge(const GraphN &g, int u, int v);

/*-----------------------------------------------------------------------
    EdgeSet -
                Extends std::set
------------------------------------------------------------------------- */
struct uniqueEdge {

    bool operator() (const EdgeId &a, const EdgeId &b) const
    {
        if(a == b) return false;

        return true;
    }
};

struct EdgeSet : public std::set<EdgeId, uniqueEdge>
{
    EdgeSet() : std::set<EdgeId, uniqueEdge>() {}

    EdgeSet(const EdgeSet& orig) : std::set<EdgeId, uniqueEdge>(orig) {}

    EdgeSet(EdgeId orig) : std::set<EdgeId, uniqueEdge>()
    {
        //std::set<EdgeId, uniqueEdge>::
        insertEdge(orig);
    }

    inline bool insertEdge(const EdgeId &id)
    {
        EdgeId i;
        i.first = std::min(id.first, id.second);
        i.second = std::max(id.first, id.second);
        return ((this->insert(i)).second);
    }
};

/*-----------------------------------------------------------------------
    EdgeList -
                Extends std::list
------------------------------------------------------------------------- */
struct EdgeList : public std::list<edge_descriptorN>
{
    EdgeList() : std::list<edge_descriptorN>() {}
    EdgeList(const EdgeList& orig) : std::list<edge_descriptorN>(orig) {}

    EdgeList(edge_descriptorN orig ) : std::list<edge_descriptorN>( )
    {
        std::list<edge_descriptorN>::push_back(orig);
    }

    inline void removeEdge(const GraphN &g, edge_descriptorN e)
    {
        int src = source(e, g), targ = target(e, g);
        iterator it = this->begin(), it_end = this->end();
        for( ; it != it_end; ++it)
        {
            int it_src = source(*it, g), it_targ = target(*it, g);
            if((it_src == src && it_targ == targ) || (it_src == targ && it_targ == src))
            {
                this->erase(it);
                break;
            }
        }
    }

    inline void removeEdge(const GraphN &g, int src, int targ)
    {
        iterator it = this->begin(), it_end = this->end();
        for( ; it != it_end; ++it)
        {
            int it_src = source(*it, g), it_targ = target(*it, g);
            if((it_src == src && it_targ == targ) || (it_src == targ && it_targ == src))
            {
                this->erase(it);
                break;
            }
        }
    }
};

/*-----------------------------------------------------------------------
        EdgeList typedefs
------------------------------------------------------------------------- */

//typedef std::list<edge_descriptorN>					EdgeList;
typedef std::list<edge_descriptorN>::iterator			EdgeList_it;
typedef std::list<edge_descriptorN>::const_iterator		EdgeList_cit;
typedef std::pair<EdgeList_it, EdgeList_it>				EdgeList_tie;

/*-----------------------------------------------------------------------
    TreeStructure -
                    T(V,E)
------------------------------------------------------------------------- */
class TreeStructure
{
public:
    TreeStructure() : V(), S(),  E(), Ep(), timeStamp_(0.0), validTree_(true) {}
    TreeStructure(int num_vertices) : V(), S(), E(), Ep(), g(num_vertices), timeStamp_(0.0), validTree_(true) {}
    TreeStructure(const TreeStructure &T) : V(T.V), S(T.S), E(T.E), Ep(T.Ep), g(T.g), timeStamp_(0.0), validTree_(true) {}
    TreeStructure(const GraphN &g);

    ~TreeStructure() {}

    VertexSet 		V;		// std::set<int>, group_members + steiner_points
    VertexSet		S;		// steiner points
    VertexSet		C;		// core nodes

    EdgeList 		E;		// std::list<edge_descriptorN>
    EdgePair		Ep;		// std::pair<int, int>
    GraphN 			g;		// defined in boostprop.h

public:
    void insertGraph(const GraphN &gin);
    void meshify(int v);
    int meshifyGraph();

    void removeVertex(int v);

    void insertVertex(int v, const GraphN &global_g);
    void insertVertex(int v, const VertexProp &vp);
    void insertVertex(int v, const VertexProp &vp, const VertexState &state);
    void insertVertex(int v, const VertexState &state, const VertexFunction &func);
    void addVertex(int v);

    void removeEdge(edge_descriptorN e);
    void removeEdge(int src, int targ);
    void removeEdges(int u, const VertexSet &V, const GraphN &global_g);

    void insertEdge(edge_descriptorN e, const GraphN &global_g);
    void insertEdge(int src, int targ, const GraphN &global_g);
    void insertEdge(int src, int targ, const EdgeProp &ep);
    void insertEdges(int v, const VertexSet &V, const GraphN &global_g);
    void insertEdges(int v, const GraphN &gin);
    void updateEdge(const EdgeId &id, const EdgeProp &ep);

    bool isVertex(int v) const;
    bool isEdge(int u, int v) const;

    void makeMember(int v);
    void makeSteiner(int v);
    void makeSteinerAvailable(int v);

    int getOutDegree(int u) const;
    int getAvOutDegree(int u) const;

    VertexFunction vertexFunction(int v) const;
    VertexState vertexState(int v) const;
    bool isWCN(int v) const;
    bool isGroupMember(int v) const;
    bool isSteinerPoint(int v) const;

    void vertexDistance(int u, int v);

    void clearEdges();
    void clearSteiner();
    void clearVertex(int);
    void clear();

public:
    void mergeTrees(const TreeStructure &subTree);
    void mergeTrees(const TreeStructure &subTree, const GraphN &global_g);
    void diffTrees(const TreeStructure &subTree);
    void removeUnusedVertices();
    void getNeighbors(int v, VertexSet &V) const;

    // print functions
    void print() const;
    void printGraph() const ;
    void print(std::ostream&) const;
    void printGraph(std::ostream&) const;

    void clearVE();
    void printVertexState(std::ostream&) const;

    inline double getTimeStamp() { return timeStamp_; }
    inline void setTimeStamp(double ts) { timeStamp_ = ts; }

public:
    inline void setValid(bool in) 		{ validTree_ = in; }
    inline bool isValid() 		const	{ return validTree_; }

private:
    double timeStamp_;
    bool validTree_;

    void insertVertex(int v);
    void insertEdge(int u, int v);
};

std::ostream& operator<<(std::ostream& ostr, const TreeStructure &T);

/*-----------------------------------------------------------------------
            MsgTreeStructure - Passed in messages
------------------------------------------------------------------------- */
// simple typedefs
typedef std::vector<double>			DistanceVector;
typedef std::vector<int>			ParentVector;
typedef std::vector<bool>			BoolVector;

class MsgTreeStructure
{
public:
    MsgTreeStructure() {}
    MsgTreeStructure(const TreeStructure &tree) : V(tree.V), S(tree.S), E(tree.E), g(tree.g) {}

    ~MsgTreeStructure() {}

    VertexSet 	V;
    VertexSet	S;
    EdgeList	E;
    GraphN		g;
};

typedef std::shared_ptr<MsgTreeStructure> MsgTreeStructurePtr;

class TreeDiameter {
public:
    TreeDiameter() {}
    TreeDiameter(const DistanceVector &d, const ParentVector &p) : distance(d), parent(p) {}
    ~TreeDiameter() {}

    DistanceVector 	distance;
    ParentVector 	parent;
};

} // namespace TreeAlgorithms

#endif // .._TREE_STRUCTURE_..

