/***************************************************************************
                          insert_dynamics.cc  -  description
                             -------------------
    begin                : Thu Aug 24 2006
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
#include "../graphstats/recordtree.h"
#include "treestructure.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
		insertVertex():
			start function for inserting one vertex to a tree
------------------------------------------------------------------------- */
void
InsertDynamics::insertVertex(const DynamicTreeAlgo &treeAlgo)
{
    uint64_t a, b;

#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile( 	"push %eax\n"
						"push %ebx\n"
						"push %ecx\n"
						"push %edx\n"
						"cpuid\n"
						"pop %edx\n"
						"pop %ecx\n"
						"pop %ebx\n"
						"pop %eax\n" );

	__asm __volatile(".byte 0x0f,0x31" : "=A" (a));
#endif

	clock_t alg_time = insertVertex_in(treeAlgo);

    uint64_t cpu_cycles = 0;
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile(".byte 0x0f,0x31" : "=A" (b));
	cpu_cycles = b - a;
#endif

	// record stats?
	if(groupInfo_.getMembers().size() > 1)
	{
		StatsContainer SC(groupInfo_.getTreeStructure(), groupInfo_.getPrevTreeStructure(), NO_TREE_ALGO, treeAlgo_, NO_RECONFIG_ALGO, groupInfo_.getGroupId(), alg_time, cpu_cycles);
		recordStats(statout, &SC);
	}

	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//groupT_.print();
	checkInsert(actionVert_);
	// -- debug end --
}

clock_t
InsertDynamics::insertVertex_in(const DynamicTreeAlgo &treeAlgo)
{
	//cerr << WRITE_FUNCTION << " Action Vertex " << actionVert_ << " group Tree : " << groupT_ << endl;
	clock_t exec_time = clock();		// start algorithm timer

	if(groupInfo_.getMembers().size() == 1)
	{
		ASSERTING(groupT_.S.size() == 0);
		ASSERTING(groupT_.E.size() == 0);
		ASSERTING(groupT_.Ep.size() == 0);
		ASSERTING(num_edges(groupT_.g) == 0);
		ASSERTING(groupT_.V.size() == 0);

		//cerr << WRITE_FUNCTION << " inserting actionVert " << actionVert_ << " properties: " << globalG_[actionVert_] << endl;
		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	}
	else if(groupT_.V.contains(actionVert_)) // vertex already in group as steiner point
	{
		//cerr << WRITE_FUNCTION << " groupT contains actionVert " << endl;
		ASSERTING(groupT_.g[actionVert_].vertexState == STEINER_POINT);
		groupT_.makeMember(actionVert_);

		treeAlgo_ = INSERT_MAKE_MEMBER;
		// TODO: steiner points and group members have different degree constraints. Should I reconfigure, adjust for that?
	}
	else
	{
		treeAlgo_ = treeAlgo;
		bool again = true;
		while(again)
		{
			again = false;
			try{
				exec_time = clock();		// start again

				switch(treeAlgo_)
				{
					case INSERT_RANDOM_EDGE:
						ivRandomEdge();
						break;
					case INSERT_CENTER_NODE:
						ivCNode();
						break;
					case INSERT_CENTER_MEMBER_NODE:
						ivCNodeGM();
						break;
					case INSERT_CENTER_WCN:
						ivCNodeSP();
						break;
					//case INSERT_MULTIPLE_CENTER_STEINER_NODE:
					//	ivMCNodeSP();
					//	break;
					case INSERT_MINIMUM_COST_EDGE:
						ivMCE();
						break;
					case INSERT_MINIMUM_COST_EDGE_NON_LEAF:
						ivMCENonLeaf();
						break;
					case INSERT_MINIMUM_COST_EDGE_PRIORITY_WCN:
						ivMCEPrioritySP();
						break;
					case INSERT_MINIMUM_WCN:
						ivMSP();
						break;
					case INSERT_BEST_NEIGHBORHOOD_EDGE:
						ivBNP();
						break;
					case INSERT_MINIMUM_DIAMETER_BEST_NEIGHBORHOOD_EDGE:
						ivMDBNP();
						break;
					case INSERT_MINIMUM_DIAMETER_EDGE:
						ivMDP();
						break;
					case INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE:
						ivMDDLP();
						break;
					case INSERT_MINIMUM_RADIUS_DEGREE_LIMITED_EDGE:
						ivMRDLP();
						break;
					case INSERT_TRY_REPLACE_MC_NAIVE:
						ivTRMCN();
						break;
					case INSERT_TRY_REPLACE_MDDL_NAIVE:
						ivTRMDDLN();
						break;
					// -- delay constrained algorithms --
					case INSERT_DC_CENTER_WCN:
						ivDCCNodeSP();
						break;
					case INSERT_DC_CENTER_NODE:
						ivDCCNode();
						break;
					case INSERT_DC_MINIMUM_COST_EDGE:
						ivDCMCE();
						break;
					case INSERT_DC_MINIMUM_COST_EDGE_PRIORITY_WCN:
					    ivDCMCEPrioritySP();
						break;
					case INSERT_DC_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE:
						ivDCMDDLP();
						break;
					// -- delay constrained algorithms end --
					// -- bounded diameter algorithms --
					case INSERT_BD_CENTER_WCN:
						ivBDCNodeWCN();
						break;
					case INSERT_BD_CENTER_NODE:
						ivBDCNode();
						break;
					case INSERT_BD_MINIMUM_COST_EDGE:
						ivBDMCE();
						break;
					case INSERT_BD_MINIMUM_COST_EDGE_PRIORITY_WCN:
					    ivBDMCEPriorityWCN();
						break;
					// -- bounded diameter algorithms end--
					// -- bounded diameter radius algorithms --
					case INSERT_BR_CENTER_WCN:
						ivBRCNodeWCN();
						break;
					case INSERT_BR_CENTER_NODE:
						ivBRCNode();
						break;
					case INSERT_BR_MINIMUM_COST_EDGE:
						ivBRMCE();
						break;
					case INSERT_BR_MINIMUM_COST_EDGE_PRIORITY_WCN:
					    ivBRMCEPriorityWCN();
						break;
					// -- bounded diameter radius algorithms end--
					case INSERT_SUBGRAPH:
					{
						if(groupInfo_.getMembers().size() < 5)
						{
							treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
							throw true;
						}

						VertexSet steinerSet;
						TreeStructure subT;
						ivSubGraph(subT, steinerSet);
						subTree(groupT_, subT, steinerSet);
						break;
					}
					default:
						cerr << "ERROR! No InsertAlgo given " << treeAlgo_ << endl;
						ASSERTING(0);
						abort();
						break;
				}
			}
			catch(bool in)
			{
				cerr << WRITE_FUNCTION << " trying againg tree algo " << treeAlgo_ << endl;
				again = in;
			}
		}
	}

	return (clock() - exec_time);
}

