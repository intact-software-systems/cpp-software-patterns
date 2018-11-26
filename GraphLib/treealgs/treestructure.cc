/***************************************************************************
                          treealgs.cc  -  description
                             -------------------
    begin                : Tue Jan 31 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <functional>
#include <fstream>

#include "treestructure.h"
#include "treealgs.h"
#include "../simdefs.h"
#include "../network/vertex_set.h"

using namespace std;
using namespace boost;

namespace TreeAlgorithms
{
ostream& operator<<(ostream& ostr, const pair<int, int> id)
{
	ostr << "( " << id.first << "," << id.second << " )" ;
	return ostr;
}

bool isVertex(const GraphN &g, int u)
{
	bool isV = true;
	if(num_vertices(g) > u)
	{
		if(g[u].id == -1)  isV = false;
		
		if(!isV)
		{
			//cerr << WRITE_PRETTY_FUNCTION << " v: " << u << " prop : " << g[u] << endl;
			ASSERTING(out_degree(u, g) <= 0);
		}
	}
	else
		isV = false;

	return isV;
}

bool isEdge(const GraphN &g, int u, int v) 
{
	if(!isVertex(g,u) || !isVertex(g,v)) return false;
	
	pair<edge_descriptorN, bool> ep = edge(u,v,g);
	if(!ep.second) return false;

	return true;	
}


int getOutDegree(const GraphN &g, int u)
{
	int od = 0;
	if(isVertex(g, u)) 
	{
		od = out_degree(u, g);
	}
	
	return od;
}

int getOutDegree(int u, const GraphN &g)
{
	int od = 0;
	if(isVertex(g, u)) 
	{
		od = out_degree(u, g);
	}
	
	return od;
}

int getDegreeConstraint(const GraphN &g, int u)
{
	//cerr << WRITE_FUNCTION << u << ":" << g[u] << endl;
	if(g[u].degree_limit > -1)
	{
		//cerr << WRITE_FUNCTION << " returning node specific degree limit : " << g[u] << endl;
		return g[u].degree_limit;
	}

	if(g[u].vertexFunction == CLIENT)
		return (GlobalSimArgs::getDegreeConstraint());
	else if(g[u].vertexFunction == WELL_CONNECTED) 
		return (GlobalSimArgs::getDegreeConstraintSP());
	else
		ASSERTING(0);

	return max(GlobalSimArgs::getDegreeConstraint(), GlobalSimArgs::getDegreeConstraintSP()); 
}

int getDegreeConstraint(int u, const GraphN &g)
{
	return getDegreeConstraint(g, u);
}

int getDegreeConstraint(const GraphN &g, int u, int leverage)
{
	return (getDegreeConstraint(g,u) + leverage);
}

std::ostream& operator<<(std::ostream& ostr, const TreeStructure &T)
{
	T.print(ostr);
	return ostr;
}
/*-----------------------------------------------------------------------
	class TreeStructure:		
				Functions to class TreeStructure
------------------------------------------------------------------------- */
TreeStructure::TreeStructure(const GraphN &gin) : timeStamp_(0.0)
{
    insertGraph(gin);
}

void
TreeStructure::insertGraph(const GraphN &gin)
{
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(gin); vit != vit_end; ++vit)
	{
		if(num_vertices(gin) > *vit && gin[*vit].id > -1)
		{
			if(out_degree(*vit, gin) > 0)
				insertEdges(*vit, gin);
		}
	}
}

void
TreeStructure::meshify(int v)
{
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		if(v == *vit) continue;
		insertEdge(v, *vit);
	}		
}

