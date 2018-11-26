/***************************************************************************
                          adh.cc  -  description
                             -------------------
    begin                : Thu Oct 6 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "adh.h"
#include <fstream>
#include "../simtime.h"
#include "../treealgs/floyd_warshall_apsp.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

void
AverageDistanceHeuristic::cleanup()
{
	vector<TreeStructure*>::iterator vit;
	for(vit = TreeBundle_adh.begin(); vit != TreeBundle_adh.end(); ++vit)
		delete *vit;
	
	TreeBundle_adh.clear();
}

/* -------------------------------------------------------------------------
	
		Algorithm(): Average Distance Heuristic start function

------------------------------------------------------------------------- */
void
AverageDistanceHeuristic::Algorithm(int zsource)
{
	Initialize(NO_SOURCE_NODE);	 // identify z-nodes and store them in vector<MyWrapper> ZVert
	
	// -- fail safe --
	if(num_zvertices <= 0) 
	{
		cerr << "[ADH::Algorithm] Error: No Z-vertices." << endl;	
		ASSERTING(num_zvertices > 0);
	} 
	// -- end fail safe -- 
	
	cerr << "[ADH::Algorithm] Start ADH with " << num_zvertices << " z-nodes" << endl;
	//cerr << " input V " << inTree << endl;
	//inTree.printVertexState(cerr);

	// -- Start ADH Algorithm --
	RunFloydWarshall();
		
	//while((dnt_matrix[1]).size() > 1)			// TODO: Ugly
	while(!T_adh.V.contains(ZVertSet))
	{
		int fm_vert = -1;						// the f(m) vertex	
		int fm_r = 0;							// fm_r decides how many trees to connect in each iteration
		
		FindMNode(fm_vert, fm_r);				// find the f(m) vertex -- the "best" located node according to RayWardSmithf()
		ASSERTING(fm_vert > -1);
		
		AddClosestZVertices(fm_vert, fm_r); 	// add fm_r closest trees by shortest path from fm_vert
	} 

	cleanup();
	
	// -- start debug --
	//cerr << "[ADH::Algorithm] Produced Steiner Tree: " << endl;
	//dumpTreeStructure(T_adh);
	//dumpGraph(T_adh.g);
	// -- end debug --
}

