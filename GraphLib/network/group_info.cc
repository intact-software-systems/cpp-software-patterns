/***************************************************************************
                          group_info.cc  -  description
                             -------------------
    begin                : Wed Feb 22 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "group_info.h"
#include "../boostprop.h"
#include "../treealgs/treealgs.h"
#include <fstream>
#include "../simtime.h"

using namespace TreeAlgorithms;
using namespace std;
using namespace boost;

namespace GraphLib
{

void GroupInformation::updateEnterRequest(double time, int newMember)
{
	if(groupMembers_.contains(newMember))
	{
		cerr << WRITE_FUNCTION << "t:" << time << " gid: " << getGroupId() << " newMember: " << newMember << " already in group: " << groupMembers_ << endl;
		//cerr << " Tree : " << treeStructure_ << endl;
	}

	ASSERTING(!groupMembers_.contains(newMember));
	
	setNewTimeStamp(time); 			
	groupMembers_.setNewTS(time); 	

	groupMembers_.insert(newMember); 
	if(steinerPoints_.count(newMember)) 
		steinerPoints_.erase(newMember);

	ASSERTING(groupMembers_.contains(newMember));
}

void GroupInformation::updateLeaveRequest(double time, int oldMember)
{
	if(!groupMembers_.contains(oldMember))
	{
		cerr << WRITE_FUNCTION << "t:" << time << " gid: " << getGroupId() << " oldMember: " << oldMember << " not in group: " << groupMembers_ << endl;
		//cerr << " Tree : " << treeStructure_ << endl;
	}
	
	//if(time > 0) ASSERTING(groupMembers_.contains(oldMember));
			
	groupMembers_.setTS(time); 	
	setTimeStamp(time); 		

	groupMembers_.erase(oldMember);

	ASSERTING(!groupMembers_.contains(oldMember));
}

void GroupInformation::updateGroupInfo(const TreeStructure &newT)
{
	treeStructure_ = newT;
	steinerPoints_ = newT.S;
}

void GroupInformation::updateGroupInfo()
{
	steinerPoints_ = treeStructure_.S;
}

void GroupInformation::updateSteinerSet()
{
	steinerPoints_ = treeStructure_.S;
}

void GroupInformation::insertServer(int server)
{
	ASSERTING(includedServer_ == false);
	includedServer_ = true;
	groupMembers_.insert(server); 
}

void GroupInformation::removeServer(int server)
{
	if(includedServer_) groupMembers_.erase(server);

	includedServer_ = false;
}

void GroupInformation::removeFromTree(int node)
{
	treeStructure_.removeVertex(node);
}

bool GroupInformation::setReconfigSetInsert(int from, VertexSet &V)
{
	groupReconfigSetInsert_.insert(from);
	V = groupReconfigSetInsert_;

	cerr << WRITE_FUNCTION << " id: " << from << " group : " << getGroupId() << " insert reconfig set: " <<  V << endl;

	return true;
}

bool GroupInformation::findReconfigSetInsert(int from)
{
	return true;
}

bool GroupInformation::remReconfigSetInsert(int from)
{
	// -- debug --
	cerr << WRITE_FUNCTION << " removing id: " << from << " group : " << getGroupId() << " from insert reconfig set: " << groupReconfigSetInsert_ << endl;
	ASSERTING(groupReconfigSetInsert_.contains(from));
	// -- debug end --
	
	groupReconfigSetInsert_.erase(from);

	return true;
}

bool GroupInformation::setReconfigSetRemove(int from, VertexSet &V, int sequence_number)
{
	bool doReconfig = findReconfigSetRemove(from, V);
	if(doReconfig)
	{
		cerr << WRITE_FUNCTION << " id: " << from << " group : " << getGroupId() << " remove reconfig set: " <<  V << endl;
		ASSERTING(!mapSeqReconfigSet_.count(sequence_number));
		mapSeqReconfigSet_.insert(pair<int, VertexSet>(sequence_number, V));
	}
	else
		cerr << WRITE_FUNCTION << " ERROR! id: " << from << " group : " << getGroupId() << " remove reconfig set: " <<  V << endl;
	
	return doReconfig;
}

bool GroupInformation::findReconfigSetRemove(int from, VertexSet &V)
{
	V.insert(from);

	if(treeStructure_.V.contains(from)) 		// identify reconfig set, neighbors of from
	{	
		const GraphN &g = treeStructure_.g;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(from, g); oit != oit_end; ++oit)
		{
			int targ = target(*oit, g);
			V.insert(targ);		
		}
	}

	bool doReconfig = true;
	int group_size = getMembers().size();
	
	// if there exist a reconfigset 
	for(MapIntReconfigSet::iterator it = mapSeqReconfigSet_.begin(), it_end = mapSeqReconfigSet_.end(); it != it_end; ++it)
	{
		const VertexSet& R = it->second; 
		VertexSet diffSet = (V - R) + (R - V);
		VertexSet interSet = (V + R) - ((V - R) + (R - V));
		
		//cerr << " new set : " << V << " current reconfig set " << R << " intersection set " << interSet << endl;
		if(interSet.size() > 1)
		{
			doReconfig = false;		
			break;
		}
		else if(interSet.size() == V.size()) // == 1 or 2
		{
			doReconfig = false;
			break;
		}
		else if(group_size < 3 && interSet.size() > 0)
		{
			doReconfig = false;
			break;
		}
		
		if(!doReconfig) break;
	}

	//if(doReconfig)
	//	cerr << WRITE_PRETTY_FUNCTION << " id: " << from << " group : " << getGroupId() << " reconfig set: " <<  V << endl;

	return doReconfig;
}

bool GroupInformation::remReconfigSet(int sequence_number)
{
	int ret = mapSeqReconfigSet_.count(sequence_number);
	ASSERTING(mapSeqReconfigSet_.count(sequence_number));
	if(ret <= 0) return false;

	mapSeqReconfigSet_.erase(sequence_number);

	return true;
}

/*-----------------------------------------------------------------------
	GroupInformation-
			check/debug group information
------------------------------------------------------------------------- */
void GroupInformation::checkAll()
{
#ifndef NDEBUG
	if(treeStructure_.isValid() == false) return;

	if(treeStructure_.V.size() > 2)
	{	
		VertexSet::iterator vit = groupMembers_.begin(), vit_end = groupMembers_.end();
		for( ; vit != vit_end; ++vit)
		{
			if(((treeStructure_.g)[*vit].vertexState != GROUP_MEMBER) || (steinerPoints_.contains(*vit)))
			{
				cerr << getGroupId() << "GroupMember Error: " << *vit << endl;
				//cerr << "GroupMembers : " << groupMembers_ << endl;
				//treeStructure_.printVertexState(cerr);
				dumpAll(cerr);
			}
			
			ASSERTING(!steinerPoints_.contains(*vit));
			ASSERTING((treeStructure_.g)[*vit].vertexState == GROUP_MEMBER);
		}
	
		vit = steinerPoints_.begin(), vit_end = steinerPoints_.end();
		for( ; vit != vit_end; ++vit)
		{
			if(((treeStructure_.g)[*vit].vertexState != STEINER_POINT) || (groupMembers_.contains(*vit)))
			{
				cerr << getGroupId() << "SteinerPoint Error: " << *vit << endl;
				//cerr << "SteinerPoints : " << steinerPoints_ << endl;
				//treeStructure_.printVertexState(cerr);
				dumpAll(cerr);
			}
			ASSERTING(!groupMembers_.contains(*vit));	
			ASSERTING((treeStructure_.g)[*vit].vertexState == STEINER_POINT);
		}
	}

	VertexSet intersection = groupMembers_ - steinerPoints_;
	if(intersection.size() != groupMembers_.size() ||	intersection != groupMembers_)
		dumpAll(cerr);
	if(treeStructure_.V.size() - treeStructure_.S.size() != getMembers().size()) dumpAll(cerr);

	ASSERTING(treeStructure_.V.size() - treeStructure_.S.size() == getMembers().size());
	ASSERTING(treeStructure_.S.size() == steinerPoints_.size());
	ASSERTING(intersection.size() == groupMembers_.size());
	ASSERTING(intersection == groupMembers_);	
#endif // NDEBUG	
}
void GroupInformation::checkGroupInfo()
{
#ifndef NDEBUG
	VertexSet::iterator vit = groupMembers_.begin(), vit_end = groupMembers_.end();
	for( ; vit != vit_end; ++vit)
	{
		ASSERTING((treeStructure_.g)[*vit].vertexState == GROUP_MEMBER);
	}
#endif // NDEBUG
}
void GroupInformation::checkSteinerInfo()
{
#ifndef NDEBUG	
	VertexSet::iterator vit = steinerPoints_.begin(), vit_end = steinerPoints_.end();
	for( ; vit != vit_end; ++vit)
	{
		ASSERTING((treeStructure_.g)[*vit].vertexState == STEINER_POINT);
	}
#endif // NDEBUG
}


