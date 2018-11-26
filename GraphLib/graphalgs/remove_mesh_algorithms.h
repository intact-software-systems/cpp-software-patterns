/***************************************************************************
                          remove_mesh_algorithms.h  -  description
                             -------------------
    begin                : Sat May 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_REMOVE_MESH_ALGORITHMS_KHV
#define GRAPHALGO_REMOVE_MESH_ALGORITHMS_KHV

#include "../simdefs.h"
#include "../treealgs/treestructure.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"
#include "../treealgs/remove_dynamics.h"
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
bool removeMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo, clock_t &alg_time, uint64_t &cpu_cycles);
clock_t removeMeshAlgorithm(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, int actionVert, int k, DynamicMeshAlgo algo);

// k-single-edges
void kRemoveMDDL(RemoveDynamics &RD, int actionVert, int k);
void kRemoveMC(RemoveDynamics &RD, int actionVert, int k);

};


#endif // ...