/* -------------------------------------------------------------------------
	
		AddClosestZVertices(): Adds the closest z-vertices from f(m)

------------------------------------------------------------------------- */
void
AverageDistanceHeuristic::AddClosestZVertices(int fm_vert, int fm_r)
{
	vsVertexMapConst g_vsmap = get(&VertexProp::vertexState, g);
	//vsVertexMap g_vsmap = get(&VertexProp::vertexState, g);

	while(fm_r > 0) 	// find fm_r closest z-nodes from f(m) in the nt_matrix
	{
		multimap<double, TreeStructure*>::iterator mmit_nt, mmit_nt_end;
		for(mmit_nt = (dnt_matrix[fm_vert]).begin(), mmit_nt_end = (dnt_matrix[fm_vert]).end(); mmit_nt != mmit_nt_end; ++mmit_nt)
		{
			// dnt_matrix will contain trees with zero distance when a node is a part of that tree
			// Therefore: skip this tree and continue until distance > 0
			if(mmit_nt->first > 0)
				break;
		}
		if(mmit_nt == mmit_nt_end) break;

		fm_r--;
		
		//Nxt: 	merge the fm_r closest trees and the fm_vert into one column in the dnt_matrix
		TreeStructure *pTadh_fm = NULL, *pTadh_nt = NULL; 		// find where f(m) and d(n,t) is located in the vertTOtree_map
		pTadh_fm = vertTOtree_map[fm_vert];
		pTadh_nt = vertTOtree_map[*((*mmit_nt).second->V.begin())];

		// -- debug --
		//dumpDNTMatrix();
		//cerr << "[ADH::AddClosestZVertices] " << fm_vert << " Add to tree: pTadh_nt: " << *pTadh_nt << endl << " d:" << (*mmit_nt).first << endl;
		ASSERTING(pTadh_nt);
		ASSERTING(pTadh_nt == (*mmit_nt).second);
		ASSERTING(pTadh_nt->V.count(fm_vert) == 0); 
		ASSERTING(pTadh_fm != pTadh_nt); 
		//ASSERTING(!(g_vsmap[fm_vert] == STEINER_POINT && pTadh_fm == NULL));
		ASSERTING(!(steinerSet.contains(fm_vert) && pTadh_fm == NULL));
		// -- debug end --
		
		// variables
		map<int, multimap<double, TreeStructure*> >::iterator dntit, dntit_end;
		multimap<double, TreeStructure*>::iterator dntit_in, dntit_in_end, distfm_it, distnt_it;
		double distfm = 0, distnt = 0;
		
		// -- if f(m) is steiner it is in the dnt_matrix -- then merge two columns inside dnt_matrix --
		//if(pTadh_fm && (g_vsmap[fm_vert] == STEINER_POINT || g_vsmap[fm_vert] == GROUP_MEMBER))
		if(pTadh_fm && (steinerSet.contains(fm_vert) || g_vsmap[fm_vert] == GROUP_MEMBER))
		{
			AddEdges(fm_vert, pTadh_fm, pTadh_nt);
			pTadh_nt->mergeTrees(*pTadh_fm);
			
			// find minimum distance - dnt_matrix pTadh_nt and dnt_matrix pTadh_fm - treating them as one contracted piece of the graph
			for(dntit = dnt_matrix.begin(), dntit_end = dnt_matrix.end(); dntit != dntit_end; ++dntit)
			{
				for(dntit_in = dntit->second.begin(), dntit_in_end = dntit->second.end(); dntit_in != dntit_in_end; ++dntit_in)
				{
					if(pTadh_fm == dntit_in->second)
					{
						distfm = dntit_in->first;
						distfm_it = dntit_in;
					}
					if(pTadh_nt == dntit_in->second)
					{
						distnt = dntit_in->first;
						distnt_it = dntit_in;
					}
				}

				//if((g_vsmap[dntit->first] == STEINER_POINT) && (pTadh_nt->V.count(dntit->first) == 1))
				if(steinerSet.contains(dntit->first) && (pTadh_nt->V.count(dntit->first) == 1))
				{
					//cerr << "1st " << dntit->first << " distnt " << distnt << " distfm " << distfm << " min: " << std::min(distnt, distfm) << " pTadh_nt->V.count(dntit->first)= " << pTadh_nt->V.count(dntit->first) << endl;
					distnt = 0;
				}

				// insert the new distance and a pointer to the tree pTadh_nt 
				(dnt_matrix[dntit->first]).insert(pair<double, TreeStructure*>(std::min(distnt, distfm), pTadh_nt));
				(dnt_matrix[dntit->first]).erase(distfm_it); 		// erase old positions
				(dnt_matrix[dntit->first]).erase(distnt_it);		// erase old positions
				
				//cerr << std::min(distnt, distfm) << " ";
				distnt = distfm = 0;
			}
			//cerr << endl;
		}
		// -- else if not steiner then f(m) is not in the dnt_matrix -- then compare columns dnt_matrix (*mmit_nt).second and  fw_matrix[fm_vert] and find minimum distances --
		//else if(g_vsmap[fm_vert] != STEINER_POINT)
		else if(!steinerSet.contains(fm_vert))
		{
			// now: pTadh_fm == NULL -- it is an unused steiner point and has no tree attached to it
			AddEdges(fm_vert, pTadh_fm, pTadh_nt);

			steinerSet.insert(fm_vert);
			//g_vsmap[fm_vert] = STEINER_POINT;
			pTadh_nt->insertVertex(fm_vert, g);

			// nxt: find minimum distances -- result -> treats pTadh_fm and pTadh_nt as a contracted part of the graph
			map<int, double>::iterator 	fw_mit, fw_mit_end;
			for(dntit = dnt_matrix.begin(), dntit_end = dnt_matrix.end(), fw_mit = fw_matrix[fm_vert].begin(), fw_mit_end = fw_matrix[fm_vert].end(); dntit != dntit_end, fw_mit != fw_mit_end; ++dntit, ++fw_mit)
			{
				for(dntit_in = dntit->second.begin(), dntit_in_end = dntit->second.end(); dntit_in != dntit_in_end; ++dntit_in)
				{
					if(pTadh_nt == dntit_in->second)
					{
						distnt = dntit_in->first;
						distnt_it = dntit_in;
					}
				}

				//if((g_vsmap[dntit->first] == STEINER_POINT) && (pTadh_nt->V.count(dntit->first) == 1))
				if(steinerSet.contains(dntit->first) && (pTadh_nt->V.count(dntit->first) == 1))
				{
					//cerr << "2nd " << dntit->first << " distnt " << distnt << " distfm " << distfm << " min: " << std::min(distnt, distfm) << " pTadh_nt->V.count(dntit->first)= " << pTadh_nt->V.count(dntit->first) << endl;
					distnt = 0;
				}

				// insert the new distance and a pointer to the tree pTadh_nt merged right after the for loop
				distfm = fw_mit->second;
				(dnt_matrix[dntit->first]).insert(pair<double, TreeStructure*>(std::min(distnt, distfm),pTadh_nt));
				(dnt_matrix[dntit->first]).erase(distnt_it); 		// erase old position
				
				//cerr << std::min(distnt, distfm) << " ";
				distnt = distfm = 0;
			}		
			//cerr << endl;

		} // end else if
		else
		{
			cerr << "FUCK! fm_vert: " << fm_vert << " fm_r : " << fm_r << " vertex state " << g_vsmap[fm_vert] << endl;
			ASSERTING(0);
			exit(0);
		}

		// update tree links
		vertTOtree_map[fm_vert] = pTadh_nt;
		for(VertexSet_it sit = pTadh_nt->V.begin(); sit != pTadh_nt->V.end(); ++sit)
			vertTOtree_map[*sit] = pTadh_nt;

		// -- Final tree --
		// Explanation: in the last iteration pTadh_nt will hold the final tree that connected pTadh_fm and pTadh_nt
		T_adh = *pTadh_nt;	
	}
}

