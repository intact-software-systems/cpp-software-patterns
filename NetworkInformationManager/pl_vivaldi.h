/***************************************************************************
                          pl_vivaldi.h  -  description
                             -------------------
    begin                : Thu May 10 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef PLANETLAB_VIVALDI_KHV
#define PLANETLAB_VIVALDI_KHV

#include <iostream>
#include <sys/utsname.h>
#include "./os/MNPCList.h"
#include "../simulator/network/message.h"
#include "../simulator/network/vertex_set.h"
#include "../simulator/boostprop.h"
#include "../simulator/treealgs/treestructure.h"
#include "socket_info.h"
#include "system_monitor.h"
#include "./vivaldi/vivaldi.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

#define DIMENSION 4
namespace VivaldiSpace
{

int getDim();

enum VivaldiType { 
		VIVALDI_TYPE_ERROR = 0,
		
		NO_TYPE, 
		NEIGHBOR_UNREACHABLE,
		NEIGHBOR, 
		FROM_SERVER, 
		TO_SERVER 
};

};

using namespace VivaldiSpace;

std::ostream& operator<<(std::ostream& ostr, const VivaldiSpace::VivaldiType& type);

/*-----------------------------------------------------------------------
					vivaldi threads and wrapper
------------------------------------------------------------------------- */
void* plvvclient(void* arg);
void* plvvserver(void* arg);

struct VVWrapper
{	
	VVWrapper() : node_id(-1), pipe_mc(0), pipe_vv(0), cs_mutex(), condition(), global_mesh() {}
	~VVWrapper() {}

	int node_id;
	int pipe_mc;
	int pipe_vv;
	pthread_mutex_t *cs_mutex;
	pthread_cond_t *condition;
	TreeStructure *global_mesh;
};

typedef boost::shared_ptr<VVWrapper> VVWrapperPtr;
class VivaldiHeader;
/*-----------------------------------------------------------------------
					vivaldi info passed to threads
------------------------------------------------------------------------- */
struct VivaldiInfo
{
public:
		
	VivaldiInfo() : eid(-1,-1), msg(NO_MESSAGE_TYPE), type(NO_TYPE) 
	{ 
		in.err = 0.0; 
		in.rtt = 0.0;
	}		
	VivaldiInfo(const VivaldiInfo &vv) : eid(vv.getEid()), msg(vv.getMsg()), type(vv.getType()) 
	{ 
		setCoords(vv.getCoords()); 
		setErr(vv.getErr()); 
		setRtt(vv.getRtt());  
		setBuf(vv.getBuf());
		setId(vv.getId());
	}
	VivaldiInfo(const VVNodeData &vn) : eid(-1,-1), msg(NO_MESSAGE_TYPE), type(NO_TYPE) 
	{
		setCoords(vn.coords);
		setErr(vn.err);
		setRtt(vn.rtt);
		setId(vn.id);
	}
	VivaldiInfo(VivaldiHeader* vh, MessageType m);
	VivaldiInfo(VivaldiHeader* vh);

	~VivaldiInfo() {}

	inline const vector<double> getCoords() 		const { return in.coords; }
	inline double getErr() 							const { return in.err; }
	inline double getRtt() 							const { return in.rtt; }
	inline int getId()								const { return in.id; }
	inline EdgeId getEid() 							const { return eid; }
	inline MessageType getMsg()						const { return msg; }
	inline VivaldiType getType()					const { return type; }
	inline char* getBuf()							const { return buf.get(); }
	inline VVNodeData getNodeData()					const { return in; }
	
	inline void setCoords(const vector<double> c) 	{ in.coords = c; }
	inline void setErr(const double &err) 			{ in.err = err; }
	inline void setRtt(const double &rtt) 			{ in.rtt = rtt; }
	inline void setId(const int &i)					{ in.id = i; }
	inline void setEid(int u, int v)				{ eid = EdgeId(u,v); }
	inline void setEid(EdgeId id)					{ eid = id; }
	inline void setMsg(MessageType msg)				{ msg = msg; }
	inline void setType(VivaldiType t)				{ type = t; }
	inline void setBuf(const CharPtr &b)			{ buf.set(b.get()); }
	inline void setBuf(const char *b)				{ buf.set(b); }
	inline void setNodeData(const VVNodeData &n)	{ in = n; }

public:
	
