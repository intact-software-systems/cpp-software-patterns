/***************************************************************************
                          graphviz.h  -  description
                             -------------------
    begin                : Thu Nov 25 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_GRAPH_VIZ_KHV
#define GRAPHALGO_GRAPH_VIZ_KHV

#include "../boostprop.h"
#include "../treealgs/treestructure.h"

using namespace TreeAlgorithms;
using namespace std;

class GraphVisualization
{
public:
	GraphVisualization() : globalG_(NULL) {}
	GraphVisualization(GraphN *g) : globalG_(g) {}

	~GraphVisualization() {}
	
	void VizGraph(ostream &ostsr, const GraphN &g);
	void VizTree(ostream &ostr, const TreeStructure &T);
	void VizTree(ostream &ostr, const TreeStructure &T, const TreeStructure &prevT);

	void VizGraph(const char *filename, const GraphN &g);
	void VizTree(const char *filename, const TreeStructure &T);

private:
	GraphN *globalG_;
};

#endif //GRAPHALGO_GRAPH_VIZ_KHV