/*-----------------------------------------------------------------------
		insert vertex:
				multiple center of steiner nodes
------------------------------------------------------------------------- */
void
InsertDynamics::ivMCNodeSP()
{
	int center = groupInfo_.getCenterVertex();

	if(center < 0 || !(groupT_.V.contains(center)) || (getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center))) // elect new center
	{
		center = -1;

		if(!groupInfo_.getSteinerPoints().empty())
			center = GraphAlgorithms::findMDDLWCNode(groupInfo_.getMembers() + groupInfo_.getSteinerPoints(), groupT_.g);

		if(center < 0)
		{
			VertexSet setSP;
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, groupT_.S + actionVert_, setSP, 1, GlobalSimArgs::getSimWCNAlgo());
			if(!setSP.empty())
			{
				center = *(setSP.begin());
				ASSERTING(center >= 0);
				ivMCE(center, STEINER_POINT, globalG_[center].vertexFunction);
			}
			else center = GraphAlgorithms::findMDDLNode(groupT_.V, groupT_.g);
		}

		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}
	else ASSERTING(groupT_.V.contains(center));

	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);
}

/*-----------------------------------------------------------------------
		insert vertex:
				center is well connected node
------------------------------------------------------------------------- */
void
InsertDynamics::ivCNodeSP()
{
	int center = groupInfo_.getCenterVertex();

	if(center < 0 || !(groupT_.V.contains(center)) || (getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center))) // elect new center
	{
		center = -1;

		if(!groupInfo_.getSteinerPoints().empty())
			center = GraphAlgorithms::findMDDLWCNode(groupInfo_.getMembers() + groupInfo_.getSteinerPoints(), groupT_.g);

		if(center < 0)
		{
			VertexSet setSP;
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, groupT_.S + actionVert_, setSP, 1, GlobalSimArgs::getSimWCNAlgo());
			if(!setSP.empty())
			{
				center = *(setSP.begin());
				ASSERTING(center >= 0);
				ivMCE(center, STEINER_POINT, globalG_[center].vertexFunction);
			}
			else center = GraphAlgorithms::findMDDLNode(groupT_.V, groupT_.g);
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

/*-----------------------------------------------------------------------
		insert vertex:
				center is member node
------------------------------------------------------------------------- */
void
InsertDynamics::ivCNodeGM()
{
	int center = groupInfo_.getCenterVertex();

	if(center < 0 || !(groupT_.V.contains(center)) || (getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center))) // elect new center
	{
		center = GraphAlgorithms::findMDDLMemberNode(groupT_.V - groupT_.S, groupT_.g);
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

/*-----------------------------------------------------------------------
		insert vertex:
				center is tree node
------------------------------------------------------------------------- */
void
InsertDynamics::ivCNode()
{
	int center = groupInfo_.getCenterVertex();

	if(center < 0 || !(groupT_.V.contains(center)) || (getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center))) // elect new center
	{
		center = GraphAlgorithms::findMDDLNode(groupT_.V, groupT_.g);
		ASSERTING(center >= 0);
		groupInfo_.setCenterVertex(center);
	}

	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, center, globalG_);

	// -- debug --
	ASSERTING(groupT_.isEdge(actionVert_, center));
	ASSERTING(groupT_.V.contains(center));
	ASSERTING(groupInfo_.getMembers().contains(center) || groupInfo_.getSteinerPoints().contains(center));
	// -- debug end --
}

