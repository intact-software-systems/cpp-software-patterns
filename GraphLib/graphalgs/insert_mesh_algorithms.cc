/***************************************************************************
                          insert_mesh_algorithms.cc  -  description
                             -------------------
    begin                : Sat May 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "insert_mesh_algorithms.h"
#include "complete_graph.h"
#include "graphalgs.h"
#include "coreh.h"
#include <iostream>
#include <fstream>
#include <time.h>

#include "../treealgs/treealgs.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"
#include "../graphstats/recordtree.h"
#include "../simtime.h"
#include "./meshalgs.h"
#include <cmath>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

namespace GraphAlgorithms
{
/*-----------------------------------------------------------------------
			start function for building meshes
----------------------------------------------------------------------- */
bool insertMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo, clock_t &alg_time, uint64_t &cpu_cycles)
{
#ifdef SYSTEM_SUPPORTS_PUSH_POP
    uint64_t a, b;
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

	alg_time = insertMeshAlgorithm(g, groupInfo, fifoSteiner, actionVert, k, algo);
	
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile(".byte 0x0f,0x31" : "=A" (b));
	cpu_cycles = b - a;
#endif


	// record stats?
	if(groupInfo.getMembers().size() > 1)
	{
		StatsContainer SC(groupInfo.getTreeStructure(), groupInfo.getPrevTreeStructure(), GraphAlgorithms::NO_GRAPH_ALGO, algo, NO_RECONFIG_ALGO, groupInfo.getGroupId(), alg_time, cpu_cycles);
		recordStats(statout, &SC);
	}

	TreeStructure &groupT = (TreeStructure&)groupInfo.getTreeStructure();
	return (isValidMesh(groupT, groupT));
}
		

clock_t insertMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo)
{
	// -- start debug --
	//cerr << WRITE_FUNCTION << SimTime::Now() << " " << algo << " actionVert " << actionVert << " k " << k << endl; 
	// -- end debug --
	
	TreeStructure &groupT = (TreeStructure&)groupInfo.getTreeStructure();
	groupT.setValid(true);
	clock_t start_time = ::clock();		// start algorithm timer

	InsertDynamics ID(g, groupInfo, fifoSteiner, actionVert);
	if(groupInfo.getMembers().size() == 1)
	{
		ASSERTING(groupT.S.size() == 0);
		ASSERTING(groupT.E.size() == 0);
		ASSERTING(groupT.Ep.size() == 0);
		ASSERTING(num_edges(groupT.g) == 0);
		ASSERTING(groupT.V.size() == 0);
		
		//cerr << WRITE_FUNCTION << " inserting actionVert " << actionVert << " properties: " << g[actionVert] << endl;
		groupT.insertVertex(actionVert, GROUP_MEMBER, g[actionVert].vertexFunction);
	}
	else if(groupT.V.contains(actionVert)) // vertex already in group as steiner point
	{
		ASSERTING(groupT.g[actionVert].vertexState == STEINER_POINT);
		groupT.makeMember(actionVert);
	}
	else
	{
		bool again = true;
		while(again)
		{
			again = false;
			try
			{
				// 	-- Execute Dynamic Mesh Algorithm --
				switch(algo)
				{
					case K_INSERT_MDDL:
						kInsertMDDL(g, groupT, actionVert, k);
						break;
					case K_INSERT_MC:
						kInsertMC(g, groupT, actionVert, k);
						break;
					//case ENHANCING_INSERT_MDDL:
					//case ENHANCING_INSERT_MC:
						// add edges to the group subgraph such that it never has more than XX percent more than what a tree would
						// break;

					case K_INSERT_MC_MDDL:
						kInsertMCandMDDL(g, groupT, actionVert, k);
						break;
					default:
						cerr << WRITE_PRETTY_FUNCTION << " algorithm not supported : " << algo << endl;
						ASSERTING(0);
						exit(0);
						break;
				}
			}
			catch(bool in)
			{
				cerr << WRITE_FUNCTION << " trying again dynamic mesh algo " << algo << endl;
				//again = in;
			}
		}
	}

	//if(!checkIsPartitioned(groupT.g) && groupT.V.size() > 1)
	//{
	//	cerr << WRITE_FUNCTION << " new tree : " << groupT << endl;
	//	ASSERTING(0);
	//}
	//ID.rvUnusedSP();

	return (::clock() - start_time); 	// execution time
}