int 
TreeStructure::meshifyGraph()
{
	int vertex_degree = V.size() - 1;
	int num_edges_added = 0;
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		int src = *vit;
		VertexSet outV;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(src, g); oit != oit_end; ++oit)
		{	
			int targ = target(*oit, g);
			ASSERTING(targ != src);
			outV.insert(targ);		

			if(g[*oit].weight <= 0.0001) g[*oit].weight = 1.0f;
		}
		
		VertexSet remV = V - outV - src;

		for(VertexSet::iterator it = remV.begin(), it_end = remV.end(); it != it_end; ++it)
		{	
			int targ = *it;

			if(isEdge(src, targ)) continue;
			if(src == targ) continue;

			pair<edge_descriptorN, bool> e = add_edge(src, targ, g);
			ASSERTING(e.second);
			
			if(g[src].id < 0)
			{
				g[src].id = src;
				g[src].vertexFunction = CLIENT;
			}
			if(g[targ].id < 0)
			{
				g[targ].id = targ;
				g[targ].vertexFunction = CLIENT;
			}
		
			num_edges_added++;

			EdgeProp ep;
			ep.id = pair<int, int>(src, targ);
			ep.delay = 1.0f; 
			ep.weight = 1.0f; 
			ep.bw = 1000.0; 
			ep.cost = 1.0;
			ep.length = 1.0;
			ep.hops = 1;	
			
			g[e.first] = ep;
		}

		ASSERTING(getOutDegree(src) == vertex_degree);
	}
	
	return num_edges_added;
}

VertexFunction
TreeStructure::vertexFunction(int v) const
{
	if(!isVertex(v)) return NO_FUNCTION;
	
	return g[v].vertexFunction;
}

VertexState
TreeStructure::vertexState(int v) const
{
	if(!isVertex(v)) return NO_STATE;
	return g[v].vertexState;
}

bool
TreeStructure::isWCN(int v) const
{
	return vertexFunction(v) == WELL_CONNECTED;
}

bool
TreeStructure::isGroupMember(int v) const
{
	return vertexState(v) == GROUP_MEMBER;
}

bool
TreeStructure::isSteinerPoint(int v) const
{
	return vertexState(v) == STEINER_POINT;
}

void TreeStructure::makeMember(int v)
{
	g[v].vertexState = GROUP_MEMBER;

	S.erase(v);
}
void TreeStructure::makeSteiner(int v)
{
	g[v].vertexState = STEINER_POINT;

	S.insert(v);
}

void TreeStructure::makeSteinerAvailable(int v)
{
	g[v].vertexState = STEINER_POINT_AVAILABLE;
	S.insert(v);
}

void TreeStructure::clearEdges()
{
	EdgeList::iterator oit = E.begin(), oit_end = E.end();
	for( ; oit != oit_end; ++oit)
	{
		int src = source(*oit, g), targ = target(*oit, g);
		pair<edge_descriptorN, bool> ep = edge(src, targ, g);
		if(ep.second) g[ep.first] = EdgeProp();
		
		remove_edge(src, targ, g);
		remove_edge(src, targ, g);
	}
	
	// -- debug --
	edge_iteratorN ei, ei_end;
    boost::tuples::tie(ei, ei_end) = edges(g);
	ASSERTING(ei == ei_end);
	// -- debug end --
	
	E.clear();
	Ep.clear();
}
void TreeStructure::clearSteiner()
{
	//cerr << WRITE_FUNCTION << "removing steiner : " << endl;

	V = V - S;
	VertexSet::iterator sit = S.begin(), sit_end = S.end();
	for( ; sit != sit_end; ++sit)
	{
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*sit, g); oit != oit_end; ++oit) removeEdge(*oit);

		//cerr << *sit << " " ;
		clear_vertex(*sit, g);
		g[*sit] = VertexProp();
	}
	S.clear();
	//cerr << endl;
}

void TreeStructure::removeEdge(edge_descriptorN e)
{
	int src = source(e, g), targ = target(e, g);
	this->removeEdge(src, targ);
}

void TreeStructure::removeEdge(int src, int targ) 
{
	if(isEdge(src, targ))  
	{
		pair<edge_descriptorN, bool> ep = edge(src, targ, g);
		ASSERTING(ep.second);
		g[ep.first] = EdgeProp();
		remove_edge(src, targ, g);
		remove_edge(targ, src, g);
		Ep.removeEdge(g[src].id, g[targ].id);
	}

	E.removeEdge(g, src, targ); 
}

