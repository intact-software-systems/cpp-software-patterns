/***************************************************************************
                          boostprop.h  -  description
                             -------------------
    begin                : Tue Feb 14 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef BOOST_PROPERTIES_ALGORITHMS_KHV
#define BOOST_PROPERTIES_ALGORITHMS_KHV

#include <map>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>

class NodeBase;
class EdgeBase;

/*-----------------------------------------------------------------------
		Vertex - Tree/Group states
------------------------------------------------------------------------- */
enum VertexState {
	VERTEX_STATE_ERROR = 0,
	
	NO_STATE,
	GROUP_MEMBER,
	STEINER_POINT,
	STEINER_POINT_AVAILABLE,
	
	END_VERTEX_STATE
};

std::ostream& operator<<( std::ostream& ostr, const VertexState& msg );
const char *printAlgo(VertexState vs);
VertexState getVertexState(char *vs);

/*-----------------------------------------------------------------------
		Vertex - function
------------------------------------------------------------------------- */
enum VertexFunction {
	VERTEX_FUNCTION_ERROR = 0,
	
	NO_FUNCTION,
	CLIENT,
	PROXY,
	SERVER,
	WELL_CONNECTED,
	
	END_VERTEX_FUNCTION
};

std::ostream& operator<<(std::ostream& ostr, const VertexFunction& vf);
const char *printAlgo(VertexFunction vf);
VertexFunction getVertexFunction(char *vf);

/*-----------------------------------------------------------------------
	VertexProp -
				Boost graph vertex descriptor
------------------------------------------------------------------------- */
typedef int VertexId;
typedef std::map<int, double> InterfaceMap;

struct VertexProp 
{
public:
	VertexProp();
	VertexProp(int inid, double x, double y, VertexFunction function);
	VertexProp(int inid, double x, double y, VertexState state, VertexFunction function);

	VertexProp(int inid, VertexFunction function);
	VertexProp(int inid, VertexState state, VertexFunction function);
	VertexProp(const VertexProp& vp);
	VertexProp(const VertexProp& vp, VertexState state);
	VertexProp(const VertexProp& vp, VertexState state, VertexFunction function);
	
	void init();
	void initProp(const VertexProp& vp);

public:
	// -- Vertex Properties --
	VertexId		id;				// vertex identification number
	double 			xpos;			// y position in grid
	double 			ypos;			// x position in grid
	int 			asid;  			// Autonomous System id
	int 			degree_limit;	

	// Steiner Algorithms
	VertexState		vertexState; 	// GROUP_MEMBER, STEINER etc. 
		
	VertexFunction	vertexFunction; // VertexFunction = CLIENT, PROXY, SERVER
	
	// Used in simulator
	NodeBase 		*obj;
	int 			val;				// TODO: what is?
};

std::ostream& operator<<(std::ostream& ostr, const VertexProp& vp);

/*-----------------------------------------------------------------------
		Edge - State
------------------------------------------------------------------------- */
enum EdgeState {
	EDGE_STATE_ERROR = 0,
	
	NO_EDGE_STATE,
	BASIC,
	BRANCH,
	REJECTED,
	
	END_EDGE_STATE
};

std::ostream& operator<<(std::ostream& ostr, const EdgeState& ef);
const char *printAlgo(EdgeState ef);
EdgeState getEdgeState(char *vf);

/*-----------------------------------------------------------------------
	EdgeProp -
				Boost graph edge descriptor
------------------------------------------------------------------------- */
typedef std::pair<int,int> EdgeId;

bool operator==(const EdgeId& e, const EdgeId& f);

struct EdgeProp 
{
public:
	EdgeProp();
	EdgeProp(const EdgeProp& ep);
	EdgeProp(const EdgeProp& ep, EdgeState state);
	EdgeProp(const EdgeProp& ep, int src, int dest);
	EdgeProp(const EdgeProp& ep, EdgeId eid);
	EdgeProp(const EdgeProp& ep, EdgeId eid, EdgeState state);
	EdgeProp(EdgeId eid, double indelay, double inweight, double inbw, double incost, double inlength, int inhops);