/*-----------------------------------------------------------------------
		insert vertex:
				center is well connected node
------------------------------------------------------------------------- */
void
InsertDynamics::ivDCCNodeSP()
{
	int src = -1;
	ParentVector parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
	int center = groupInfo_.getCenterVertex();

	double distToSrc = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		distToSrc = distance[center] + globalG_[ep.first].weight;
	}

	if( (center < 0) ||
		!(groupT_.V.contains(center)) ||
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(distToSrc > GlobalSimArgs::getDelayConstraint())) // elect new center
	{
		center = -1;
		if(!groupInfo_.getSteinerPoints().empty())
			center = findDCMDDLWCNode(groupInfo_.getMembers() + groupInfo_.getSteinerPoints(), groupT_.g, globalG_, distance, actionVert_);

		if(center < 0)
		{
			VertexSet setSP;
			findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, groupT_.S + actionVert_, setSP, 1, GlobalSimArgs::getSimWCNAlgo());
			if(!setSP.empty())
			{
				center = *(setSP.begin());
				ASSERTING(center >= 0);
				ivDCMCE(center, STEINER_POINT, globalG_[center].vertexFunction);
			}
			else center = findDCMDDLNode(groupT_.V, groupT_.g, globalG_, distance, actionVert_);
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
InsertDynamics::ivDCCNode()
{
	int src = -1;
	ParentVector parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);
	int center = groupInfo_.getCenterVertex();

	double distToSrc = 0;
	if(center > -1 && groupT_.V.contains(center))
	{
		pair<edge_descriptorN, bool> ep = edge(actionVert_, center, globalG_);
		ASSERTING(ep.second);
		distToSrc = distance[center] + globalG_[ep.first].weight;
	}

	if( (center < 0) ||
		!(groupT_.V.contains(center)) ||
		(getOutDegree(groupT_.g, center) >= getDegreeConstraint(globalG_, center)) ||
		(distToSrc > GlobalSimArgs::getDelayConstraint())) // elect new center
	{
		center = GraphAlgorithms::findDCMDDLNode(groupT_.V, groupT_.g, globalG_, distance, actionVert_);
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

/*-----------------------------------------------------------------------
		insert vertex:
				random edge
------------------------------------------------------------------------- */
void
InsertDynamics::ivRandomEdge()
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int i = 0, targ = -1, failsafe = 0;
	VertexSet::iterator vit, vit_end;

	while(1)
	{
		int vertex = GraphAlgorithms::generateRandomNumber(groupT_.V.size());
		ASSERTING(vertex < groupT_.V.size());

        for(i = 0, vit = groupT_.V.begin(), vit_end = groupT_.V.end(); i < vertex, vit != vit_end; ++vit, i++)
        {
            // just iterate
        }

		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit)) break;

		// -- fail safe --
		failsafe++;
		ASSERTING(failsafe < (numeric_limits<int>::max)());
		if(failsafe >= (numeric_limits<int>::max)()) exit(0);
		// -- fail safe end --
	}

	pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
	ASSERTING(e.second);
	if(wmap[e.first] < path_cost)
	{
		path_cost = wmap[e.first];
		targ = *vit;
	}

	ASSERTING(targ >= 0);
 	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, *vit, globalG_);

	// -- debug --
	ASSERTING(path_cost < (numeric_limits<double>::max)());
	ASSERTING(actionVert_ != targ);

	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
}

/*-----------------------------------------------------------------------
		insert vertex:
			minimum cost edge
------------------------------------------------------------------------- */
void
InsertDynamics::ivMCE()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			path_cost = wmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
	}

	ASSERTING(targ >= 0);

 	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, targ, globalG_);

	// -- debug --
	ASSERTING(path_cost < (numeric_limits<double>::max)());

	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
}

void
InsertDynamics::ivMCE(vertex_descriptorN insVert, VertexState state, VertexFunction func)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(insVert, *vit, globalG_);
		ASSERTING(e.second);
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			path_cost = wmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
	}
	ASSERTING(targ >= 0);

 	groupT_.insertVertex(insVert, state, func);
	groupT_.insertEdge(insVert, targ, globalG_);

	// -- debug --
	ASSERTING(path_cost < (numeric_limits<double>::max)());

	//cerr << WRITE_FUNCTION << " Inserted " << insVert << endl;
	//dumpTreeStructure(groupT_);
}

void
InsertDynamics::ivDCMCE()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
	dEdgeMapConst cmap = get(&EdgeProp::cost, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1, src = -1;

	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double dist = distance[*vit] + wmap[e.first];

		if(cmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			path_cost = cmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
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
		treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}
}

void
InsertDynamics::ivDCMCE(vertex_descriptorN insVert, VertexState state, VertexFunction func)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
	dEdgeMapConst cmap = get(&EdgeProp::cost, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1, src = -1;

	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(insVert, *vit, globalG_);
		ASSERTING(e.second);
		double dist = distance[*vit] + wmap[e.first];

		if(cmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			path_cost = cmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
	}

	if(targ > -1)
	{
 		groupT_.insertVertex(insVert, state, func);
		groupT_.insertEdge(insVert, targ, globalG_);
		//cerr << WRITE_FUNCTION << " Inserted " << insVert << endl;
		//dumpTreeStructure(groupT_);
	}
	else
	{
		ivMCE(insVert, state, func);
	}
}
/*-----------------------------------------------------------------------
		insert vertex:
			minimum cost edge priority is well connected nodes
------------------------------------------------------------------------- */
void
InsertDynamics::ivMCEPrioritySP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	VertexSet normV;
	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		if( !(groupT_.isWCN(*vit)) )
		{
			normV.insert(*vit);
			continue;
		}

		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			path_cost = wmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
	}

	if(targ < 0)
	{
		for(vit = normV.begin(), vit_end = normV.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
			{
				path_cost = wmap[e.first];
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
			}
		}
	}

 	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, targ, globalG_);

	// -- debug --
	ASSERTING(path_cost < (numeric_limits<double>::max)());

	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
}

void
InsertDynamics::ivDCMCEPrioritySP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
	dEdgeMapConst cmap = get(&EdgeProp::cost, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1, src = -1;

	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);

	VertexSet normV;
	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		if( !(groupT_.isWCN(*vit)) )
		{
			normV.insert(*vit);
			continue;
		}

		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double dist = distance[*vit] + wmap[e.first];

		if(cmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			path_cost = cmap[e.first];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
		}
	}

	if(targ < 0)
	{
		for(vit = normV.begin(), vit_end = normV.end(); vit != vit_end; ++vit)
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);
			double dist = distance[*vit] + wmap[e.first];

			if(cmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
			{
				path_cost = cmap[e.first];
				targ = *vit;
				//cerr << "MCE " << e.first << endl;
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
		treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}
}