	VVNodeData		in;
	EdgeId 			eid;
	MessageType 	msg;
	VivaldiType		type;
	CharPtr 		buf;
};

typedef boost::shared_ptr<VivaldiInfo> 	VivaldiInfoPtr;
typedef MNPCList<VivaldiInfoPtr> 		MuxListVivaldiInfo;
std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords);
std::ostream& operator<<(std::ostream& ostr, const VivaldiInfo &vv);
std::ostream& operator<<(std::ostream& ostr, const double *coords);

/*-----------------------------------------------------------------------
			vivaldi info piggybacked in multicast messages
------------------------------------------------------------------------- */
struct VivaldiHeader
{
public:
	VivaldiHeader() { in.err = 0.0; in.rtt = 0.0; in.eid = EdgeId(-1,-1); in.type = NO_TYPE; }
	VivaldiHeader(const VivaldiInfo &inf) 
	{ 
		setCoords(inf.getCoords()); 
		setErr(inf.getErr()); 
		setRtt(inf.getRtt()); 
		setEid(inf.getEid()); 
		setType(inf.getType());
	}
	~VivaldiHeader() {}
	
	inline const double* getCoords() 				const { return in.coords; }
	inline double getErr() 							const { return in.err; }
	inline double getRtt() 							const { return in.rtt; }
	inline EdgeId getEid() 							const { return in.eid; }
	inline VivaldiType getType()					const { return in.type; }

	inline void setCoords(const double *c, int sz) { 
			for(int i = 0; i < sz; i++)
				in.coords[i] = c[i]; 
	}
	inline void setCoords(const vector<double> &c) { 
			for(int i = 0; i < c.size(); i++)
				in.coords[i] = c[i]; 
	}
	inline void setErr(const double &err) 			{ in.err = err; }
	inline void setRtt(const double &rtt) 			{ in.rtt = rtt; }
	inline void setEid(int u, int v)				{ in.eid = EdgeId(u,v); }
	inline void setEid(EdgeId id)					{ in.eid = id; }
	inline void setType(VivaldiType t)				{ in.type = t; }

	struct Data {
		double 		coords[DIMENSION];
		double 		err;
		double 		rtt;
		EdgeId 		eid;
		VivaldiType type;
	};

public:	
	Data in;
	CharPtr buf;
};

typedef boost::shared_ptr<VivaldiHeader> 	VivaldiHeaderPtr;
typedef MNPCList<VivaldiHeaderPtr> 			MuxListVivaldiHeader;
std::ostream& operator<<(std::ostream& ostr, const VivaldiHeader &vv);

/*-----------------------------------------------------------------------
						class PLVivaldiServer
------------------------------------------------------------------------- */
void* plvvserver_groups(void* arg);
void* plvvserver_rcv(void* arg);
void* plvvserver_log(void* arg);

class PLVivaldiServer
{
public:
	PLVivaldiServer(VVWrapper *vvw);
  	~PLVivaldiServer() {}
	
	void server();
	void groups();
	void logNodeCoords();

public:
	typedef map<int, VVNodeData> MapNodeCoords;
	MapNodeCoords mapNodeCoords;
	
	boost::numeric::ublas::matrix<double> distMatrix; 
	
	// thread id
	pthread_t thread_gr, thread_rcv, thread_log;
	void start();
	void join();
	
private:
	void logNodeCoords(int seconds);
	void checkValidity(VivaldiInfoPtr vip);
	void updateMesh(const VVEdgeData &ed); 
	void createGroups(VertexSet &newMembers);
	void createRandomGroups(VertexSet &newMembers, int sz);
	void writeStats(const TreeStructure &g, const double &seconds, ostream &ostr);

	// inline
	inline void lock() {  pthread_mutex_lock(&cs_mutex);}
	inline void unlock()  {  pthread_mutex_unlock(&cs_mutex); }
	inline void lock_server() { pthread_mutex_lock(&cs_server_mutex); }
	inline void unlock_server() { pthread_mutex_unlock(&cs_server_mutex);}
	
