/***************************************************************************
                          boostprop.h  -  description
                             -------------------
    begin                : Thu Feb 16 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "boostprop.h"
#include "treealgs/treealgs.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
	VertexProp -
				Boost graph vertex descriptor
------------------------------------------------------------------------- */
VertexProp::VertexProp() : id(-1), vertexState(NO_STATE), vertexFunction(NO_FUNCTION)
{
	init();
}
VertexProp::VertexProp(int inid, double x, double y, VertexFunction function) : id(inid), xpos(x), ypos(y), vertexState(NO_STATE), vertexFunction(function) 
{
	asid 	= -1;
	val 	= 0;
	obj 	= NULL;
	degree_limit = -1;
}

VertexProp::VertexProp(int inid, double x, double y, VertexState state, VertexFunction function) : id(inid), xpos(x), ypos(y), vertexState(state), vertexFunction(function) 
{
	asid 	= -1;
	val 	= 0;
	obj 	= NULL;
	degree_limit = -1;
}

VertexProp::VertexProp(int inid, VertexFunction function) : id(inid), vertexState(NO_STATE), vertexFunction(function) 
{
	init();
}

VertexProp::VertexProp(int inid, VertexState state, VertexFunction function) : id(inid), vertexState(state), vertexFunction(function)
{
	init();
}
VertexProp::VertexProp(const VertexProp& vp) : vertexState(vp.vertexState), vertexFunction(vp.vertexFunction)
{
	initProp(vp);
}
VertexProp::VertexProp(const VertexProp& vp, VertexState state) : vertexState(state), vertexFunction(vp.vertexFunction)
{
	initProp(vp);
}
VertexProp::VertexProp(const VertexProp& vp, VertexState state, VertexFunction function) : vertexState(state), vertexFunction(function)
{
	initProp(vp);
}
/*-----------------------------------------------------------------------
						init()
------------------------------------------------------------------------- */
void VertexProp::init() 
{
	xpos 			= 0;
	ypos 			= 0;
	asid 			= -1;
	val 			= 0;
	obj 			= NULL;
	degree_limit 	= -1;
}

void VertexProp::initProp(const VertexProp& vp) 
{
	id 				= vp.id;
	xpos 			= vp.xpos;
	ypos 			= vp.ypos;
	asid 			= vp.asid;
	val 			= vp.val;
	obj 			= vp.obj;
	degree_limit	= vp.degree_limit;
}

/*-----------------------------------------------------------------------
	EdgeProp -
				Boost graph edge descriptor
------------------------------------------------------------------------- */
EdgeProp::EdgeProp() : id(-1,-1), delay(0), weight(0), bw(0), cost(0), length(0), edgeState(BASIC), hops(0), obj(NULL)
{ 
}
EdgeProp::EdgeProp(const EdgeProp& ep) : id(ep.id) 
{ 
	init(ep);
}

EdgeProp::EdgeProp(const EdgeProp& ep, int src, int dest) : id(src, dest)	
{
	assert(id.first > -1 && id.second > -1);
	init(ep);
}
EdgeProp::EdgeProp(const EdgeProp& ep, EdgeId eid) : id(eid) 
{
	assert(id.first > -1 && id.second > -1);

	id = eid;
	init(ep);
}
EdgeProp::EdgeProp(const EdgeProp& ep, EdgeState state)
{
	init(ep);
	edgeState = state;
}
EdgeProp::EdgeProp(const EdgeProp& ep, EdgeId eid, EdgeState state) : id(eid) 
{
	assert(id.first > -1 && id.second > -1);

	init(ep);

	id = eid;
	edgeState = state;
}

EdgeProp::EdgeProp(EdgeId eid, double indelay, double inweight, double inbw, double incost, double inlength, int inhops) : 
	id(eid), delay(indelay), weight(inweight), bw(inbw), cost(incost), length(inlength), hops(inhops) 
{
	assert(id.first > -1 && id.second > -1);
	assert(delay >= 0);
	assert(weight >= 0);
	assert(bw >= 0);
	assert(cost >= 0);
	assert(hops >= 0);

	edgeState = BASIC;
}