/*-----------------------------------------------------------------------
		insert vertex:
			minimum cost edge, no leaf
------------------------------------------------------------------------- */
void
InsertDynamics::ivMCENonLeaf()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);

	multimap<double, vertex_descriptorN> mmRankEdges;
	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
			mmRankEdges.insert(pair<double, vertex_descriptorN>(wmap[e.first], *vit));
	}

	int targ = -1;
	double bestWeight = (numeric_limits<double>::max)();
	multimap<double, vertex_descriptorN>::iterator mmit, mmit_end;
	for(mmit = mmRankEdges.begin(), mmit_end = mmRankEdges.end(); mmit != mmit_end; ++mmit)
	{
		if(bestWeight > mmit->first) // case of 2 members -> two leaves
			targ = mmit->second;

		int out_deg = getOutDegree(groupT_.g, mmit->second);
		if(out_deg > 1 && out_deg < getDegreeConstraint(globalG_, mmit->second))
		{
			targ = mmit->second;
			break;
		}
	}
	ASSERTING(targ >= 0);

 	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, targ, globalG_);

	//cerr << WRITE_FUNCTION << " Inserted (" << actionVert_ << ", " << targ << ") " << endl;
	//dumpTreeStructure(groupT_);
}

/*-----------------------------------------------------------------------
		insert vertex:
				minimum diameter edge
------------------------------------------------------------------------- */
void
InsertDynamics::ivMDP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		//cerr << *vit << ": ";

		// worst case distance from *vit to any node in the tree
		double worstcase_dist = 0;
		if(getOutDegree(groupT_.g, *vit) > 0)
			worstCaseDist(*vit, *vit, *vit, groupT_.g, worstcase_dist);

		if((wmap[e.first] + worstcase_dist) < path_cost)
		{
			path_cost = wmap[e.first] + worstcase_dist;
			targ = *vit;
			//cerr << "New: " << e.first << " pathcost " << path_cost << " weight " << wmap[e.first] << " worstcase_dist " << worstcase_dist << endl;
		}
	}

 	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	groupT_.insertEdge(actionVert_, targ, globalG_);

	// -- debug --
	ASSERTING(targ >= 0);
	ASSERTING(path_cost < (numeric_limits<double>::max)());
	ASSERTING(actionVert_ != targ);
	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << " weight + worstcase_dist = " << path_cost << endl;
	//dumpTreeStructure(groupT_);
}
/*-----------------------------------------------------------------------
		insert vertex:
				minimum diameter degree limited edge
------------------------------------------------------------------------- */
void
InsertDynamics::ivMDDLP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);

		// worst case distance from *vit to any node in the tree
		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(groupT_.g, *vit) > 0)
				worstCaseDist(*vit, *vit, *vit, groupT_.g, worstcase_dist);

			if((wmap[e.first] + worstcase_dist) < path_cost)
			{
				path_cost = wmap[e.first] + worstcase_dist;
				targ = *vit;
			}
		}
	}

	if(targ >= 0)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
		groupT_.insertEdge(actionVert_, targ, globalG_);

		// -- debug --
		ASSERTING(path_cost < (numeric_limits<double>::max)());
		ASSERTING(actionVert_ != targ);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << " to " <<  targ << " weight + worstcase_dist = " << path_cost << endl;
		//dumpTreeStructure(groupT_);
		//dumpGraph(groupT_.g);
		//char c = getchar();
	}
	else
	{
		treeAlgo_ = INSERT_TRY_REPLACE_MDDL_NAIVE; //GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}
}

void
InsertDynamics::ivMRDLP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1;

	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	int src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);

	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		//double radius = distance[*vit] + wmap[e.first];

		if(wmap[e.first] + distance[*vit] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			path_cost = wmap[e.first] + distance[*vit];
			targ = *vit;
			//cerr << "MCE " << e.first << endl;
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
		treeAlgo_ = INSERT_TRY_REPLACE_MDDL_NAIVE; //GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}
}

void
InsertDynamics::ivDCMDDLP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	dEdgeMapConst cmap = get(&EdgeProp::cost, globalG_);
 	double path_cost = (numeric_limits<double>::max)();
	int targ = -1, src = -1;

	ParentVector 	parent(num_vertices(groupT_.g));
	DistanceVector 	distance(num_vertices(groupT_.g));
	src = GraphAlgorithms::findMDNode(groupT_.V, groupT_.g);
	ASSERTING(src > -1);

	TreeAlgorithms::createPDVectors(groupT_.g, parent, distance, src);

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		double dist = distance[*vit] + wmap[e.first];

		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit) && dist <= GlobalSimArgs::getDelayConstraint())
		{
			// worst case distance from *vit to any node in the tree
			double worstcase_dist = 0;
			if(getOutDegree(groupT_.g, *vit) > 0)
				worstCaseCost(*vit, *vit, *vit, groupT_.g, worstcase_dist);

			if((cmap[e.first] + worstcase_dist) < path_cost)
			{
				path_cost = cmap[e.first] + worstcase_dist;
				targ = *vit;
			}
		}
	}

	if(targ >= 0)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
		groupT_.insertEdge(actionVert_, targ, globalG_);

		// -- debug --
		ASSERTING(path_cost < (numeric_limits<double>::max)());
		ASSERTING(actionVert_ != targ);
		//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << " to " <<  targ << " weight + worstcase_dist = " << path_cost << endl;
		//dumpTreeStructure(groupT_);
		//dumpGraph(groupT_.g);
		//char c = getchar();
	}
	else
	{
		treeAlgo_ = INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE; //GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}
}