void TreeStructure::removeEdges(int u, const VertexSet &V, const GraphN &global_g)
{
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
		removeEdge(u, *vit);
}

void TreeStructure::removeVertex(int v)
{
	if(!isVertex(v)) return;
			
	if((g[v].vertexState == STEINER_POINT) || (g[v].vertexState == STEINER_POINT_AVAILABLE))
		S.erase(v);

	//cerr << WRITE_FUNCTION << "id: " << v << " removing edges " << endl;
	vertex_descriptorN v_src = vertex(v, g);
	ASSERTING(v_src == v);

	//EdgePair epList;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(v_src, g); oit != oit_end; ++oit)
	{
		vertex_descriptorN src = source(*oit, g), targ = target(*oit, g);
	
		// -- debug --
		//cerr << *oit << " " << " prop: " << g[*oit] << " " ;
		//epList.push_back(EdgeId(src, targ));
		ASSERTING(src == v_src);
		ASSERTING(V.contains(targ));
		// -- end debug --

		E.removeEdge(g, src, targ);
		Ep.removeEdge(g[src].id, g[targ].id);
	}
	
	// -- debug remove --
	//for(EdgePair::iterator it = epList.begin(), it_end = epList.end(); it != it_end; ++it)
	//{
	//	cerr << *it << " ";
	//	int src = it->first, targ = it->second;
	//	if(isEdge(src, targ)) remove_edge(src, targ, g);
	//}
	// -- end debug remove --
	//cerr << endl;
	
	V.erase(v_src);
	clear_vertex(v_src, g);
	g[v_src] = VertexProp(); // reset vertex
}

void TreeStructure::insertVertex(int v, const GraphN &global_g)
{
	this->insertVertex(v, global_g[v]);
}

void TreeStructure::addVertex(int v)
{
	int num = num_vertices(g);
	if(num <= v)
	{
		ASSERTING(num <= v);
		int ret = -1;	
	
		for(int i = num; i <= v; i++)
			ret = add_vertex(g);	
			
		ASSERTING(ret == v);
		ASSERTING(num_vertices(g) == v + 1);
	}
}

void TreeStructure::insertVertex(int v, const VertexProp &vp)
{
	addVertex(v);
	
	if(vp.vertexState == STEINER_POINT || vp.vertexState == STEINER_POINT_AVAILABLE)
	{
		S.insert(v);
		g[v] = VertexProp(vp, STEINER_POINT);
	}
	else g[v] = VertexProp(vp);

	V.insert(v);

	ASSERTING(isVertex(v));
}
void TreeStructure::insertVertex(int v, const VertexProp &vp, const VertexState &state)
{
	addVertex(v);
	if(state == STEINER_POINT_AVAILABLE || state == STEINER_POINT) S.insert(v);

	V.insert(v);
	g[v] = VertexProp(vp, state);
	ASSERTING(isVertex(v));
}

void TreeStructure::insertVertex(int v, const VertexState &state, const VertexFunction &func)
{
	addVertex(v);

	if(state == STEINER_POINT_AVAILABLE || state == STEINER_POINT) S.insert(v);

	V.insert(v);
	g[v] = VertexProp(v, state, func);

	ASSERTING(isVertex(v));
}

bool TreeStructure::isVertex(int u) const
{
	ASSERTING(u > -1);

	bool isV = true;
	if(num_vertices(g) > u) // && u > -1)
	{
		vertex_descriptorN src = vertex(u, g);
		ASSERTING(src == u);

		if(g[u].id == -1)  isV = false;
		
		if(!isV)
		{
			//cerr << WRITE_PRETTY_FUNCTION << " v: " << u << " prop : " << g[u] << endl;
			ASSERTING(out_degree(u, g) <= 0);
		}
		else ASSERTING(g[u].id == u);
	}
	else
	{
		isV = false;
        ASSERTING(V.contains(u));
        ASSERTING(S.contains(u));
	}

	return isV;
}

bool TreeStructure::isEdge(int u, int v) const
{
	if(!isVertex(u) || !isVertex(v)) return false;
	
	pair<edge_descriptorN, bool> ep = edge(u,v,g);
	if(!ep.second) return false;

	return true;	
}

