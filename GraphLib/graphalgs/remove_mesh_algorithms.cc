/***************************************************************************
                          remove_mesh_algorithms.cc  -  description
                             -------------------
    begin                : Sat May 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "remove_mesh_algorithms.h"
#include "complete_graph.h"
#include "graphalgs.h"
#include "coreh.h"
#include "steiner_set.h"
#include <iostream>
#include <fstream>

#include "../treealgs/treealgs.h"
#include "../network/group_info.h"
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
bool removeMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo, clock_t &alg_time, uint64_t &cpu_cycles)
{
    uint64_t a = 0, b = 0;
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

	alg_time = removeMeshAlgorithm(g, groupInfo, fifoSteiner, actionVert, k, algo);
	
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
		

clock_t removeMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo)
{
	// -- start debug --
	//cerr << WRITE_FUNCTION << SimTime::Now() << " " << algo << " actionVert " << actionVert << " k " << k << endl; 
	// -- end debug --
	
	TreeStructure &groupT = (TreeStructure&)groupInfo.getTreeStructure();
	groupT.setValid(true);
	RemoveDynamics RD(g, groupInfo, fifoSteiner, actionVert);

	clock_t start_time = ::clock();		// start algorithm timer
	
	// 	-- Execute Dynamic Mesh Algorithm --
	if(groupInfo.getMembers().size() <= 1)
	{
		groupT.removeVertex(actionVert);
		groupT.clearSteiner();
		groupT.clearEdges();
					
		//cerr << " GetMembers.size() " << groupInfo.getMembers().size() << " Tree V.size() : " << groupT.V.size() << endl;

		// -- debug --	
		ASSERTING(groupInfo.getMembers().size() == groupT.V.size());
		ASSERTING(groupT.S.size() == 0);
		ASSERTING(groupT.E.size() == 0);
		ASSERTING(groupT.Ep.size() == 0);
		ASSERTING(num_edges(groupT.g) == 0);
		// -- debug end --
	}	
	else if(getOutDegree(actionVert, groupT.g) == 1) // REMOVE_LEAF
	{
		RD.rvLeaf();
	}
	else
	{
		bool again = true;
		while(again)
		{
			again = false;
			try
			{

				// if the number of steiner points in the group tree is more than XX percent -> then call remove with prune

				// 	-- Execute Dynamic Mesh Algorithm --
				switch(algo)
				{
					case K_REMOVE_MDDL:
						kRemoveMDDL(RD, actionVert, k);
						break;
					case K_REMOVE_MC:
						kRemoveMC(RD, actionVert, k);
						break;
					case K_REMOVE_TRY_REPLACE_MC_NAIVE:
						//RD.treeAlgo_ = REMOVE_TRY_REPLACE_MC_NAIVE;
						RD.rvTryReplaceNaive();
						//RD.removeVertex(REMOVE_TRY_REPLACE_MC_NAIVE);
						break;
					case K_REMOVE_TRY_REPLACE_MDDL_NAIVE:
						//RD.treeAlgo_ = REMOVE_TRY_REPLACE_MDDL_NAIVE;
						RD.rvTRMDDLN();
						//RD.removeVertex(REMOVE_TRY_REPLACE_MDDL_NAIVE);
						break;
					case K_REMOVE_TRY_REPLACE_PRUNE_MDDL:
						RD.treeAlgo(REMOVE_TRY_REPLACE_PRUNE_MDDL);
						RD.rvTryReplace();
						//RD.removeVertex(REMOVE_TRY_REPLACE_PRUNE_MDDL);
						break;
					case K_REMOVE_TRY_REPLACE_PRUNE_MC:
						RD.treeAlgo(REMOVE_TRY_REPLACE_PRUNE_MC);
						RD.rvTryReplace();
						//RD.removeVertex(REMOVE_TRY_REPLACE_PRUNE_MC);
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
				again = in;

				algo = K_REMOVE_MC;
			}
		}
	}

	RD.rvUnusedSP();
	//if(!checkIsPartitioned(groupT.g) && groupT.V.size() > 1)
	//{
	//	cerr << WRITE_FUNCTION << " new tree : " << groupT << endl;
	//	ASSERTING(0);
	//}

	return (::clock() - start_time); 	// execution time
}

void kRemoveMDDL(RemoveDynamics &RD, int actionVert, int k)
{
	//for(int i = 0; i < k; i++)
	//{
		RD.removeVertex(REMOVE_MINIMUM_DIAMETER_EDGE);
	//}
}

void kRemoveMC(RemoveDynamics &RD, int actionVert, int k)
{
	//for(int i = 0; i < k; i++)
	//{
		RD.removeVertex(REMOVE_MINIMUM_COST_EDGE);
	//}
}



}; // namespace GraphAlgorithms


