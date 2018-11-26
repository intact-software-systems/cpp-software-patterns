/***************************************************************************
                          steiner_set.h  -  description
                             -------------------
    begin                : Thu Mar 09 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_STEINER_SET_KHV
#define GRAPHALGO_STEINER_SET_KHV

#include "../boostprop.h"
#include "../network/vertex_set.h"
#include "../simdefs.h"
#include <map>
#include <list>

using namespace std;
using namespace boost;

/*-----------------------------------------------------------------------
						class SteinerPoints
------------------------------------------------------------------------- */
class SteinerPoints
{
public:
	SteinerPoints() { }
	~SteinerPoints() { }
	
	typedef std::list<int>	SteinerQueue;
	
	// -- debug --
	static void checkQueue();
	
public:
	inline static size_t size() { return steinerQueue_.size(); }
	inline static void insertSet(const VertexSet &vertSet) 
	{
		steinerSet_ = vertSet;

		VertexSet::const_iterator vi, vi_end;
		for(vi = vertSet.begin(), vi_end = vertSet.end(); vi != vi_end; ++vi)
		{
			steinerQueue_.push_back(*vi);
		}
		
		size_ = steinerQueue_.size();
	}
	
public:
	static SteinerQueue	steinerQueue_;
	static VertexSet steinerSet_;
	
	static size_t size_;
};

/*-----------------------------------------------------------------------
					core selection heuristics
------------------------------------------------------------------------- */
namespace GraphAlgorithms
{
	// start function
	//void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &steinerSet, const VertexSet &intree, VertexSet &newWCN, int k);
	void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &steinerSet, const VertexSet &relativeV, const VertexSet &intree, VertexSet &newWCN, int k, CoreSelectionAlgo algo);
	//void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &excludeSet, VertexSet &newWCN, int k, CoreSelectionAlgo algo);

	// -- WCN selection functions --
	void findkNodesNotInSet(SteinerPoints::SteinerQueue &steinerQueue, const VertexSet &V,  VertexSet &newV, int k);
	void findkNodesNotInSets(SteinerPoints::SteinerQueue &steinerQueue, const VertexSet &totalV, const VertexSet &V, VertexSet &newV, int k);
	
	void findkBestLocatedNodesNotInSet_Average(const GraphN &global_g, const VertexSet &steinerSet, const VertexSet &V, VertexSet &newV, int k);
	void findkBestLocatedNodesNotInSets_Average(const GraphN &global_g, const VertexSet &steinerSet, const VertexSet &totalV, const VertexSet &V, VertexSet &newV, int k);

	void kCenterNodes(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k);
	//void kCenterNodes(const GraphN &g, const VertexSet &V, const VertexSet &excludeV, VertexSet &coreSet, int k);
	
	void kMedianNodes(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k);

	void findkBestLocatedNodesNotInSet_Median(const GraphN &global_g, const VertexSet &steinerSet, const VertexSet &V, VertexSet &newV, int k);
	void findkBestLocatedNodesNotInSets_Median(const GraphN &global_g, const VertexSet &steinerSet, const VertexSet &totalV, const VertexSet &V, VertexSet &newV, int k);
	
	void findkTailedSpreadNotInSet(const GraphN &global_g, const GraphN &group_g, const VertexSet &steinerSet, const VertexSet &V, VertexSet &vSet, int k);
	void findkTailedSpreadNotInSets(const GraphN &global_g, const GraphN &group_g, const VertexSet &steinerSet, const VertexSet &totalV, const VertexSet &V, VertexSet &vSet, int k);
	
	void findkTailedSpreadNotInSets(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet &V, VertexSet &vSet, int k, CoreSelectionAlgo algo);
	void random_broadcast_walk(const GraphN &group_g, const VertexSet &traverseV, const int &startv, const int &region, const VertexSet &steinerV, VertexSet &markedV);


	void findkNodesTailedSpreadNotInSet(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &V, VertexSet &vertexSet, int k);
	//void findkNodesTailedSpreadNotInSets(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet &V, VertexSet &vertexSet, int k);

	void findkNodesBandwidthNotInSet(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet& V, VertexSet &vertexSet, int k);
	void findkNodesBandwidthNotInSets(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet& V, VertexSet &vertexSet, int k);

	void findkTailedSpreadNotInSet_snake(const GraphN &global_g, const GraphN &group_g, const VertexSet &searcSet, const VertexSet &V, VertexSet &vSet, int k);
	void findkTailedSpreadNotInSets_snake(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet &V, VertexSet &vSet, int k);
};

/*-----------------------------------------------------------------------
							UNUSED
------------------------------------------------------------------------- */
/*class SteinerSet {
public:
		SteinerSet() {}
		~SteinerSet() {}
		
		struct Info {
			Info() {}
			Info(int i, int j) : inGroupCounter(i), v(j) {}
			~Info() {}
			
			int inGroupCounter;
			int v;					
		};
		
		void 		addSteinerMap(int);
		void 		removeSteinerMap(int);
		bool 		getSteinerMapGroupCounter(int v);
		virtual int getSteinerMapPoint();

		typedef std::map<int, Info> SteinerMap;
protected:		
		SteinerMap	steinerMap_;
};

class RRSteiner
{
public:
	RRSteiner(const VertexSet& v) : nextVertex_(0) 
	{
		insertSet(v);
	}
	RRSteiner() {}
	~RRSteiner() {}
	
	inline int next() { 
		nextVertex_ = ((nextVertex_ + 1) % steinerQueue_.size());
		return steinerQueue_[nextVertex_];
	}
	inline void insert(int v) { steinerQueue_.push_back(v); }
	
	inline void insertSet(const VertexSet &vertSet) {
		VertexSet::const_iterator vi, vi_end;
		for(vi = vertSet.begin(), vi_end = vertSet.end(); vi != vi_end; ++vi)
			insert(*vi);
	}
	inline int size() {	return steinerQueue_.size();}
	
	typedef std::vector<int> SteinerQueue;
private:
	unsigned int nextVertex_;
	SteinerQueue steinerQueue_;
};
*/

#endif // GRAPHALGO_STEINER_SET_KHV

