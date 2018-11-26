/***************************************************************************
                          bounded_diameter_dynamics.cc  -  description
                             -------------------
    begin                : Sun Jun 17 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
 
#include "insert_dynamics.h"
#include "prim_mst.h"
#include "../simdefs.h"
#include "../graphalgs/coreh.h"
#include "../graphalgs/complete_graph.h"
#include "../graphalgs/graphalgs.h"
//#include "../../graphstats/recordtree.h"
#include "treestructure.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
		addVertex():
			start function for adding a single vertex to a tree	
------------------------------------------------------------------------- */
void
InsertDynamics::ivBDMCE()
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;
	TreeDiameter TD;
	
	//cerr << " group tree : " << groupT_ << endl;
	//dumpGraph(groupT_.g);

	int src = *(groupT_.V.begin()); 
	ASSERTING(src > -1);
	TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
	
	DistanceVector diameter_bound(num_vertices(groupT_.g)), degree_bound(num_vertices(groupT_.g));
	VertexSet degreeBroken, diameterBroken;
	
	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double diameter = TD.distance[*vit] + wmap[e.first]; 
		
		diameter_bound[*vit] = GlobalSimArgs::getDiameterBound();
		degree_bound[*vit] = getDegreeConstraint(globalG_, *vit);

		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && diameter <= GlobalSimArgs::getDiameterBound())
		{
			path_cost = wmap[e.first];	
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
		else if(wmap[e.first] < path_cost) // relax
		{
			if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
				degreeBroken.insert(*vit);
			if(diameter > diameter_bound[*vit])
				diameterBroken.insert(*vit);			
		}
	}

	if(targ > -1)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
		groupT_.insertEdge(actionVert_, targ, globalG_);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
		//dumpTreeStructure(groupT_);
		return;
	}
	else
	{
		targ = relax_ivBDMCE(TD, degree_bound, diameter_bound, degreeBroken, diameterBroken);
		if(targ < 0)
		{	
			treeAlgo_ = INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE; 
			throw true;	
		}
	}
}

int
InsertDynamics::relax_ivBDMCE(const TreeDiameter &TD, DistanceVector &degree_bound, DistanceVector &diameter_bound, VertexSet &degreeBroken, VertexSet &diameterBroken)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	// start relaxing diameter and degree bounds
	while(targ < 0)
	{
		if(!TreeAlgorithms::relaxDegreeAndDiameter(degreeBroken, degree_bound, diameterBroken, diameter_bound)) break; 
		if(!TreeAlgorithms::isRelaxWorking(degreeBroken, diameterBroken)) break;
		
		degreeBroken.clear();
		diameterBroken.clear();
	
		for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			double diameter = TD.distance[*vit] + wmap[e.first]; 
			
			if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < degree_bound[*vit] && diameter <= diameter_bound[*vit])
			{
				path_cost = wmap[e.first];	
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
			}
			else if(wmap[e.first] < path_cost) // relax
			{
				if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
					degreeBroken.insert(*vit);
				if(diameter > diameter_bound[*vit])
					diameterBroken.insert(*vit);			
			}
		}

		
		if(targ > -1)
		{
			groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
			groupT_.insertEdge(actionVert_, targ, globalG_);
			//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
			//dumpTreeStructure(groupT_);
			break;
		}
	}

	return targ;
}

		
void 
InsertDynamics::ivBRMCE()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;
	
	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	int src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
	
	DistanceVector radius_bound(num_vertices(groupT_.g)), degree_bound(num_vertices(groupT_.g));
	VertexSet degreeBroken, radiusBroken;

	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double radius = distance[*vit] + wmap[e.first]; 
		
		radius_bound[*vit] = GlobalSimArgs::getDiameterBound()/2;
		degree_bound[*vit] = getDegreeConstraint(globalG_, *vit);
		
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && radius <= (GlobalSimArgs::getDiameterBound()/2))
		{
			path_cost = wmap[e.first];	
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
		else if(wmap[e.first] < path_cost) // relax
		{
			if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
				degreeBroken.insert(*vit);
			if(radius > radius_bound[*vit])
				radiusBroken.insert(*vit);			
		}
	}
	
	if(targ > -1)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
		groupT_.insertEdge(actionVert_, targ, globalG_);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
		//dumpTreeStructure(groupT_);
		return;
	}
	else
	{
		targ = relax_ivBRMCE(distance, degree_bound, radius_bound, degreeBroken, radiusBroken);
		if(targ < 0)
		{	
			treeAlgo_ = INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE; 
			throw true;
		}
	}
}