	void init(const EdgeProp& ep); 

public:
	// -- Edge Properties --
	EdgeId 			id;			// edge identification std::pair<int, int>
	double 			delay;		// edge delay = weight
	double 			weight;
	double 			bw;			// edge bandwidth (TODO: what is it used for?)
	double			cost; 		// cost = hops
	double			length;     // = brite euclidian length
	EdgeState		edgeState;
	
	// Used in sim
	int 			hops;
	EdgeBase	 	*obj;
};

std::ostream& operator<<(std::ostream& ostr, const EdgeProp& vp);
	
/*-----------------------------------------------------------------------
	Boost Graph - typedefs
------------------------------------------------------------------------- */
// GraphN		// edges, vertices, directed/undirected, vertex props, edge props 
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProp, EdgeProp> GraphN;
								
// Vertex Properties
typedef boost::property_map<GraphN, int VertexProp::*>::type   			iVertexMap;
typedef boost::property_map<GraphN, double VertexProp::*>::type  		dVertexMap;
typedef boost::property_map<GraphN, NodeBase* VertexProp::*>::type 		objVertexMap;
typedef boost::property_map<GraphN, VertexState VertexProp::*>::type	vsVertexMap;
typedef boost::property_map<GraphN, VertexFunction VertexProp::*>::type	vfVertexMap;

// Vertex Properties
typedef boost::property_map<GraphN, int VertexProp::*>::const_type   			iVertexMapConst;
typedef boost::property_map<GraphN, double VertexProp::*>::const_type  			dVertexMapConst;
typedef boost::property_map<GraphN, NodeBase* VertexProp::*>::const_type 		objVertexMapConst;
typedef boost::property_map<GraphN, VertexState VertexProp::*>::const_type		vsVertexMapConst;
typedef boost::property_map<GraphN, VertexFunction VertexProp::*>::const_type	vfVertexMapConst;

// Edge Properties
typedef boost::property_map<GraphN, int EdgeProp::*>::type   			iEdgeMap;
typedef boost::property_map<GraphN, double EdgeProp::*>::type			dEdgeMap;
typedef boost::property_map<GraphN, EdgeBase* EdgeProp::*>::type 		objEdgeMap;
typedef boost::property_map<GraphN, EdgeId EdgeProp::*>::type			eidEdgeMap;

// Edge Properties
typedef boost::property_map<GraphN, int EdgeProp::*>::const_type   		iEdgeMapConst;
typedef boost::property_map<GraphN, double EdgeProp::*>::const_type		dEdgeMapConst;
typedef boost::property_map<GraphN, EdgeBase* EdgeProp::*>::const_type 	objEdgeMapConst;
typedef boost::property_map<GraphN, EdgeId EdgeProp::*>::const_type		eidEdgeMapConst;

// boost::typedefs
typedef boost::graph_traits<GraphN>::vertex_descriptor		vertex_descriptorN;
typedef boost::graph_traits<GraphN>::vertices_size_type  	vertices_size_typeN;
typedef boost::graph_traits<GraphN>::vertex_iterator     	vertex_iteratorN;
typedef boost::graph_traits<GraphN>::edge_descriptor     	edge_descriptorN;
typedef boost::graph_traits<GraphN>::edge_iterator       	edge_iteratorN;
typedef boost::graph_traits<GraphN>::in_edge_iterator    	in_edge_iteratorN;
typedef boost::graph_traits<GraphN>::out_edge_iterator   	out_edge_iteratorN;

typedef std::vector<boost::optional<unsigned> > values_type;
typedef boost::property_map<GraphN, boost::vertex_index_t>::type IndexMap;
typedef boost::graph_traits<GraphN>::degree_size_type degree_size_type;

#endif // BOOST_PROPERTIES_ALGORITHMS_KHV 