int TreeStructure::getOutDegree(int u) const
{
	int od = 0;
	if(isVertex(u)) od = out_degree(u, g);
	
	return od;
}

int TreeStructure::getAvOutDegree(int u) const
{
	return (getDegreeConstraint(g, u) - getOutDegree(u)); 
}


void TreeStructure::insertEdge(edge_descriptorN e, const GraphN &global_g)
{
	int src = source(e, global_g), targ = target(e, global_g);
	this->insertEdge(src, targ, global_g);
}

void TreeStructure::insertEdge(int src, int targ, const GraphN &global_g)
{
	vertex_descriptorN u = vertex(src, global_g);
	vertex_descriptorN v = vertex(targ, global_g);

	if(u != src) cerr << WRITE_FUNCTION << u << " != " << src << endl;
	if(v != targ) cerr << WRITE_FUNCTION << v << " != " << targ << endl;
	ASSERTING(u == src);
	ASSERTING(v == targ);
	
	pair<edge_descriptorN, bool> e = edge(src, targ, global_g);
	ASSERTING(e.second);

	if(!(isVertex(src)))  insertVertex(src, global_g[src]);
	if(!(isVertex(targ))) insertVertex(targ, global_g[targ]);
	
	this->insertEdge(src, targ, global_g[e.first]);
}

void TreeStructure::insertEdge(int src, int targ, const EdgeProp &ep)
{
	//cerr << WRITE_FUNCTION << " src " << src << " targ " << targ << " ep: " << ep << endl;
	if(!isEdge(src, targ)) 
	{
		ASSERTING(num_vertices(g) > src || num_vertices(g) > targ);

		pair<edge_descriptorN, bool> new_e = add_edge(src, targ, g);
		ASSERTING(new_e.second);
		g[new_e.first] = ep;
		g[new_e.first].id = ep.id;
		g[new_e.first].weight = ep.weight;

		E.push_back(new_e.first);
		Ep.insertEdge(g[src].id, g[targ].id);

		// -- debug --
		ASSERTING(g[src].id == src);
		ASSERTING(g[targ].id == targ);
		int u = source(new_e.first, g), v = target(new_e.first, g);
		ASSERTING(src == u || src == v);
		ASSERTING(targ == u || targ == v);
		// -- debug end --
	}
	else // exists -> update edge properties
	{
		ASSERTING(num_vertices(g) > src || num_vertices(g) > targ);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		ASSERTING(e.second);
		g[e.first] = ep;
	
		g[e.first].id = ep.id;
		g[e.first].weight = ep.weight;

		// -- debug --
		ASSERTING(g[src].id == src);
		ASSERTING(g[targ].id == targ);
		// -- end debug --
	}
}

void TreeStructure::updateEdge(const EdgeId &id, const EdgeProp &ep)
{
	insertEdge(id.first, id.second, ep);
}

void TreeStructure::insertEdges(int u, const VertexSet &V, const GraphN &global_g)
{
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
		insertEdge(u, *vit, global_g);
}

void
TreeStructure::insertEdges(int v, const GraphN &gin)
{
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(v, gin); oit != oit_end; ++oit)
		insertEdge(*oit, gin);
}

void TreeStructure::mergeTrees(const TreeStructure &subTree)
{
	VertexSet::const_iterator vit = subTree.V.begin(), vit_end = subTree.V.end();
	for( ; vit != vit_end; ++vit)
	{
		insertVertex(*vit, subTree.g);
	}

	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(subTree.g); eit != eit_end; ++eit)
	{
		int src = source(*eit, subTree.g), targ = target(*eit, subTree.g);
		if(isEdge(src, targ)) continue;
		
		insertEdge(src, targ, subTree.g);
	}
}