/*-----------------------------------------------------------------------
						init()
------------------------------------------------------------------------- */
void EdgeProp::init(const EdgeProp& ep) 
{
	delay 	= ep.delay; 
	weight 	= ep.weight;
	bw 		= ep.bw;
	cost	= ep.cost;
	length  = ep.length;
	hops 	= ep.hops;
	obj 	= ep.obj; 	
	edgeState = ep.edgeState;

	assert(delay >= 0);
	assert(weight >= 0);
	assert(bw >= 0);
	assert(cost >= 0);
	assert(hops >= 0);
}

/*-----------------------------------------------------------------------
						operator<< - VertexState
------------------------------------------------------------------------- */
const char *stringVertexState[] = { "error", "no_algo", "member", "steiner", "steiner-available" };
const char *printAlgo(VertexState vs)
{
	return stringVertexState[vs];
}

VertexState getVertexState(char *vs)
{
	for( int i = 0; i < END_VERTEX_STATE; i++)
		if(strcmp(vs, stringVertexState[i]) == 0) return (VertexState) i;

	return (VertexState) 0;
}

std::ostream& operator<<( std::ostream& ostr, const VertexState& msg )
{
    switch( msg )
    {
#define CASE(t) case t: ostr << #t; break;
    CASE(VERTEX_STATE_ERROR)
	CASE(NO_STATE)
	CASE(GROUP_MEMBER)
	CASE(STEINER_POINT)
	CASE(STEINER_POINT_AVAILABLE)
#undef CASE
	default:
		cerr << "Missing operator<< case for VertexState " << (int) msg << endl;
		assert(0);
		break;
	}
	return ostr;
}	
/*-----------------------------------------------------------------------
						operator<< - VertexFunction
------------------------------------------------------------------------- */
const char *stringVertexFunction[] = { "error", "no_algo", "client", "proxy", "server", "wcn"};
const char *printAlgo(VertexFunction vf)
{
	return stringVertexFunction[vf];
}

VertexFunction getVertexFunction(char *vf)
{
	for( int i = 0; i < END_VERTEX_FUNCTION; i++)
		if(strcmp(vf, stringVertexFunction[i]) == 0) return (VertexFunction) i;

	return (VertexFunction) 0;
}

std::ostream& operator<<(std::ostream& ostr, const VertexFunction& vf)
{
	switch(vf)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(VERTEX_FUNCTION_ERROR)
	CASE(NO_FUNCTION)
	CASE(CLIENT)
	CASE(PROXY)
	CASE(SERVER)
	CASE(WELL_CONNECTED)
#undef CASE	
	default:
		ostr << "Missing operator<< case for VertexFunction " << endl;
		assert(0);
		break;	
	}
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const VertexProp& vp)
{
	ostr << "V: " << vp.id << "\t" << vp.vertexFunction << "\t" << vp.vertexState ;

	if(vp.degree_limit > -1)
		ostr << " dl: " << vp.degree_limit ;

	return ostr;
}

/*-----------------------------------------------------------------------
						operator<< - EdgeState
------------------------------------------------------------------------- */
const char *stringEdgeState[] = { "error", "no_algo", "basic", "branch", "rejected" };
const char *printAlgo(EdgeState vs)
{
	return stringEdgeState[vs];
}

EdgeState getEdgeState(char *vs)
{
	for( int i = 0; i < END_EDGE_STATE; i++)
		if(strcmp(vs, stringEdgeState[i]) == 0) return (EdgeState) i;

	return (EdgeState) 0;
}

std::ostream& operator<<( std::ostream& ostr, const EdgeState& msg )
{
    switch( msg )
    {
#define CASE(t) case t: ostr << #t; break;
    CASE(EDGE_STATE_ERROR)
	CASE(NO_EDGE_STATE)
	CASE(BASIC)
	CASE(BRANCH)
	CASE(REJECTED)
#undef CASE
	default:
		cerr << "Missing operator<< case for EdgeState " << (int) msg << endl;
		assert(0);
		break;
	}
	return ostr;
}	

std::ostream& operator<<(std::ostream& ostr, const EdgeProp& ep)
{
	ostr << "E: (" << ep.id.first << "," << ep.id.second << ") w: " <<  ep.weight << " c: " << ep.cost ;
	return ostr;
}

bool operator==(const EdgeId& e, const EdgeId& f)
{
	if(e.first == f.first && e.second == f.second)
		return true;	
	
	if(e.second == f.first && e.first == f.second)
		return true;
		
	//cerr << e << f << " are not equal " << endl;
	return false;	
}