/*-----------------------------------------------------------------------
	insert vertex:

	three cases are possible
	1. Add actionVert_ using MC edge to mcpVert (IMC)
	2. Use actionVert_ as intersection iff mcpVert out-degree > 2 && |neighbors| <= degree constraint
	3. Use new wcn as intersection
	Solution: the case that reduces total cost the most

	TODO: a MDDL version of this function?
	three cases are possible
	1. Add actionVert_ using MDDL edge to mcpVert (IMDDL)
	2. Use actionVert_ as intersection iff mcpVert out-degree > 2 && |neighbors| <= degree constraint
	3. Use new wcn as intersection
	Solution: the case that reduces the diameter the most
------------------------------------------------------------------------- */
void
InsertDynamics::ivTRMCN()
{
	// find MC edge from actionVert_ to group node -> mcpVert
	dEdgeMapConst wmap 	  = get(&EdgeProp::weight, globalG_);
 	double 				  path_cost = (numeric_limits<double>::max)();
	vertex_descriptorN 	  mcpVert = 0;

	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		if(wmap[e.first] < path_cost && getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			path_cost = wmap[e.first];
			mcpVert = target(e.first, globalG_);
			ASSERTING(mcpVert != actionVert_);
		}
	}

	//Find WCN
	int sp_vert = -1;
	VertexSet adjacentSet, steinerSet;
	getNeighbors(groupT_, mcpVert, adjacentSet);
	findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, steinerSet, 1, GlobalSimArgs::getSimWCNAlgo());
	if(steinerSet.size()) sp_vert = *(steinerSet.begin());

	pair<edge_descriptorN, bool> ep;
	double sumEdgesSP = 0;
	double sumEdges = 0;
	double sumEdgesAV = 0;

	int mcpVertOD = getOutDegree(groupT_.g, mcpVert);
	int actionVertDL = getDegreeConstraint(globalG_, actionVert_);
	int spVertDL = 0;
	if(sp_vert > -1) spVertDL = getDegreeConstraint(globalG_, sp_vert);

	out_edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = out_edges(mcpVert, groupT_.g); eit != eit_end; ++eit)
	{
		// using mcpVert as intersection
		sumEdges = sumEdges + groupT_.g[*eit].weight;

		vertex_descriptorN targ = target(*eit, groupT_.g);
		ASSERTING(targ != mcpVert);

		//if(sp_vert > -1)
		if(mcpVertOD <= spVertDL && sp_vert > -1)
		{
			// using sp_vert as intersection
			ep = edge(sp_vert, targ, globalG_);
			ASSERTING(ep.second);
			sumEdgesSP = sumEdgesSP + globalG_[ep.first].weight;
		}

		if(mcpVertOD <= actionVertDL)
		{
			// use actionVert_ as intersection
			ep = edge(actionVert_, targ, globalG_);
			ASSERTING(ep.second);
			sumEdgesAV = sumEdgesAV + globalG_[ep.first].weight;
		}
	}

	//if(sp_vert > -1)
	if(mcpVertOD <= spVertDL && sp_vert > -1)
	{
		// pluss edge weight from SP -> mcpVert
		ep = edge(sp_vert, mcpVert, globalG_);
		ASSERTING(ep.second);
		sumEdgesSP = sumEdgesSP + globalG_[ep.first].weight;

		// pluss edge weight from actionVert_ -> SP
		ep = edge(sp_vert, actionVert_, globalG_);
		ASSERTING(ep.second);
		sumEdgesSP = sumEdgesSP + globalG_[ep.first].weight;
		ASSERTING(!groupT_.isEdge(actionVert_, sp_vert));
	}
	else sumEdgesSP = (numeric_limits<double>::max());

	// pluss edge weight from actionVert_ -> mcpVert
	ep = edge(actionVert_, mcpVert, globalG_);
	ASSERTING(ep.second);

	if(mcpVertOD <= actionVertDL)
	{
		sumEdgesAV = sumEdgesAV + globalG_[ep.first].weight;
	}
	else sumEdgesAV = (numeric_limits<double>::max());

	// pluss edge weight from actionVert_ -> mcpVert
	sumEdges = sumEdges + globalG_[ep.first].weight;

	//cerr << "Steiner Point " << sp_vert << " group member " << mcpVert << " sumEdges: " << sumEdges << " sumEdgeSP " << sumEdgeSP << endl;

	EdgeList remEdges;
	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);

	if(sumEdgesAV < sumEdges && sumEdgesAV < sumEdgesSP) // -> use actionVert_ as intersection
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(mcpVert, groupT_.g); eit != eit_end; ++eit)
		{
			vertex_descriptorN targ = target(*eit, groupT_.g);
			ASSERTING(targ != mcpVert);
			groupT_.insertEdge(actionVert_, targ, globalG_);
			remEdges.push_back(*eit);
		}

		// remove leaf steiner point?
		if(groupT_.isSteinerPoint(mcpVert)) groupT_.removeVertex(mcpVert);
		else groupT_.insertEdge(actionVert_, mcpVert, globalG_);

		//cerr << "-> use actionVert_ as intersection" << endl;
	}
	else if(sumEdgesSP < sumEdges && sumEdgesSP < sumEdgesAV && sp_vert > -1) // -> use sp_vert as intersection
	{
		ASSERTING(sp_vert > -1);
		groupT_.insertVertex(sp_vert, STEINER_POINT, globalG_[sp_vert].vertexFunction);
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(mcpVert, groupT_.g); eit != eit_end; ++eit)
		{
			vertex_descriptorN targ = target(*eit, groupT_.g);
			ASSERTING(targ != mcpVert);
			groupT_.insertEdge(sp_vert, targ, globalG_);
			remEdges.push_back(*eit);
		}
		groupT_.insertEdge(actionVert_, sp_vert, globalG_);

		// remove leaf steiner point?
		if(groupT_.isSteinerPoint(mcpVert)) groupT_.removeVertex(mcpVert);
		else groupT_.insertEdge(mcpVert, sp_vert, globalG_);

		//cerr << "-> use sp_vert as intersection" << endl;
	}
	else //if(sumEdges < sumEdgesAV && sumEdges < sumEdgesSP) // -> use mcpVert as intersection
	{
		groupT_.insertEdge(actionVert_, mcpVert, globalG_);		// MC edge actionVert_ mcpVert
		//cerr << "-> use MC edge actionVert_ mcpVert" << endl;
	}

	// remove edges
	for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
		groupT_.removeEdge(*eit);

	//cerr << WRITE_FUNCTION << "Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
	//char c = getchar();
}
/*-------------------------------------------------------------------------
	a MDDL version of this function?
	three cases are possible
	1. Add actionVert_ using MDDL edge to mcpVert (IMDDL)
	2. Use actionVert_ as intersection iff mcpVert out-degree > 2 && |neighbors| <= degree constraint
	3. Use new wcn as intersection
	Solution: the case that reduces the diameter the most
------------------------------------------------------------------------- */
void
InsertDynamics::ivTRMDDLN()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double mcpDiameter = (numeric_limits<double>::max)();
	double spDiameter = (numeric_limits<double>::max)();
	double avDiameter = (numeric_limits<double>::max)();
	double path_cost = (numeric_limits<double>::max)();
	int mcpVert = -1;

	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		// worst case distance from *vit to any node in the tree
		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);

			double worstcase_dist = 0;
			if(getOutDegree(groupT_.g, *vit) > 0)
				worstCaseDist(*vit, *vit, *vit, groupT_.g, worstcase_dist);

			if((wmap[e.first] + worstcase_dist) < path_cost)
			{
				path_cost = wmap[e.first] + worstcase_dist;
				//mcpDiameter = wmap[e.first];
				mcpVert = *vit;
			}
		}
	}

	ASSERTING(mcpVert > -1);

	// Find WCN
	int sp_vert = -1;
	VertexSet adjacentSet, steinerSet;
	getNeighbors(groupT_, mcpVert, adjacentSet);
	findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, adjacentSet, groupT_.V + actionVert_, steinerSet, 1, GlobalSimArgs::getSimWCNAlgo());
	if(steinerSet.size()) sp_vert = *(steinerSet.begin());
	//cerr << WRITE_FUNCTION << " adjacent set " << adjacentSet <<  " sp vert " << sp_vert << endl;

	// variables
	int mcpVertOD = getOutDegree(groupT_.g, mcpVert);
	int actionVertDL = getDegreeConstraint(globalG_, actionVert_);
	int spVertDL = 0;
	if(sp_vert > -1) spVertDL = getDegreeConstraint(globalG_, sp_vert);

	// 1st: Check using mcpVert as intersection
	mcpDiameter = 0;
	groupT_.insertEdge(mcpVert, actionVert_, globalG_);
	//worstCaseDist(mcpVert, mcpVert, mcpVert, groupT_.g, mcpDiameter);
	mcpDiameter = getTreeDiameter(groupT_);
	//cerr << "mcpVert " << mcpVert << " od : " << mcpVertOD << " diameter " << mcpDiameter << endl;

	int checkVert = mcpVert;
	// 2nd: Check using sp_vert as intersection
	if(mcpVertOD <= spVertDL && sp_vert > -1)
	{
		groupT_.clearVertex(checkVert);
		groupT_.insertEdges(sp_vert, adjacentSet, globalG_);

		// mcpVert becomes sp leaf?
		if(!groupT_.isSteinerPoint(mcpVert)) groupT_.insertEdge(sp_vert, mcpVert, globalG_);

		groupT_.insertEdge(sp_vert, actionVert_, globalG_);
		//spDiameter = 0;
		//worstCaseDist(sp_vert, sp_vert, sp_vert, groupT_.g, spDiameter);
		spDiameter = getTreeDiameter(groupT_);

		//cerr << "sp_vert " << sp_vert << " diameter " << spDiameter << endl;

		checkVert = sp_vert;
	}

	// 3rd: Check using actionVert_ as intersection
	if(mcpVertOD <= actionVertDL)
	{
		groupT_.clearVertex(checkVert);
		groupT_.insertEdges(actionVert_, adjacentSet, globalG_);

		// mcpVert becomes sp leaf?
		if(!groupT_.isSteinerPoint(mcpVert)) groupT_.insertEdge(actionVert_, mcpVert, globalG_);

		//avDiameter = 0;
		//worstCaseDist(actionVert_, actionVert_, actionVert_, groupT_.g, avDiameter);
		avDiameter = getTreeDiameter(groupT_);

		//cerr << "actionVert_ " << actionVert_ << " diameter " << avDiameter << endl;

		checkVert = actionVert_;
	}

	// Now: clean up after checking
	groupT_.clearVertex(checkVert);
	if(sp_vert > -1) groupT_.removeVertex(sp_vert);

	// Insert action vertex
	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);

	// Finally: Which diameter is the smallest?
	if(avDiameter < spDiameter && avDiameter < mcpDiameter)
	{
		// use actionVert_
		groupT_.insertEdges(actionVert_, adjacentSet, globalG_);

		// remove leaf steiner point?
		if(groupT_.isSteinerPoint(mcpVert)) groupT_.removeVertex(mcpVert);
		else groupT_.insertEdge(actionVert_, mcpVert, globalG_);
		//cerr << "-> use actionVert_ as intersection" << endl;
	}
	else if(spDiameter < avDiameter && spDiameter < mcpDiameter && sp_vert > -1)
	{
		groupT_.insertVertex(sp_vert, STEINER_POINT, globalG_[actionVert_].vertexFunction);

		// use sp_vert
		groupT_.insertEdges(sp_vert, adjacentSet, globalG_);

		// remove leaf steiner point?
		if(groupT_.isSteinerPoint(mcpVert)) groupT_.removeVertex(mcpVert);
		else groupT_.insertEdge(mcpVert, sp_vert, globalG_);

		groupT_.insertEdge(sp_vert, actionVert_, globalG_);

		//cerr << "-> use sp_vert " << sp_vert << " as intersection" << endl;
	}
	else // use mcpVert
	{
		// use mcpVert
		groupT_.insertEdges(mcpVert, adjacentSet, globalG_);
		groupT_.insertEdge(actionVert_, mcpVert, globalG_);		// MC edge actionVert_ mcpVert
		//cerr << "-> use MC edge actionVert_ mcpVert" << endl;
	}

	//cerr << WRITE_FUNCTION << "Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
	//char c = getchar();
}