int
InsertDynamics::relax_ivBRMCE(const DistanceVector &distance, DistanceVector &degree_bound, DistanceVector &radius_bound, VertexSet &degreeBroken, VertexSet &radiusBroken)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	// start relaxing radius and degree bounds
	while(targ < 0)
	{
		if(!TreeAlgorithms::relaxDegreeAndRadius(degreeBroken, degree_bound, radiusBroken, radius_bound)) break; 
		if(!TreeAlgorithms::isRelaxWorking(degreeBroken, radiusBroken)) break;
		
		degreeBroken.clear();
		radiusBroken.clear();
	
		for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			double radius = distance[*vit] + wmap[e.first]; 
					
			if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < degree_bound[*vit] && radius <= radius_bound[*vit])
			{
				path_cost = wmap[e.first];	
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
			}
			else if(wmap[e.first] < path_cost) // relax
			{
				if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
					degreeBroken.insert(*vit);
				if(radius > radius_bound[*vit])
					radiusBroken.insert(*vit);			
			}
		}
		
		if(targ > -1)
		{
			groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
			groupT_.insertEdge(actionVert_, targ, globalG_);
			//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
			//dumpTreeStructure(groupT_);
			break; 
		}
	}

	return targ;
}

void 
InsertDynamics::ivBDMCEPriorityWCN()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;
	TreeDiameter TD;
		
	int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);
	TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
	
	DistanceVector diameter_bound(num_vertices(groupT_.g)), degree_bound(num_vertices(groupT_.g));
	VertexSet degreeBroken, diameterBroken;
	
	VertexSet normV;
	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		diameter_bound[*vit] = GlobalSimArgs::getDiameterBound();
		degree_bound[*vit] = getDegreeConstraint(globalG_, *vit);

		if( !(groupT_.isWCN(*vit)) ) 
		{
			normV.insert(*vit);
			continue; 
		}
			
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double diameter = TD.distance[*vit] + wmap[e.first]; 
		
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && diameter <= GlobalSimArgs::getDiameterBound())
		{
			path_cost = wmap[e.first];	
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
		else if(wmap[e.first] < path_cost) // relax
		{
			if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
				degreeBroken.insert(*vit);
			if(diameter > diameter_bound[*vit])
				diameterBroken.insert(*vit);			
		}
	}

	if(targ < 0)
	{
		for(vit = normV.begin(), vit_end = normV.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			double diameter = TD.distance[*vit] + wmap[e.first]; 
		
			if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && diameter <= GlobalSimArgs::getDiameterBound())
			{
				path_cost = wmap[e.first];	
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
			}
			else if(wmap[e.first] < path_cost) // relax
			{
				if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
					degreeBroken.insert(*vit);
				if(diameter > diameter_bound[*vit])
					diameterBroken.insert(*vit);			
			}
		}
	}
	
	if(targ > -1)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
		groupT_.insertEdge(actionVert_, targ, globalG_);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
		//dumpTreeStructure(groupT_);
	}
	else
	{
		targ = relax_ivBDMCE(TD, degree_bound, diameter_bound, degreeBroken, diameterBroken);
		if(targ < 0)
		{	
			treeAlgo_ = INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE; 
			throw true;	
		}
	}
}

void 
InsertDynamics::ivBRMCEPriorityWCN()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1, src = -1;
	
	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
		
	DistanceVector radius_bound(num_vertices(groupT_.g)), degree_bound(num_vertices(groupT_.g));
	VertexSet degreeBroken, radiusBroken;

	VertexSet normV;
	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		radius_bound[*vit] = GlobalSimArgs::getDiameterBound()/2;
		degree_bound[*vit] = getDegreeConstraint(globalG_, *vit);
	
		if( !(groupT_.isWCN(*vit)) ) 
		{
			normV.insert(*vit);
			continue; 
		}
			
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double radius = distance[*vit] + wmap[e.first]; 
		
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && radius <= (GlobalSimArgs::getDiameterBound()/2))
		{
			path_cost = wmap[e.first];	
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
		else if(wmap[e.first] < path_cost) // relax
		{
			if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
				degreeBroken.insert(*vit);
			if(radius > radius_bound[*vit])
				radiusBroken.insert(*vit);			
		}
	}

	if(targ < 0)
	{
		for(vit = normV.begin(), vit_end = normV.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			double radius = distance[*vit] + wmap[e.first]; 
		
			if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && radius <= (GlobalSimArgs::getDiameterBound()/2))
			{
				path_cost = wmap[e.first];	
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
			}
			else if(wmap[e.first] < path_cost) // relax
			{
				if(getOutDegree(groupT_.g, *vit) >= degree_bound[*vit])
					degreeBroken.insert(*vit);
				if(radius > radius_bound[*vit])
					radiusBroken.insert(*vit);			
			}
		}
	}
	
	if(targ > -1)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);	
		groupT_.insertEdge(actionVert_, targ, globalG_);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
		//dumpTreeStructure(groupT_);
	}
	else
	{
		targ = relax_ivBRMCE(distance, degree_bound, radius_bound, degreeBroken, radiusBroken);
		if(targ < 0)
		{	
			treeAlgo_ = INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE; 
			throw true;
		}
	}
}