void GroupInformation::dumpAll()
{
	cerr << getGroupId() << "GroupMembers : " << groupMembers_ << endl;
	cerr << "SteinerPoints : " << steinerPoints_ << endl;
	treeStructure_.printVertexState(cerr);
}

void GroupInformation::dumpAll(ostream& ostr)
{
	ostr << getGroupId() << "GroupMembers : " << groupMembers_ << endl;
	ostr << "SteinerPoints : " << steinerPoints_ << endl;
	ostr << "Treestructure vertex state : " << endl;
	treeStructure_.printVertexState(cerr);
	ostr << "tree structure : " << endl;
	treeStructure_.print(ostr);
	treeStructure_.printGraph(ostr);
}
/*
bool GroupInformation::setReconfigSetInsert(int from, VertexSet &V)
{
	int group = getGroupId();
	if(mapGroupReconfigSetInsert_.count(group))
	{
		VertexSet &R = mapGroupReconfigSetInsert_[group];
		R.insert(from);
		V = R;
	}
	else
	{
		mapGroupReconfigSetInsert_.insert(pair<int, VertexSet>(group, VertexSet(from)));
		V.insert(from);
	}

	cerr << WRITE_FUNCTION << " id: " << from << " group : " << getGroupId() << " insert reconfig set: " <<  V << endl;

	return true;
}

bool GroupInformation::remReconfigSetInsert(int from)
{
	int group = getGroupId();

	// -- debug --
	ASSERTING(mapGroupReconfigSetInsert_.count(group));
	int ret = mapGroupReconfigSetInsert_.count(group);
	if(ret <= 0) return false;
	// -- debug end--
	
	VertexSet &R = mapGroupReconfigSetInsert_[group];
	
	// -- debug --
	ASSERTING(R.contains(from));
	cerr << WRITE_FUNCTION << " removing id: " << from << " group : " << getGroupId() << " from insert reconfig set: " << R << endl;
	// -- debug end --
	
	R.erase(from);

	return true;
}

*/
}; // namespace GraphLib