/*-----------------------------------------------------------------------
		insert vertex:
				create subgraph including new vertex
------------------------------------------------------------------------- */
void
InsertDynamics::ivSubGraph(TreeStructure& subT, VertexSet& steinerSet)
{
	dEdgeMapConst wmap 		= get(&EdgeProp::weight, globalG_);
 	double 				  path_cost = (numeric_limits<double>::max)();
	vertex_descriptorN 	  mcpVert = 0;

	for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> e = edge(actionVert_, *vit, globalG_);
		ASSERTING(e.second);
		if(wmap[e.first] < path_cost)
		{
			path_cost = wmap[e.first];
			mcpVert = target(e.first, globalG_);
			ASSERTING(mcpVert != actionVert_);
		}
	}

	// now: I have found minimum cost path vertex
	// next: create subgraph from mcpVert and create subtree from it
	EdgeList remEdges;
	VertexSet subVertices;

	// insert actionVert_
	groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
	subVertices.insert(actionVert_);
	subVertices.insert(mcpVert);

	// find subgraph vertices
	subGraphRingSP(subVertices, steinerSet, actionVert_, mcpVert, mcpVert, remEdges, 2);

	// remove subgraph edges from groupT_
	for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
		groupT_.removeEdge(*eit);

	// create subgraph
	CompleteGraph CG(subVertices, steinerSet, groupInfo_, actionVert_);
	CG.compGraph(subT, globalG_, fifoSteiner_, false);

	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//subT.print();
	//dumpTreeStructure(groupT_);
	//char c = getchar();
}