	inline void lock_all() 
	{ 
		pthread_mutex_lock(&cs_mutex);
		pthread_mutex_lock(&cs_server_mutex);
	}
	inline void unlock_all() 
	{ 
		pthread_mutex_unlock(&cs_server_mutex);
		pthread_mutex_unlock(&cs_mutex);
	}

	inline void signal(){ pthread_cond_broadcast(&mesh_updated); }
	inline void wait() { pthread_cond_wait(&mesh_updated, &cs_server_mutex); }
	

private:
	VivaldiNode vvNode_;
	
private:
	int node_id_;
	int groupSizeVV_;

	// pipes
	int pipe_mc_;
	int pipe_vv_;

	// pthread mutex
	pthread_mutex_t cs_mutex;
	pthread_mutex_t &cs_server_mutex;
	pthread_cond_t 	&mesh_updated;

	TreeStructure  		*global_mesh_;  		// shared copy with server thread
	GraphN 				vivaldiGraph_;
	VertexSet			prev_mesh_nodes_;
	map<int, VertexSet> vivaldiGroups_;
	VertexSet			appMembers_;
};

/*-----------------------------------------------------------------------
						class PLVivaldiClient
------------------------------------------------------------------------- */
void* plvvclient_rcv(void* arg);
void* plvvclient_gen(void* arg);
void* plvvclient_snd(void* arg);
void* plvvpinger(void* arg);

class PLVivaldiClient
{
public:
	PLVivaldiClient(VVWrapper *vvw);
  	~PLVivaldiClient() {}
	
	void client();
	void generator();
	void sender();
	void start();
	void pinger();
	
private:

	void logNodeCoordsToServer();

	// inline
	inline void lock() 			{ pthread_mutex_lock(&cs_mutex); }
	inline void unlock()  		{ pthread_mutex_unlock(&cs_mutex); }
	inline void signal() 		{ pthread_cond_broadcast(&vvinfo_updated); }
	inline void wait()			{ pthread_cond_wait(&vvinfo_updated, &cs_mutex); }
	inline void locked_wait()	{ lock(); pthread_cond_wait(&vvinfo_updated, &cs_mutex); }
	
	inline void locked_timed_wait(long int sec)
	{
		struct timespec ts;
		struct timeval tp;
		gettimeofday(&tp, NULL);
		
		ts.tv_sec  = tp.tv_sec;
		ts.tv_nsec = tp.tv_usec * 1000;
		ts.tv_sec += (long int)sec;
		
		lock();
		pthread_cond_timedwait(&vvinfo_updated, &cs_mutex, &ts);
	}

public:
	//thread ids
	pthread_t thread_gr, thread_snd, thread_png;

	bool requestNodeAddress(int remote);
	VertexSet requestedInfo;
	bool getSocketInfo(int remote, SocketInfoPtr &sip);
	double getRoundTripTime(int remote);

	map<int, double> mapNodeRTT;
	VertexSet		 commNodes;

private:
	VivaldiNode vvNode_;
	
private:
	int node_id_;

	// pipes
	int pipe_mc_;
	int pipe_vv_;
		
	// pthread mutex
	pthread_mutex_t cs_mutex;
	pthread_mutex_t &cs_client_mutex;
	pthread_cond_t 	vvinfo_updated;
	
	VertexSet 		neighborHood_;
	NodeSocketMap	nsmVivaldiGroup_;

	// shared data
	VVNodeData		myNodeData_;
	bool 			sentToServer_;
	bool			recvFromServer_;
};

namespace VivaldiSpace
{
bool debug(SystemSpace::ThreadID id, VivaldiInfoPtr, double&);
bool endsafe(SystemSpace::ThreadID id);

};

/*-----------------------------------------------------------------------
				mutex Channel class
------------------------------------------------------------------------- */
/*class VivaldiChannel
{
public: 
	VivaldiChannel() {}
	~VivaldiChannel() {}

	static MuxListVivaldiInfo	muxLVivaldiServer;
	static MuxListVivaldiInfo	muxLVivaldiClient;
	static MuxListVivaldiInfo	muxLVivaldiMulticast;
};*/

#endif // PLANETLAB_VIVALDI_KHV

