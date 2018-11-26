/***************************************************************************
                          graphviz.h  -  description
                             -------------------
    begin                : Thu Nov 25 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "graphviz.h"
#include "../graphstats/recordtree.h"
#include "fstream"
#include "iostream"
#include <string>

using namespace std;
using namespace TreeAlgorithms;


void
GraphVisualization::VizGraph(ostream &outfile, const GraphN &g)
{
	// start writing Vertices, Edges and....
	outfile << "*Vertices  " << num_vertices(g) << "\n";

	pair<vertex_iteratorN, vertex_iteratorN> pvit;
	int i;
    std::vector<float> map(num_vertices(g));
	for(pvit = vertices(g), i = 1; pvit.first != pvit.second; ++pvit.first, i++)
	{
		outfile << "       " << i << "    \"" << *(pvit.first) << "\" \n";
		map[*(pvit.first)] = i;
	}
	
	outfile << "*Edges  " << num_edges(g) << "\n";
	pair<out_edge_iteratorN, out_edge_iteratorN> sp_eit; 
	for(pvit = vertices(g); pvit.first != pvit.second; ++pvit.first)
	{
		for(sp_eit = out_edges(*(pvit.first), g); sp_eit.first != sp_eit.second; ++sp_eit.first)
		{
			vertex_descriptorN src = source(*sp_eit.first, g), targ = target(*sp_eit.first, g);
			outfile <<"       " << map[src] << "     " << map[targ] << "  " << get(&EdgeProp::weight, g)[*(sp_eit.first)]; // << "\n";
			//outfile << "       " << (src) << "     " << (targ) << "  " << get(&EdgeProp::weight, g)[*(sp_eit.first)] << "\n";
			outfile << " l \"" <<(src) << " -> " << (targ) << "\" ";
			outfile << "\n";
		}
	}
	
	outfile << "\n";
}

void
GraphVisualization::VizTree(ostream &outfile, const TreeStructure &T)
{
	outfile << "*Vertices    " << T.V.size() << "\n";

	GraphN g = T.g; 	
    std::vector<float> map(num_vertices(g));
	int i; 

	VertexSet_it vit;
	for(vit = T.V.begin(), i = 1; vit != T.V.end(); ++vit, i++)
	{
		outfile << "    " << i << "   \"" << *vit << "\" " << g[*vit].xpos/1000 << " " << g[*vit].ypos/1000 << " 0";
		
		outfile << "\tbc Black";
		// different colors
		if(get(&VertexProp::vertexState, g)[*vit] == STEINER_POINT) 	// steiner node
			outfile << " box x_fact 3 y_fact 3 ic Black ";
			//outfile << " box ic Black ";
		else if(get(&VertexProp::vertexState, g)[*vit] == GROUP_MEMBER)
			outfile << " ellipse ic White ";		// group member znode
		else cerr << "[Error] Not a znode or steiner " << *vit << endl;

		map[*vit] = i;
		outfile << "\n";
	}

	for(vit = T.C.begin(); vit != T.C.end(); ++vit, i++)
	{
		if(T.V.contains(*vit)) continue;

		outfile << "    " << i << "   \"" << *vit << "\" " << g[*vit].xpos/1000 << " " << g[*vit].ypos/1000 << " 0";
		
		outfile << "\tbc Black";
		outfile << " box x_fact 3 y_fact 3 ic Black ";

		outfile << "\n";
		map[*vit] = i;
	}

	if(T.E.size())
	outfile << "*Edges     " << T.E.size() << "\n";
	EdgeList::const_iterator eit, eit_end;
	for(eit = T.E.begin(), eit_end = T.E.end(); eit != eit_end; ++eit)
	{
		vertex_descriptorN src = source((*eit), g), targ = target((*eit), g);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		assert(e.second);
		
		outfile << "      " << map[src] << "  " << map[targ] << "   " << get(&EdgeProp::weight, g)[e.first]; 
		outfile << " l \"" <<(src) << " -> " << (targ) << "\" ";

		// TODO add different edgetypes
		outfile << " p Solid w 1.0 c Black ";
		outfile << "\n";
	}
	outfile << "\n";
}
void
GraphVisualization::VizTree(ostream &outfile, const TreeStructure &T, const TreeStructure &prevT)
{
	EdgePair diffEp = Statistics::findDifference(T.Ep, prevT.Ep);

	outfile << "*Vertices    " << T.V.size() << "\n";
	
	GraphN g = T.g; 	
    std::vector<float> map(num_vertices(g));
	int i; 

	VertexSet_it vit;
	for(vit = T.V.begin(), i = 1; vit != T.V.end(); ++vit, i++)
	{
		outfile << "    " << i << "   \"" << *vit << "\" " << g[*vit].xpos/1000 << " " << g[*vit].ypos/1000 << " 0";
		
		stringstream tstream;
		if(!prevT.V.contains(*vit))
			tstream << " bc Blue ";
		else tstream << " bc Black";
		
		if(get(&VertexProp::vertexState, g)[*vit] == STEINER_POINT) 	// steiner node
			tstream << " box ic Black ";
		else if(get(&VertexProp::vertexState, g)[*vit] == GROUP_MEMBER)
			tstream << " ellipse ic White ";		// group member znode
		else cerr << "[Error] Not a znode or steiner " << *vit << endl;

		outfile << tstream.str() ;

		map[*vit] = i;
		outfile << "\n";
	}

	outfile << "*Edges     " << T.E.size() << "\n";
	EdgeList::const_iterator eit, eit_end;
	for(eit = T.E.begin(), eit_end = T.E.end(); eit != eit_end; ++eit)
	{
		vertex_descriptorN src = source((*eit), g), targ = target((*eit), g);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		assert(e.second);
		
		outfile << "      " << map[src] << "  " << map[targ] << "   " << get(&EdgeProp::weight, g)[e.first]; 
		outfile << " l \"" <<(src) << " -> " << (targ) << "\" ";

		// TODO add different edgetypes
		bool found = false;
		stringstream tstream;
		EdgeId eid(src, targ);
		for(EdgePair::iterator epit = diffEp.begin(), epit_end = diffEp.end(); epit != epit_end; ++epit)
		{
			if(*epit == eid)
			{
				found = true;
				tstream << " p Dots w 1.5 c Black ";
				break;
			}
		}
		if(!found)
			tstream << " p Solid w 1.0 c Black ";
		
		outfile << tstream.str() ;
		outfile << "\n";
	}
	outfile << "\n";
}

void
GraphVisualization::VizGraph(const char *filename, const GraphN &g)
{
	// open file for writing
	ofstream outfile(filename, ios::out);
		
	// start writing Vertices, Edges and....
	outfile << "*Vertices  " << num_vertices(g) << "\n";

	pair<vertex_iteratorN, vertex_iteratorN> pvit;
	int i;
    std::vector<float> map(num_vertices(g));
	for(pvit = vertices(g), i = 1; pvit.first != pvit.second; ++pvit.first, i++)
	{
		outfile << "       " << i << "    \"" << *(pvit.first) << "\" \n";
		map[*(pvit.first)] = i;
	}
	
	outfile << "*Edges  " << num_edges(g) << "\n";
	pair<out_edge_iteratorN, out_edge_iteratorN> sp_eit; 
	for(pvit = vertices(g); pvit.first != pvit.second; ++pvit.first)
	{
		for(sp_eit = out_edges(*(pvit.first), g); sp_eit.first != sp_eit.second; ++sp_eit.first)
		{
			vertex_descriptorN src = source(*sp_eit.first, g), targ = target(*sp_eit.first, g);
			outfile <<"       " << map[src] << "     " << map[targ] << "  " << get(&EdgeProp::weight, g)[*(sp_eit.first)]; // << "\n";
			//outfile << "       " << (src) << "     " << (targ) << "  " << get(&EdgeProp::weight, g)[*(sp_eit.first)] << "\n";
			outfile << " l \"" <<(src) << " -> " << (targ) << "\" ";
			outfile << "\n";
		}
	}
	
	outfile << "\n";
	outfile.close();
}

void
GraphVisualization::VizTree(const char *filename, const TreeStructure &T)
{
	ofstream outfile(filename, ios::out);
	outfile << "*Vertices    " << T.V.size() << "\n";

	GraphN g = T.g; 	
    std::vector<float> map(num_vertices(g));
	int i; 

	VertexSet_it vit;
	for(vit = T.V.begin(), i = 1; vit != T.V.end(); ++vit, i++)
	{
		outfile << "    " << i << "   \"" << *vit << "\" " << g[*vit].xpos/1000 << " " << g[*vit].ypos/1000 << " 0";

		// different colors
		if(get(&VertexProp::vertexState, g)[*vit] == STEINER_POINT) // steiner node
			outfile << " x_fact 3 y_fact 3 ic Blue ";
		else if(get(&VertexProp::vertexState, g)[*vit] == GROUP_MEMBER)
			outfile << " ic Red ";	   			 // group member znode
		else cerr << "[Error] Not a znode or steiner " << *vit << endl;

		map[*vit] = i;
		outfile << "\n";
	}

	outfile << "*Edges     " << T.E.size() << "\n";
	EdgeList::const_iterator eit, eit_end;
	for(eit = T.E.begin(), eit_end = T.E.end(); eit != eit_end; ++eit)
	{
		vertex_descriptorN src = source((*eit), g), targ = target((*eit), g);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		assert(e.second);
		
		outfile << "      " << map[src] << "  " << map[targ] << "   " << get(&EdgeProp::weight, g)[e.first]; 
		outfile << " l \"" <<(src) << " -> " << (targ) << "\" ";

		// TODO add different edgetypes
		
		outfile << "\n";
	}
	outfile << "\n";
	outfile.close();
}