void TreeStructure::mergeTrees(const TreeStructure &subTree, const GraphN &global_g)
{
	VertexSet::const_iterator vit = subTree.V.begin(), vit_end = subTree.V.end();
	for( ; vit != vit_end; ++vit)
	{
		insertVertex(*vit, global_g);
	}

	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(subTree.g); eit != eit_end; ++eit)
	{
		int src = source(*eit, subTree.g), targ = target(*eit, subTree.g);
		if(isEdge(src, targ)) continue;
		
		insertEdge(src, targ, global_g);
	}
}
void TreeStructure::diffTrees(const TreeStructure &subTree)
{
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(subTree.g); eit != eit_end; ++eit)
	{
		int src = source(*eit, subTree.g), targ = target(*eit, subTree.g);
		if(isEdge(src, targ))
		{		
			removeEdge(src, targ);
		} 
	}
	
	VertexSet::const_iterator vit = subTree.V.begin(), vit_end = subTree.V.end();
	for( ; vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit) <= 0) removeVertex(*vit);
	}
}

void TreeStructure::removeUnusedVertices()
{
	VertexSet remV;
	VertexSet::iterator vit = V.begin(), vit_end = V.end();
	for( ; vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit) <= 0)
		{
			if(g[*vit].vertexState == GROUP_MEMBER)
			{
				print();
				printGraph();
				printVertexState(cerr);
			}
			
			ASSERTING(g[*vit].vertexState != GROUP_MEMBER);
			remV.insert(*vit);
		}
	}

	vit = remV.begin();
	vit_end = remV.end();
	for( ; vit != vit_end; ++vit)
		removeVertex(*vit);
}

void TreeStructure::getNeighbors(int u, VertexSet &N) const
{
	if(!isVertex(u)) return;
	
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
	{
		int v = target(*oit, g);
		N.insert(v);
	}
}


void TreeStructure::vertexDistance(int u, int v)
{
	// TODO
	ASSERTING(0);
}

void TreeStructure::clearVertex(int u)
{
	if(getOutDegree(u) <= 0) return;

	// gather edges	
	EdgePair remEdges;	
	out_edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = out_edges(u, g); eit != eit_end; ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		remEdges.push_back(EdgeId(src,targ));
	}

	// remove edges
	for(EdgePair::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit) 
		removeEdge(eit->first, eit->second);

	ASSERTING(getOutDegree(u) <= 0);
}

void TreeStructure::print() const
{
    iVertexMapConst idmap = get(&VertexProp::id, g);
	
	VertexSet_cit vit;
	cerr << "V:";
	for(vit = V.begin(); vit != V.end(); ++vit)
		cerr << *vit << ",";

	if(!S.empty())
	{
		cerr << " S:";
		for(vit = S.begin(); vit != S.end(); ++vit)
			cerr << *vit << ",";
	}

	if(!C.empty())
	{
		cerr << " C:";
		for(vit = C.begin(); vit != C.end(); ++vit)
			cerr << *vit << ",";
	}
	
	cerr << " E: ";
	EdgeList_cit eit;
	for(eit = E.begin(); eit != E.end(); ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		cerr << " (" << src << "," << targ << ")";
	}
	cerr << endl;

	cerr << " Ep : " ;
	EdgePair::const_iterator peit;
	for(peit = Ep.begin(); peit != Ep.end(); ++peit)
	{
		cerr << " (" << peit->first << ", " << peit->second << ")" ;
	}
	cerr << endl;
}
void TreeStructure::print(ostream &ostr) const
{
    iVertexMapConst idmap = get(&VertexProp::id, g);
	
	VertexSet_it vit;
	ostr << "V:";
	for(vit = V.begin(); vit != V.end(); ++vit)
		ostr << *vit << ",";
	
	if(!S.empty())
	{
		ostr << " S:";
		for(vit = S.begin(); vit != S.end(); ++vit)
			ostr << *vit << ",";
	}

	if(!C.empty())
	{
		ostr << " C:";
		for(vit = C.begin(); vit != C.end(); ++vit)
			ostr << *vit << ",";
	}	

	ostr << " E: ";
	EdgeList_cit eit;
	for(eit = E.begin(); eit != E.end(); ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		ostr << " (" << src << "," << targ << ")";
	}
	//ostr << endl;

	ostr << " Ep : " ;
	EdgePair::const_iterator peit;
	for(peit = Ep.begin(); peit != Ep.end(); ++peit)
	{
		ostr << " (" << peit->first << ", " << peit->second << ")" ;
	}
	//ostr << endl;
}

