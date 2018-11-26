/***************************************************************************
                          group_info.h  -  description
                             -------------------
    begin                : Wed Feb 22 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_GROUP_INFO_KHV
#define GRAPHALGO_GROUP_INFO_KHV

#include <string.h>
#include <errno.h>

#include "../treealgs/treestructure.h"
#include "../network/vertex_set.h"

using namespace std;
using namespace TreeAlgorithms;

class ServerBase;
class TreeDynamics;

namespace GraphLib
{

/*-----------------------------------------------------------------------
	GroupInformation-
				Server side group information
------------------------------------------------------------------------- */
class GroupInformation
{
public:
	GroupInformation(int id) : includedServer_(false), groupId_(id), leader_(-1), timeStamp_(0), numOfConfigurations_(0), centerVertex_(-1)
	{
        //if(pthread_mutex_init(&cs_mutex, NULL) == -1)
        //	cerr << "ERROR!! groupInfo pthread mutex init error " << strerror(errno) << endl;
        //if(pthread_mutex_init(&global_mutex, NULL) == -1)
        //	cerr << "ERROR!! groupInfo pthread mutex init error " << strerror(errno) << endl;
	}
	~GroupInformation() {}

	friend class ServerBase;
	friend class TreeDynamics;

	// class functions
	void updateEnterRequest(double time, int newMember);
	void updateLeaveRequest(double time, int newMember);
	void updateGroupInfo(const TreeStructure& T);
	void updateGroupInfo();

	void updateSteinerSet();
	//void updateVertexMap();
	void insertServer(int server);
	void removeServer(int server);
	
	// debug/dump functions
	void dumpAll();
	void dumpAll(ostream&);
	void checkAll();
	void checkSteinerInfo();
	void checkGroupInfo();

public:
	// inline functions
	inline int	 				getLeader() 	const { return leader_;}
	inline int					getCenterVertex() const { return centerVertex_; }
	inline int					getFirst() 		const { return *(groupMembers_.begin()); }
	inline double 				getTimeStamp() 	const { return timeStamp_; }	
	inline int					getGroupId() 	const { return groupId_; }
	
	inline const VertexSetTS& 	getMembers() 		const { return groupMembers_;}
	inline const TreeStructure&	getTreeStructure() const { return treeStructure_; }
	inline const TreeStructure&	getPrevTreeStructure() const { return prevTreeStructure_; }
	inline const VertexSet&		getSteinerPoints() const { return steinerPoints_; }
	inline const VertexSet&		getNotifySet()		const { return notifySet_; }

	inline TreeStructure&    	getCMTree() { return cmTree_; }
	
	inline void				setNotifySet(const VertexSet &V)	{ notifySet_ = V; }
	inline void 			setLeader(int l) { leader_ = l;}
	inline void				setTimeStamp(double ts) { timeStamp_ = ts;}
  	inline void 			setNewTimeStamp( double ts ) { if( ts > timeStamp_ ) timeStamp_ = ts;}
	inline void				setTreeStructure(const TreeStructure &ts) { treeStructure_ = ts; }
	inline void				setPrevTreeStructure(const TreeStructure &ts) { prevTreeStructure_ = ts; }
	inline void				setLastValidTree(const TreeStructure &T)	{ lastValidTree_ = T; }
	inline void				setCenterVertex(vertex_descriptorN v) { centerVertex_ = v; }
	inline void				setTrees(const TreeStructure &ts) { 
								prevTreeStructure_ = treeStructure_;
								treeStructure_ = ts; 
							}

	inline void				setCMTree(const TreeStructure &T) 	{ cmTree_ = T; }
	inline bool				includedServer()	const { return includedServer_; }

	
public:
	inline void				incDynamicsCounter() 				{ numOfConfigurations_++; }
	inline void				resetDynamicsCounter() 				{ numOfConfigurations_ = 0; }
	inline int 				getDynamicsCounter() const			{ return numOfConfigurations_; }
	
	// functions
	bool				setReconfigSetInsert(int node, VertexSet &V);
	bool				findReconfigSetInsert(int node);
	bool				remReconfigSetInsert(int node);
	
	bool				setReconfigSetRemove(int node, VertexSet &V, int sequence_number);
	bool				findReconfigSetRemove(int node, VertexSet &V);
	bool				remReconfigSet(int sequence);

	void 				removeFromTree(int node);
	
public:
	// TODO: Trouble with constructor in treealgs/tree_dynamics.h
	TreeStructure 		treeStructure_;

protected:
	// proxy/server
	bool 				includedServer_;
	
	// class variables
	VertexSetTS	 		groupMembers_;		// simple vertex_descriptor set
	VertexSet			steinerPoints_;		// simple vertex_descriptor set
	VertexSet			notifySet_;  		// for each leave/join this set contains the nodes that are
											// affected by the reconfiguration	
	//TreeStructure 		treeStructure_;
	TreeStructure		prevTreeStructure_;
	TreeStructure		lastValidTree_;
	
	TreeStructure		cmTree_;
	
	int					groupId_;
	int      			leader_;
	double				timeStamp_;			// last group update

protected:
	//typedef map<int, vector<VertexSet> > MapReconfigSet;
	//MapReconfigSet		reconfigSets_;		// insert/remove algorithms reconfigures smaller parts of existing tree
											// this map stores the current reconfig sets that are being updated
	
	typedef map<int, VertexSet> MapIntReconfigSet;
	MapIntReconfigSet mapSeqReconfigSet_;
	//MapIntReconfigSet mapGroupReconfigSetInsert_;
	VertexSet			groupReconfigSetInsert_;

	// dynamics
	int					numOfConfigurations_;
	int					centerVertex_;

public:
	// do not use inside this class
    //inline void 		global_lock() 	{ pthread_mutex_lock(&global_mutex); 	}
    //inline void 		global_unlock()	{ pthread_mutex_unlock(&global_mutex); 	}

	// used inside this class
    //inline void 		lock() 			{ } //pthread_mutex_lock(&cs_mutex); }
    //inline void 		unlock()		{ } //pthread_mutex_unlock(&cs_mutex); }

//private:
//	pthread_mutex_t				cs_mutex, global_mutex;
};

}; // namespace GraphLib

#endif // GRAPHALGO_GROUP_INFO_KHV