/* -------------------------------------------------------------------------
	
		AddEdges(): Adds new edges to pTadh_nt tree -- adding link
			from fm_vert to pTadh_nt. Whatever vertex is chosen in
			pTadh_nt.V "should" make no difference to what edge is
			chosen and added. But -- who knows.

------------------------------------------------------------------------- */

void					// AddEdges(fm_vert, vertTOtree_map[fm_vert], (*mmit_nt).second);
AverageDistanceHeuristic::AddEdges(int fm_vert, TreeStructure* pTadh_fm, TreeStructure* pTadh_nt)
{
	// find minimum(fm_vert, vertex(pTadh_nt.V))
	// Find minimum distance from fm_vert to new tree
	double min_dist = MAXIMUM_WEIGHT;
	int focus_vert = -1; 		
	map<int, double>::iterator mit;
	
	VertexSet_it svit, svit_end;
	for(svit = pTadh_nt->V.begin(), svit_end = pTadh_nt->V.end(); svit != svit_end; ++svit)
	{	
		ASSERTING((fw_matrix[fm_vert]).count(*svit) == 1);
		
		mit = (fw_matrix[fm_vert]).find(*svit);		
		if(min_dist > mit->second)
		{
			focus_vert = mit->first;
			min_dist = mit->second;
		}
	}
	
	ASSERTING(focus_vert >= 0); 
	
	int from_vert		= fm_vert;
	int parent_vert 		= (int)(parent_matrix[focus_vert])[fm_vert];

	while(from_vert != focus_vert)
	{
		// find shortest path to/from (fm_vert, pTadh_nt->V.begin()) -- mmit_nt is the closest vertex to f(m) - it is found in the nt_matrix
		pair<edge_descriptorN, bool> adh_edge = edge(from_vert, parent_vert, g);
		if(adh_edge.second)	pTadh_nt->insertEdge(from_vert, parent_vert, g);

		from_vert = parent_vert;
		parent_vert = (int)(parent_matrix[focus_vert])[parent_vert];		// traverse parent matrix
		
		if(get(&VertexProp::vertexState, g)[from_vert] != GROUP_MEMBER)
		{
			steinerSet.insert(from_vert);
			//get(&VertexProp::vertexState, g)[from_vert] = STEINER_POINT;
			pTadh_nt->insertVertex(from_vert, g);
			vertTOtree_map[from_vert] = pTadh_nt;
		}
	}
}

/* -------------------------------------------------------------------------
	
		FindMNode(): Finds f(m) using RaywardSmithf(..)

------------------------------------------------------------------------- */
void
AverageDistanceHeuristic::FindMNode(int &fm_vert, int &fm_r)
{
	double fm_min = MAXIMUM_WEIGHT;
	double curr_fn = 0;
	int r = 1;
	//cerr << "[ADH::FindMNode]:  " << endl;

	map<int, multimap<double, TreeStructure*> >::iterator mit, mit_end;
	multimap<double, TreeStructure*>::iterator vit, vit_end;
	for(mit = dnt_matrix.begin(), mit_end = dnt_matrix.end(); mit != mit_end; ++mit)
	{
		for(vit = (*mit).second.begin(), vit_end = (*mit).second.end(); vit != vit_end; ++vit)
			if((*vit).first > 0) break;
		
		curr_fn = RaywardSmithf((r=1),0,(*mit).first, vit, vit_end); //(*mit).second.begin());
		//cerr <<" f(" << (*mit).first << ") = " << curr_fn << endl;
		
		if(curr_fn < fm_min) 	// find minimum f(n) => f(m(in))
		{
			fm_vert = (*mit).first;
			fm_min = curr_fn;
			fm_r = r;
		}		
	}
	
	if(fm_vert < 0)
	{
		cerr << WRITE_FUNCTION << " couldn't find new fm_vert " << fm_vert << endl;
		cerr <<"[ADH::FindMNode] f_r(m) = f_"<< fm_r <<"(" << fm_vert << ") = " << fm_min << endl;
		ASSERTING(fm_vert > -1);
	}
	
	//cerr <<"[ADH::FindMNode] f_r(m) = f_"<< fm_r <<"(" << fm_vert << ") = " << fm_min << endl;
}