/*-----------------------------------------------------------------------
		insert vertex:
			better neighborhood path/edge, reduces diameter of tree
		TODO: possible to improve function? Adding worstCaseDist()?
------------------------------------------------------------------------- */
void
InsertDynamics::ivBNP()
{
	//cerr << WRITE_FUNCTION << endl;

 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double neighborhoodRank = (numeric_limits<double>::max)();
	double numNeighbors = 0;
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		double neighborhoodWeight = 0;
		int neighbors = 0;
		double tempRank = (numeric_limits<double>::max)();
		pair<edge_descriptorN, bool> e;

		//cerr << *vit << endl;

		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			calcNeighborHood(*vit, *vit, *vit, neighborhoodWeight, neighbors, 2);

			// connecting edge
			e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);

			// TODO: a function to vekte inn forsinkelse og naboer/hopp ettersom
			if(neighbors == 0) tempRank = wmap[e.first];
			else tempRank = (neighborhoodWeight + wmap[e.first])/neighbors;

			//cerr << "TempRank "  << tempRank << endl;
			// if equal then decider is neighbors
			if((neighborhoodRank > tempRank) || (neighborhoodRank == tempRank && numNeighbors < neighbors))
			{
				neighborhoodRank = tempRank;
				numNeighbors = neighbors;
				ASSERTING(neighborhoodRank > 0);
				targ = *vit;
				//cerr << "new edge " << e.first << endl;
			}
		}
	}

	if(targ >= 0)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
		groupT_.insertEdge(actionVert_, targ, globalG_);

		// -- debug --
		ASSERTING(neighborhoodRank < (numeric_limits<double>::max)());
		ASSERTING(actionVert_ != targ);
	}
	else
	{
		treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}

	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
	//char c = getchar();
}

