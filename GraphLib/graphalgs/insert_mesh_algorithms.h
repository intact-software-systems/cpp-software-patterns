/***************************************************************************
                          insert_mesh_algorithms.h  -  description
                             -------------------
    begin                : Sat May 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_INSERT_MESH_ALGORITHMS_KHV
#define GRAPHALGO_INSERT_MESH_ALGORITHMS_KHV

#include "../simdefs.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/insert_dynamics.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"
#include <map>
#include <list>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
				namespace GraphAlgorithms
----------------------------------------------------------------------- */
namespace GraphAlgorithms
{

// mesh start functions
bool insertMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo, clock_t &alg_time, uint64_t &cpu_cycles);
clock_t insertMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo);

// k-single-edges
void kInsertMDDL(const GraphN &g, TreeStructure &groupT, int actionVert, int k);
void kInsertMC(const GraphN &g, TreeStructure &groupT, int actionVert, int k);
void kInsertMCandMDDL(const GraphN &g, TreeStructure &groupT, int actionVert, int k);

};


#endif // ...