/* -------------------------------------------------------------------------
	
		RaywardSmithf(): Recursive function
		
	f(n) = min(1<=r<=k){ (sum of i=0 to r) d(n,t_i)/r:t_0,t_1,...,t_r in T}

------------------------------------------------------------------------- */
double
AverageDistanceHeuristic::RaywardSmithf(int &r, double fn, int v, multimap<double, TreeStructure*>::iterator vit, multimap<double, TreeStructure*>::iterator vit_end)
{
	double dn = (*vit).first;			// easier read

	if(dn <= 0)// || dn > MAXIMUM_WEIGHT)  // -- fail safe --
	{
		cerr << "[ADH::RaywardSmithf] Error: dn " << dn << " Exiting. " << endl; ASSERTING(0); exit(0);
	} // -- end fail safe --
	
	if(get(&VertexProp::vertexState, g)[v] == GROUP_MEMBER) // z-node? then f(n) is d(n,z) -> distance from n to closest z-node
	{
		//cerr << "(" << v << ","; dumpSteinerTree(*(*vit).second); cerr << ")" << " d_n.0 " << dn << endl;
		return dn;
	}
	else if(r == 1) 	// first round? then f(n) = d(n,t_n.0) + d(n,t_n.1)
	{
		//cerr << "(" << v << ","; dumpSteinerTree(*(*vit).second); cerr << ")";

		vit++;
		if(vit == vit_end) return dn;

		double dn1 = ((*vit).first);
		fn = dn + dn1;
		//cerr << "(" << v << ","; dumpSteinerTree(*(*vit).second); cerr << ") ";
		//cerr << "f_" << r <<"(" << v << ") = d_n.0 " << dn << " + d_n.1 " << dn1 << " = " << fn << endl;

	}
	else if(r > 1 && dn < fn ) // f(n) = ((r-1)f_r(n) + d(n, t_n.r))/r
	{
		//cerr << "(" << v << ","; dumpSteinerTree(*(*vit).second); cerr << ") ";
		//cerr << "f_" << r <<"(" << v << ") = f_" << r-1 << " " << fn << " + d_n." << r << " " << dn;
		fn = ((r-1)*fn + dn)/r;	
		//cerr << " = " << fn << endl;
	}
	
	vit++;
	
	//if(*(vit->first) > 0 && dn < fn)
	if(vit != vit_end && dn < fn)
		fn = RaywardSmithf(++r, fn, v, vit, vit_end);
		
	return fn;
}

