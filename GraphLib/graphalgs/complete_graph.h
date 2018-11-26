/***************************************************************************
                          complete_graph.h  -  description
                             -------------------
    begin                : Wed Mar 08 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_COMPLETE_GRAPH_KHV
#define GRAPHALGO_COMPLETE_GRAPH_KHV

#include "../boostprop.h"
#include "../network/vertex_set.h"
#include "../network/group_info.h"
#include "../treealgs/treealgs.h"

using namespace TreeAlgorithms;

class SteinerPoints;

/*-----------------------------------------------------------------------
		class CompleteGraph: Input is a complete graph, produces a complete
			graph, complete member graph, complete member graph with k
			steiner points. 
----------------------------------------------------------------------- */
class CompleteGraph
{
public:
	CompleteGraph(GraphLib::GroupInformation& groupInfo, int newV) 
				:     groupMembers_(groupInfo.getMembers())
					, steinerPoints_(groupInfo.getSteinerPoints()) 
				    , groupInfo_(groupInfo) 
					, newMember_(newV) {}
	CompleteGraph(VertexSet &groupMembers, VertexSet& steinerPoints, GraphLib::GroupInformation& groupInfo, int newV) 
				 :    groupMembers_(groupMembers)
			   		, steinerPoints_(steinerPoints) 
				    , groupInfo_(groupInfo)	
					, newMember_(newV) {}
	~CompleteGraph() {}

	void compGraph(TreeStructure &inputT, const GraphN &global_g, SteinerPoints& fifoSteiner, bool);

	// create complete graphs	
	void createCGraph(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers);
	void createCMGraph(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers);
	void createCMGraphSteinerSet(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, VertexSet &steinerSet); 

	// update complete graph from GroupInformation
	void updateCTree(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, int new_v);
	void updateCMTree(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, int new_v);
	void updateCMGraphSteinerSet(TreeStructure &inputT, const GraphN &g, const VertexSet &groupMembers, VertexSet &steinerSet); 

private:
	const VertexSet			&groupMembers_;
	const VertexSet			&steinerPoints_;
	const GraphLib::GroupInformation 	&groupInfo_;
	int	newMember_;
};

#endif // GRAPHALGO_COMPLETE_GRAPH_KHV