void TreeStructure::printGraph() const
{
	// get the property map for vertex indices
    typedef property_map<const GraphN, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, g);
    iVertexMapConst idmap = get(&VertexProp::id, g);

    cerr << "vertices(g) = (";
    pair<vertex_iteratorN, vertex_iteratorN> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		if(idmap[*vp.first] >= 0 )
			cerr << index[*vp.first] << " "; // << " id: " << name[idmap[*vp.first]] << " sp: " << zmap[*vp.first] << ") ";
    }
    cerr << ")" << endl;

    cerr << "edges(g) = ";
    dEdgeMapConst wm = get(&EdgeProp::weight, g);
    graph_traits<GraphN>::edge_iterator ei, ei_end;
    for (boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
        cerr << "((" << index[source(*ei, g)] << "," << index[target(*ei, g)] << ") " ; // w(" << ei.weight << ") " ;
		//EdgeProp ep = get(boost::edge_bundle, g)[*ei];
		cerr << " w: " << wm[*ei] << ") "; // << ep.weight << " ";
    }
    cerr << endl;
}
void TreeStructure::printGraph(ostream &ostr) const
{
	// get the property map for vertex indices
    typedef property_map<const GraphN, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, g);
    iVertexMapConst idmap = get(&VertexProp::id, g);

    ostr << "vertices(g) = (";
    pair<vertex_iteratorN, vertex_iteratorN> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		if(idmap[*vp.first] >= 0 )
			ostr << index[*vp.first] << " "; // << " id: " << name[idmap[*vp.first]] << " sp: " << zmap[*vp.first] << ") ";
    }
    ostr << ")" << endl;

    ostr << "edges(g) = ";
    dEdgeMapConst wm = get(&EdgeProp::weight, g);
    graph_traits<GraphN>::edge_iterator ei, ei_end;
    for (boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
        ostr << "((" << index[source(*ei, g)] << "," << index[target(*ei, g)] << ") " ; // w(" << ei.weight << ") " ;
		//EdgeProp ep = get(boost::edge_bundle, g)[*ei];
		ostr << " w: " << wm[*ei] << ") "; // << ep.weight << " ";
    }
    ostr << endl;
}
void TreeStructure::printVertexState(ostream& ostr) const
{
	vsVertexMapConst vsmap = get(&VertexProp::vertexState, g);
	iVertexMapConst idmap  = get(&VertexProp::id, g);
	
	ostr << "Graph VertexState: " << endl;

	VertexSet::const_iterator it,end;
	for(it = V.begin(), end = V.end(); it != end; it++)
	{
		ostr << g[*it] << endl;
	}
}

void TreeStructure::clearVE()
{
	V.clear();
	E.clear();	
}

void TreeStructure::clear()
{
	V.clear();
	S.clear();
	E.clear();
	Ep.clear();
	g.clear();
}

void TreeStructure::insertEdge(int src, int targ)
{
	if(!(isVertex(src)))  insertVertex(src); 
	if(!(isVertex(targ))) insertVertex(targ);
	
	EdgeProp ep;
	ep.id = pair<int, int>(src, targ);
	ep.delay = 1.0f; //(std::numeric_limits<double>::max)() - 2;
	ep.weight = 1.0f; //(std::numeric_limits<double>::max)() - 2;
	ep.bw = 1000.0; //(std::numeric_limits<double>::max)() - 2;
	ep.cost = 1.0;
	ep.length = 1.0;
	ep.hops = 1;

	this->insertEdge(src, targ, ep); 
}

void TreeStructure::insertVertex(int v)
{
	insertVertex(v, VertexProp(v, NO_STATE, NO_FUNCTION) );
}

}; // namespace TreeAlgorithms