/* -------------------------------------------------------------------------
	
		RunFloydWarshall(): Runs Floyd-Warshall, getting fw_matrix and
			parent matrix. It also initializes dnt_matrix and vertTOtree_map
		
------------------------------------------------------------------------- */
void
AverageDistanceHeuristic::RunFloydWarshall()
{
	//cerr << "[ADH::RunFloydWarshall] Floyd-Warshall Matrix : " << endl;
	vsVertexMapConst g_vsmap = get(&VertexProp::vertexState, g);
	//vsVertexMap g_vsmap = get(&VertexProp::vertexState, g);

	clock_t start_time = ::clock();

	cerr << "." ;
	//floyd_warshall_all_pairs_shortest_paths2(g, fw_matrix, parent_matrix, weight_map(get(&EdgeProp::weight, g))); 
	floyd_warshall_apsp(g, fw_matrix, parent_matrix, inputT.V); 
	cerr << "." ;
	
	overhead_exec_time_ = overhead_exec_time_ + (::clock() - start_time); 	// execution time

	TreeStructure* pT_adh;
	map<int, map<int, double> >::iterator mit, mit_end;
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)  	// initialize dnt_matrix and vertTOtree_map
	{	
		if((g_vsmap[*vit] == GROUP_MEMBER))
		{
			// initialze TreeBundle_adh
			pT_adh = new TreeStructure;
			pT_adh->insertVertex(*vit, g);
			TreeBundle_adh.push_back(pT_adh);	 
		
			vertTOtree_map[*vit] = pT_adh;
		}
		else vertTOtree_map[*vit] = NULL;
	}
	cerr << "." ;

	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		for(VertexSet::iterator vit_in = inputT.V.begin(), vit_in_end = inputT.V.end(); vit_in != vit_in_end; ++vit_in)
		{	
			if(g_vsmap[*vit_in] == GROUP_MEMBER)
				(dnt_matrix[*vit]).insert( pair<double, TreeStructure*>(fw_matrix[*vit][*vit_in], vertTOtree_map[*vit_in]));
		}
	}
	cerr << "." ;

	/*map<int, double>::iterator mit_in, mit_in_end;
	for(mit = fw_matrix.begin(), mit_end = fw_matrix.end(); mit != mit_end; ++mit)
	{
		for(mit_in = (*mit).second.begin(), mit_in_end = (*mit).second.end(); mit_in != mit_in_end; ++mit_in)
		{	
			// initialize dnt_matrix -- according to distance 
			if((g_vsmap[(*mit_in).first] == GROUP_MEMBER))
				(dnt_matrix[(*mit).first]).insert( pair<double, TreeStructure*>((*mit_in).second, vertTOtree_map[(*mit_in).first]));
		}
	}
	cerr << "." ;

	dumpDNTMatrix();
	char c = getchar();*/

}

/* -------------------------------------------------------------------------
	
	
		Dump(): Below are dump functions printing trees and matrices


------------------------------------------------------------------------- */

void
AverageDistanceHeuristic::dumpParentMatrix()
{
	cerr << "[ADH::dumpParentMatrix] Floyd-Warshall Parent Matrix : " << endl;
	map<int, map<int, int> >::iterator mit;
	map<int, int>::iterator mit_in;
	for(mit = parent_matrix.begin(); mit != parent_matrix.end(); ++mit)
	{
		cerr << (*mit).first << " : ";
		for(mit_in = (*mit).second.begin(); mit_in != (*mit).second.end(); ++mit_in)
			cerr << " (id:" << (*mit_in).first << " p:" << (*mit_in).second << ")";
		cerr << endl;
	}			
}


void
AverageDistanceHeuristic::dumpFWMatrix()
{
	cerr << "[ADH::dumpFWMatrix] Floyd-Warshall Matrix : " << endl;
	map<int, map<int, double> >::iterator mit;
	map<int, double>::iterator mit_in;
	for(mit = fw_matrix.begin(); mit != fw_matrix.end(); ++mit)
	{
		cerr << (*mit).first << " Distances: ";
		for(mit_in = (*mit).second.begin(); mit_in != (*mit).second.end(); ++mit_in)
			cerr << " (id:" << (*mit_in).first << " d:" << (*mit_in).second << ")";
		cerr << endl;	
	}
}

void
AverageDistanceHeuristic::dumpDNTMatrix()
{
	cerr << "[ADH::dumpDNTMatrix] d(n,t) Matrix : " << endl;
	map<int, multimap<double, TreeStructure*> >::iterator mmit;
	multimap<double, TreeStructure*>::iterator mmit_in;
	for(mmit = dnt_matrix.begin(); mmit != dnt_matrix.end(); ++mmit)
	{
		cerr << (*mmit).first << " Distances: ";
		for(mmit_in = (*mmit).second.begin(); mmit_in != (*mmit).second.end(); ++mmit_in)
			cerr << " (" << *(mmit_in->second) << " d:" << (*mmit_in).first << ")";
		cerr << endl;	
	} 
}

void
AverageDistanceHeuristic::dumpVertTOtreeMap()
{
	cerr << "[ADH:cumpVertTOtreeMap] : " << endl;
	map<int, TreeStructure*>::iterator mit;
	for(mit = vertTOtree_map.begin(); mit != vertTOtree_map.end(); ++mit)
	{
		cerr << "Id: " << mit->first << " Tree: "; 
		if(mit->second)
			dumpSteinerTree(*(mit->second));
		cerr << endl;
	}
}

void
AverageDistanceHeuristic::dumpAll()
{
	dumpFWMatrix(); 
	dumpDNTMatrix(); 
	dumpParentMatrix(); 
	dumpVertTOtreeMap();
	print_graph(g);
	dumpGraph(g);
}