void kInsertMCandMDDL(const GraphN &g, TreeStructure &groupT, int actionVert, int k)
{
	int mc_edges = 0;
	int mddl_edges = 0;
	int i = k % 2;

	if(i == 0) // even number
	{
		mc_edges = k/2;
		mddl_edges = k/2;
	}
	else
	{
		mc_edges = (k + 1)/2;
		mddl_edges = (k - mc_edges);
	}

	kInsertMC(g, groupT, actionVert, mc_edges);
	kInsertMDDL(g, groupT, actionVert, mddl_edges);
}

void kInsertMDDL(const GraphN &g, TreeStructure &groupT, int actionVert, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapEccId;
	
	VertexSet::iterator vit, vit_end;
	for(vit = groupT.V.begin(), vit_end = groupT.V.end(); vit != vit_end; ++vit)
	{
		if(*vit == actionVert) continue;

		// worst case distance from *vit to any node in the mesh
		if(getOutDegree(groupT.g, *vit) < getDegreeConstraint(g, *vit))
		{
			double worstcase_dist = 0;
			if(getOutDegree(groupT.g, *vit) > 0)
				worstCaseDist(*vit, *vit, *vit, groupT, worstcase_dist);

			//cerr << "edge : (" << actionVert << "," << *vit << ")" << endl;
			pair<edge_descriptorN, bool> e = edge(actionVert, *vit, g);
			ASSERTING(e.second);

			double res_eccentricity = wmap[e.first] + worstcase_dist;
			mapEccId.insert(pair<double, int>(res_eccentricity, *vit));
		}
	}

	if(!mapEccId.empty())
	{
 		groupT.insertVertex(actionVert, GROUP_MEMBER, g[actionVert].vertexFunction);	

		int i = 0;
		for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
		{
			int targ = it->second;
			ASSERTING(actionVert != targ);
			groupT.insertEdge(actionVert, targ, g);
		}
	}
	else 
	{
		//treeAlgo_ = INSERT_TRY_REPLACE_MDDL_NAIVE; 
		throw true;
	}
}

void kInsertMC(const GraphN &g, TreeStructure &groupT, int actionVert, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapCostId;
	
	VertexSet::iterator vit, vit_end;
	for(vit = groupT.V.begin(), vit_end = groupT.V.end(); vit != vit_end; ++vit)
	{
		if(*vit == actionVert) continue;

		// worst case distance from *vit to any node in the mesh
		if(getOutDegree(groupT.g, *vit) < getDegreeConstraint(g, *vit))
		{
			pair<edge_descriptorN, bool> e = edge(actionVert, *vit, g);
			ASSERTING(e.second);

			mapCostId.insert(pair<double, int>(wmap[e.first], *vit));
		}
	}

	if(!mapCostId.empty())
	{
 		groupT.insertVertex(actionVert, GROUP_MEMBER, g[actionVert].vertexFunction);	

		int i = 0;
		for(map<double, int>::iterator it = mapCostId.begin(), it_end = mapCostId.end(); i < k && it != it_end; ++i, ++it)
		{
			int targ = it->second;
			ASSERTING(actionVert != targ);
			groupT.insertEdge(actionVert, targ, g);
		}
	}
	else 
	{
		//treeAlgo_ = INSERT_TRY_REPLACE_MDDL_NAIVE; 
		throw true;
	}
}



/*
void kInsertMDDL(InsertDynamics &ID, int actionVert, int k)
{
	for(int i = 0; i < k; i++)
	{
		ID.insertVertex(INSERT_MINIMUM_DIAMETER_DEGREE_LIMITED_EDGE);
	}
}

void kInsertMDDL(const GraphN &g, TreeStructure &groupT, int actionVert, int k)
{
	//void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &excludeSet, VertexSet &newWCN, int k, CoreSelectionAlgo algo)
	VertexSet connectV;
	findWCNodes(g, groupT.g, groupT.V, VertexSet(), connectV, k, AVERAGE_DISTANCE);
	ASSERTING(!connectV.empty());
	
	groupT.insertVertex(actionVert, GROUP_MEMBER, g[actionVert].vertexFunction);

	for(VertexSet::iterator vit = connectV.begin(), vit_end = connectV.end(); vit != vit_end; ++vit)
	{
		ASSERTING(groupT.V.contains(*vit));
		groupT.insertEdge(actionVert, *vit, g);
	}
}
*/

}; // namespace GraphAlgorithms