/*
void TreeStructure::createSimVIndex()
{
	ASSERTING(V.size() != 0);
		
	VertexSet::iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		VertexProp vp = g[*vit];
		simV.insertVertex(vp);
	}
}

void TreeStructure::createSimVId()
{
	ASSERTING(V.size() != 0);

	iVertexMap idmap = get(&VertexProp::id, g);
		
	VertexSet::iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		VertexProp vp = g[idmap[*vit]];
		simV.insertVertex(vp);
	}
}

void TreeStructure::createSimEIndex()
{
	ASSERTING(E.size() != 0);
	
	EdgeList::iterator eit, eit_end;
	for(eit = E.begin(), eit_end = E.end(); eit != eit_end; ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		ASSERTING(e.second);
		simE.insertEdge(src, targ, g[e.first]);
	}
}

void TreeStructure::createSimEId()
{
	ASSERTING(Ep.size() != 0);
	
	EdgePair::iterator peit, peit_end;
	for(peit = Ep.begin(), peit_end = Ep.end(); peit != peit_end; ++peit)
	{
		pair<edge_descriptorN, bool> e = edge(peit->first, peit->second, g);
		assert(e.second);
		EdgeProp ep = g[e.first];
		simE.insertEdge(peit->first, peit->second, ep);
	}
}
void TreeStructure::createSimVEIndex()
{
	createSimVIndex();
	createSimEIndex();
}

void TreeStructure::createSimVEId()
{
	createSimVId();
	createSimEId();
}


void TreeStructure::createSimGraphIndex()
{
	ASSERTING(V.size() != 0 || E.size() != 0);
			
	vertex_iteratorN vit, vit_end;
	for(tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		VertexProp vp = g[*vit];
		simG.insertVertex(vp);
	}
	
	edge_iteratorN eit, eit_end;
	for(tie(eit, eit_end) = edges(g); eit != eit_end; ++eit)
	{
		EdgeProp ep = g[*eit];
		int src = source(*eit, g), targ = target(*eit, g);
		simG.insertEdge(src, targ, ep);
	}
}


void TreeStructure::createSimGraphId()
{
	ASSERTING(V.size() != 0 || E.size() != 0);
	iVertexMap idmap = get(&VertexProp::id, g);
			
	vertex_iteratorN vit, vit_end;
	for(tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		VertexProp vp = g[idmap[*vit]];
		simG.insertVertex(vp);
	}
	
	edge_iteratorN eit, eit_end;
	for(tie(eit, eit_end) = edges(g); eit != eit_end; ++eit)
	{
		EdgeProp ep = g[*eit];
		int src = source(*eit, g), targ = target(*eit, g);
		simG.insertEdge(idmap[src], idmap[targ], ep);
	}
}


void TreeStructure::createSimTreeIndex()
{
	ASSERTING(V.size() != 0 || E.size() != 0);
		
	VertexSet::iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		VertexProp vp = g[*vit];
		simTree.insertVertex(vp);
	}
	
	EdgeList::iterator eit, eit_end;
	for(eit = E.begin(), eit_end = E.end(); eit != eit_end; ++eit)
	{
		int src = source(*eit, g), targ = target(*eit, g);
		pair<edge_descriptorN, bool> e = edge(src, targ, g);
		ASSERTING(e.second);
		simTree.insertEdge(src, targ, g[e.first]);
	}	
}

void TreeStructure::createSimTreeId()
{
	assert(0);
	
	ASSERTING(V.size() != 0 || Ep.size() != 0);
	iVertexMap idmap = get(&VertexProp::id, g);
			
	VertexSet::iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		VertexProp vp = g[idmap[*vit]];
		simTree.insertVertex(vp);
	}
	
	EdgePair::iterator peit, peit_end;
	for(peit = Ep.begin(), peit_end = Ep.end(); peit != peit_end; ++peit)
	{
		pair<edge_descriptorN, bool> e = edge(peit->first, peit->second, g);
		assert(e.second);
		EdgeProp ep = g[e.first];
		simTree.insertEdge(peit->first, peit->second, ep);
	}
}
*/