/*-----------------------------------------------------------------------
		insert vertex:
			better neighborhood path/edge, reduces diameter of tree
		TODO: possible to improve function? Adding worstCaseDist()?
------------------------------------------------------------------------- */
void
InsertDynamics::ivMDBNP()
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, globalG_);
 	double neighborhoodRank = (numeric_limits<double>::max)();
	double numNeighbors = 0;
	int targ = -1;

	VertexSet::iterator vit, vit_end;
	for(vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
	{
		double neighborhoodWeight = 0;
		int neighbors = 0;
		double tempRank = (numeric_limits<double>::max)();
		pair<edge_descriptorN, bool> e;

		if(getOutDegree(groupT_.g, *vit) < getDegreeConstraint(globalG_, *vit))
		{
			calcNeighborHood(*vit, *vit, *vit, neighborhoodWeight, neighbors, 2);

			double worstcase_dist = 0;
			if(getOutDegree(groupT_.g, *vit) > 0)
				worstCaseDist(*vit, *vit, *vit, groupT_.g, worstcase_dist);

			// connecting edge
			e = edge(actionVert_, *vit, globalG_);
			ASSERTING(e.second);

			if(neighbors == 0) tempRank = wmap[e.first];
			else tempRank = (neighborhoodWeight + wmap[e.first] + worstcase_dist)/neighbors;

			//cerr << "TempRank "  << tempRank << endl;
			// if equal then decider is neighbors
			if((neighborhoodRank > tempRank) || (neighborhoodRank == tempRank && numNeighbors < neighbors))
			{
				neighborhoodRank = tempRank;
				numNeighbors = neighbors;
				ASSERTING(neighborhoodRank > 0);
				targ = *vit;
			}
		}
	}

	if(targ >= 0)
	{
 		groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
		groupT_.insertEdge(actionVert_, targ, globalG_);

		// -- debug --
		ASSERTING(neighborhoodRank < (numeric_limits<double>::max)());
		ASSERTING(actionVert_ != targ);
	}
	else
	{
		treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}

	//cerr << WRITE_FUNCTION << " Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
	//char c = getchar();
}

void
InsertDynamics::ivMSP()
{
	VertexSet steinerSet;
	findWCNodes(globalG_, groupT_.g, fifoSteiner_.steinerSet_, groupT_.V + actionVert_, groupT_.S + actionVert_, steinerSet, 1, GlobalSimArgs::getSimWCNAlgo());

	VertexSet::iterator sit = steinerSet.begin(), sit_end = steinerSet.end();
	for( ; sit != sit_end; ++sit)
	{
		double bestImprovement = (numeric_limits<double>::max)();
		vertex_descriptorN bestVert = 0;

		VertexSet MN = groupT_.V - groupT_.S;
		VertexSet::iterator vit = MN.begin(), vit_end = MN.end();
		for( ; vit != vit_end; ++vit)
		{
			if(groupT_.g[*vit].vertexState == STEINER_POINT) continue;

			bool foundSP = false;
			pair<edge_descriptorN, bool> ep;
			double sumEdgeSP = 0;
			double sumEdges = 0;

			out_edge_iteratorN eit, eit_end;
            for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, groupT_.g); eit != eit_end; ++eit)
			{
				sumEdges = sumEdges + groupT_.g[*eit].weight;

				vertex_descriptorN targ = target(*eit, groupT_.g);
				ASSERTING(targ != *vit);
				ep = edge(*sit, targ, globalG_);
				ASSERTING(ep.second);

				sumEdgeSP = sumEdgeSP + globalG_[ep.first].weight;
				if(groupT_.g[targ].vertexState == STEINER_POINT)
				{
					foundSP = true;
					break;
				}
			}

			// do not add two neighboring steiner points
			if(foundSP) continue;

			// pluss edge weight from SP -> *vit
			ep = edge(*sit, *vit, globalG_);
			ASSERTING(ep.second);
			sumEdgeSP = sumEdgeSP + globalG_[ep.first].weight;

			// pluss edge weight from actionVert_ -> SP
			ep = edge(*sit, actionVert_, globalG_);
			ASSERTING(ep.second);
			sumEdgeSP = sumEdgeSP + globalG_[ep.first].weight;

			// pluss edge weight from actionVert_ -> *vit
			ep = edge(actionVert_, *vit, globalG_);
			ASSERTING(ep.second);
			sumEdges = sumEdges + globalG_[ep.first].weight;

			//cerr << "Steiner Point " << *sit << " group member " << *vit << " sumEdges: " << sumEdges << " sumEdgeSP " << sumEdgeSP << endl;
			double improvement = sumEdgeSP/sumEdges;
			if(bestImprovement > improvement)
			{
				bestImprovement = improvement;
				bestVert = *vit;
			}
		}

		if(bestImprovement < 1)
		{
			//cerr << "bestImprovement " << bestImprovement << endl;

			// -- debug --
			if(num_vertices(groupT_.g) > max((int)actionVert_, (int)*sit))
			{
				pair<edge_descriptorN, bool> epair = edge(actionVert_, *sit, groupT_.g);
				ASSERTING(epair.second == false);
			}
			// -- debug end --

			//cerr << "Adding sp " << *sit << endl;
			groupT_.insertVertex(actionVert_, GROUP_MEMBER, globalG_[actionVert_].vertexFunction);
			groupT_.insertVertex(*sit, STEINER_POINT, globalG_[*sit].vertexFunction);

			//cerr << "Adding edges " ;
			EdgeList remEdges;
			out_edge_iteratorN eit, eit_end;
            for(boost::tuples::tie(eit, eit_end) = out_edges(bestVert, groupT_.g); eit != eit_end; ++eit)
			{
				vertex_descriptorN targ = target(*eit, groupT_.g);
				ASSERTING(targ != bestVert);
				//cerr << "(" << *sit << ", " << targ << ") " << endl;

				groupT_.insertEdge(*sit, targ, globalG_);
				remEdges.push_back(*eit);
			}
			groupT_.insertEdge(actionVert_, *sit, globalG_);
			groupT_.insertEdge(bestVert, *sit, globalG_);

			// remove edges
			for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
				groupT_.removeEdge(*eit);
			break;
		}

		if(groupT_.V.contains(actionVert_)) break;
	}

	if(!groupT_.V.contains(actionVert_))
	{
		treeAlgo_ = GlobalSimArgs::insertTreeBackupAlgo();
		throw true;
	}

	//cerr << WRITE_FUNCTION << "Inserted " << actionVert_ << endl;
	//dumpTreeStructure(groupT_);
	//dumpGraph(groupT_.g);
	//char c = getchar();
}