void
InsertDynamics::ivBRCNode()
{
	int src = -1;
	ParentVector parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
	int center = groupInfo_.getCenterVertex();
	
	double radius = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		radius = distance[center] + globalG_[ep.first].weight;
	}
	
	if( (center < 0) || 
		!(groupT_.V.contains(center)) || 
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(radius > GlobalSimArgs::getDiameterBound()/2)) // elect new center
	{
		center = GraphAlgorithms::findBRMDDLNode(groupT_.V, groupT_.g, globalG_, distance, actionVert_);
		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}
	else ASSERTING(groupT_.V.contains(center));

	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);	
	
	// -- debug --	
	ASSERTING(groupT_.isEdge(actionVert_, center));
	ASSERTING(groupT_.V.contains(center));
	// -- debug end --
}


void
InsertDynamics::ivBDCNode()
{
	TreeDiameter TD;
	int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);
	TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);

	int center = groupInfo_.getCenterVertex();
	
	double diameter = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		diameter = TD.distance[center] + globalG_[ep.first].weight;
	}
	
	if( (center < 0) || 
		!(groupT_.V.contains(center)) || 
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(diameter > GlobalSimArgs::getDiameterBound())) // elect new center
	{
		center = GraphAlgorithms::findBDMDDLNode(groupT_.V, groupT_.g, globalG_, TD.distance, actionVert_);
		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}
	else ASSERTING(groupT_.V.contains(center));

	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);	
	
	// -- debug --	
	ASSERTING(groupT_.isEdge(actionVert_, center));
	ASSERTING(groupT_.V.contains(center));
	// -- debug end --
}

void
InsertDynamics::ivBRCNodeWCN()
{
	int src = -1;
	ParentVector parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
	int center = groupInfo_.getCenterVertex();
	
	double radius = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		radius = distance[center] + globalG_[ep.first].weight;
	}
	
	if( (center < 0) || 
		!(groupT_.V.contains(center)) || 
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(radius > GlobalSimArgs::getDiameterBound()/2)) // elect new center
	{
		center = -1;
		if(!groupInfo_.getSteinerPoints().empty()) 
			center = findBRMDDLWCNode(groupInfo_.getMembers() + groupInfo_.getSteinerPoints(), groupT_.g, globalG_, distance, actionVert_);

		if(center < 0)
		{
			VertexSet setSP;
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, VertexSet(), setSP, 1, GlobalSimArgs::getSimWCNAlgo());
			if(!setSP.empty())
			{
				center = *(setSP.begin());
				ASSERTING(center >= 0);
				ivDCMCE(center, STEINER_POINT, globalG_[center].vertexFunction);
			}
			else center = findBRMDDLNode(groupT_.V, groupT_.g, globalG_, distance, actionVert_);
		}

		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}
	else ASSERTING(groupT_.V.contains(center));
	
	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);	
	
	// -- debug --	
	ASSERTING(groupT_.isEdge(actionVert_, center));
	ASSERTING(groupT_.V.contains(center));
	// -- debug end --
}

void
InsertDynamics::ivBDCNodeWCN()
{
	TreeDiameter TD;
	int src = *(groupT_.V.begin()); //GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);
	TreeAlgorithms::worstCaseDist(groupT_.g, groupT_.V, TD, src);
	int center = groupInfo_.getCenterVertex();
	
	double diameter = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		diameter = TD.distance[center] + globalG_[ep.first].weight;
	}
	
	if( (center < 0) || 
		!(groupT_.V.contains(center)) || 
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(diameter > GlobalSimArgs::getDiameterBound())) // elect new center
	{
		center = -1;
		if(!groupInfo_.getSteinerPoints().empty()) 
			center = findBDMDDLWCNode(groupInfo_.getMembers() + groupInfo_.getSteinerPoints(), groupT_.g, globalG_, TD.distance, actionVert_);

		if(center < 0)
		{
			VertexSet setSP;
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, VertexSet(), setSP, 1, GlobalSimArgs::getSimWCNAlgo());
			if(!setSP.empty())
			{
				center = *(setSP.begin());
				ASSERTING(center >= 0);
				ivDCMCE(center, STEINER_POINT, globalG_[center].vertexFunction);
			}
			else center = findBDMDDLNode(groupT_.V, groupT_.g, globalG_, TD.distance, actionVert_);
		}

		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}
	else ASSERTING(groupT_.V.contains(center));
	
	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);	
	
	// -- debug --	
	ASSERTING(groupT_.isEdge(actionVert_, center));
	ASSERTING(groupT_.V.contains(center));
	// -- debug end --
}


