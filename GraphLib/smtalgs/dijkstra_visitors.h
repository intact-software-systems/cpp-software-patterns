/***************************************************************************
                          dijkstra_visitors.h  -  description
                             -------------------
    begin                : Wed Sep 14 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_DIJKSTRA_VISITOR_KHV
#define GRAPHALGO_DIJKSTRA_VISITOR_KHV

#include "../boostprop.h"

class DijkstraVisitor
{

public:
	//DijkstraVisitor();
	//~DijkstraVisitor();
		
	void initialize_vertex( vertex_descriptorN v, GraphN g ) { }
	void examine_vertex( vertex_descriptorN v, GraphN g ) { }
	void finish_vertex( vertex_descriptorN v, GraphN g ) { }
	void examine_edge( edge_descriptorN e, GraphN g ) { }
	void edge_relaxed( edge_descriptorN e, GraphN g ) { }
	void edge_not_relaxed( edge_descriptorN e, GraphN g ) { }
	void discover_vertex( vertex_descriptorN v, GraphN g ); 
};


#endif // GRAPHALGO_DIJKSTRA_VISITOR_KHV

