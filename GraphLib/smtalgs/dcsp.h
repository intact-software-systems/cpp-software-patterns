/***************************************************************************
                          dcsp.h  -  description
                             -------------------
    begin                : Thu Mar 22 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_KHV
#define GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_KHV

#include "steiner_class.h"
#include "../boostprop.h"
#include <vector>

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
					class DelayConstrainedShortestPath
------------------------------------------------------------------------- */
class DelayConstrainedShortestPath : SteinerHeuristic
{
public:
	DelayConstrainedShortestPath(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_dcsp_(out) {}
	~DelayConstrainedShortestPath() {}

	// typedefs definitions
	enum COLOR { WHITE = 0, GRAY, BLACK};
	typedef vector<double> 				DistanceVector;
	typedef vector<vertex_descriptorN> 	ParentVector;
	typedef vector<bool>				BoolVector;
	typedef vector<COLOR>				ColorVector;
	
	
	void Algorithm(vertex_descriptorN, double);

	// class functions
	void DCSP(vertex_descriptorN src, double delayLimit);
	void DijkstraZ(vertex_descriptorN src);	
	
	bool createTree(vertex_descriptorN src, TreeStructure& treeStructure, bool zintree, const ParentVector &zparent);
	void combineTrees(const TreeStructure &treeDCSP, const TreeStructure &treeZ);
	void removeLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ);
	void removeFromTree(const ParentVector &zparent, vertex_descriptorN intersection, vertex_descriptorN src);
	
	void init();
		
private:
	TreeStructure 		&T_dcsp_;	
	ColorVector			colorDCSP_, colorZ_;
		
	DistanceVector		zdistanceDCSP_, zcostDCSP_;
	ParentVector 		zparentDCSP_;
	
	DistanceVector		zdistanceZ_;
	ParentVector 		zparentZ_;
	
	BoolVector			zintree_;
};

/*
class AnEdge
{
public:
	AnEdge(edge_descriptorN e, GraphN g) : g_(g), e_(e) {}
	~AnEdge() {}
	
	edge_descriptorN e_;
	GraphN g_;	
};

class AnEdge
{
public:
	AnEdge(EdgeId e) : e_(e) {}
	~AnEdge() {}
	
	EdgeId e_;
};



struct CompareEdges
{

	bool operator()(AnEdge edgeE, AnEdge edgeF) const
	{
		vertex_descriptorN src_e = source(edgeE.e_, edgeF.g_), targ_e = target(edgeE.e_, edgeF.g_);
		vertex_descriptorN src_f = source(edgeF.e_, edgeF.g_), targ_f = target(edgeF.e_, edgeF.g_);
		
		cerr << "(" << src_e << "," << targ_e << ")" << endl;
		cerr << "(" << src_f << "," << targ_f << ")" << endl;
		
		if(src_e == src_f && targ_e == targ_f)
		{
			cerr << " Found match " << endl;
			cerr << " src_e == src_f " << src_e << " == " << src_f << endl;
			cerr << " targ_e == targ_f " << targ_e << " == " << targ_f << endl;
			char c = getchar();

			return false;
		}
		
		if(src_e == targ_f && targ_e == src_f)
		{
			cerr << " Found match " << endl;
			cerr << " src_e == targ_f " << src_e << " == " << targ_f << endl;
			cerr << " targ_e == src_f " << targ_e << " == " << src_f << endl;
			char c = getchar();

			return false;
		}
		
		
		
		return true;
	}
	
	bool operator()(AnEdge edgeE, AnEdge edgeF) const
	{
		cerr << edgeE.e_ << " == " << edgeF.e_ << "?" << endl;
		
		if(edgeE.e_ == edgeF.e_)
			return false;
		
		cerr << " Edges are not the same " << endl;
		return true;
	}
};
*/
#endif // GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_KHV



