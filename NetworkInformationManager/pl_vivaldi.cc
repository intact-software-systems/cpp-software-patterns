/***************************************************************************
                          pl_vivaldi.cc  -  description
                             -------------------
    begin                : Thu May 10 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <iostream>
#include <sys/utsname.h>
#include "pl_vivaldi.h"
#include "pl_interrupt.h"
#include "pl_debug.h"
#include "pl_message.h"
#include "pl_socket.h"
#include "pl_simdefs.h"
#include "../simulator/graphalgs/graphalgs.h"
#include "system_monitor.h"
#include "./vivaldi/vivaldi.h"
#include "./network/pl_server_base.h"
#include "./graphalgs/pl_graphalgs.h"
#include "./pl_functions.h"

#include <vector>

using namespace std;
using namespace boost;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
						vivaldi operators
------------------------------------------------------------------------- */
std::ostream& operator<<(std::ostream& ostr, const VivaldiInfo &vv)
{
	ostr << "eid " << vv.eid << " msg: " << vv.getMsg() <<  " type: " << vv.getType() << " coords: " << vv.getCoords() << " err: " << vv.getErr() << " rtt: " << vv.getRtt() ;
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const VivaldiHeader &vv)
{
	ostr << " eid: " << vv.getEid() << " type: " << vv.getType() << " coords: " << vv.getCoords() << " err: " << vv.getErr() << " rtt: " << vv.getRtt() ;
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords)
{
	ostr << "(" ;
	for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
	{
		ostr << *vit << "," ;
	}
	ostr << ")"; 
	
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const double *coords)
{
	ostr << "(" ;
	for(int i = 0; i < getDim(); i++)
	{
		ostr << coords[i] << "," ;
	}
	ostr << ")"; 
	
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const VivaldiSpace::VivaldiType& type)
{
	switch(type)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(VivaldiSpace::VIVALDI_TYPE_ERROR)
	CASE(VivaldiSpace::NO_TYPE)
	CASE(VivaldiSpace::NEIGHBOR_UNREACHABLE)
	CASE(VivaldiSpace::NEIGHBOR) 
	CASE(VivaldiSpace::FROM_SERVER)
	CASE(VivaldiSpace::TO_SERVER)
#undef CASE
	default:
		cerr << "Missing operator<< case for VivaldiType " << endl;
		ASSERTING(0);
		break;
	}
}

/*-----------------------------------------------------------------------
					vivaldi info constructor
------------------------------------------------------------------------- */
VivaldiInfo::VivaldiInfo(VivaldiHeader *vh) : msg(NO_MESSAGE_TYPE)
{
	in.coords = vector<double>(getDim());
	const double *coords = vh->getCoords();
	for(int i = 0; i < getDim(); i++)
		in.coords[i] = coords[i];
	
	in.err = vh->getErr(); 
	in.rtt = vh->getRtt();
	in.id = PlanetLab::getRemote(vh->getEid(), PlanetLabArgs::nodeId()); 
	eid = vh->getEid();
	type = vh->getType();
}

VivaldiInfo::VivaldiInfo(VivaldiHeader *vh, MessageType m) : msg(m)
{
	in.coords = vector<double>(getDim());
	const double *coords = vh->getCoords();
	for(int i = 0; i < getDim(); i++)
		in.coords[i] = coords[i];
	
	in.err = vh->getErr(); 
	in.rtt = vh->getRtt();
	in.id = PlanetLab::getRemote(vh->getEid(), PlanetLabArgs::nodeId()); 
	eid = vh->getEid();
	type = vh->getType();
}

/*-----------------------------------------------------------------------
						vivaldi client thread
------------------------------------------------------------------------- */
void* plvvclient(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi client thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT, ThreadInfo::RUN, &plvvclient, arg);

		PLVivaldiClient plv((VVWrapper*)arg); 
		plv.start();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT, KILL_PROGRAM);
	}

	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab VIVALDI_CLIENT vivaldi client thread " << endl;
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT, ThreadInfo::STOP);

	return NULL;
}

/*-----------------------------------------------------------------------
					PLVivaldiClient constructor
------------------------------------------------------------------------- */
PLVivaldiClient::PLVivaldiClient(VVWrapper *vvw) : pipe_mc_(vvw->pipe_mc), pipe_vv_(vvw->pipe_vv), node_id_(vvw->node_id), cs_client_mutex(*(vvw->cs_mutex))
{ 
	if(pthread_mutex_init(&cs_mutex, NULL) == -1) 
			cerr << WRITE_PRETTY_FUNCTION << " pthread mutex init error " << strerror(errno) << endl;	
	if(pthread_cond_init(&vvinfo_updated, NULL) == -1) 
			cerr << WRITE_PRETTY_FUNCTION << "pthread cond init error " << strerror(errno) << endl;

	ASSERTING(vvw->node_id == PlanetLabArgs::nodeId());
	ASSERTING(node_id_ == PlanetLabArgs::nodeId());

    vvNode_ = VivaldiNode(getDim(), VivaldiSpace::maxNodeErr); 

	myNodeData_.coords = vector<double>(getDim());
	for(int i = 0; i < getDim(); i++)
		myNodeData_.coords[i] = 0.0f;
	myNodeData_.rtt = 0.0f;
	myNodeData_.err = 0.0f;
	myNodeData_.id  = vvw->node_id;

	sentToServer_ = false;
	recvFromServer_ = false;
}

/*-----------------------------------------------------------------------
			starting the vivaldi server threads
------------------------------------------------------------------------- */

void PLVivaldiClient::start()
{
	pthread_t thread_rcv, thread_gr, thread_snd, thread_png;

	int err = pthread_create(&thread_rcv, NULL, &plvvclient_rcv, this );
	cerr << WRITE_PRETTY_FUNCTION << " creating thread receive vivaldi info " << strerror(err) <<  endl;
	if(err != 0) throw("PLVivaldiClient::threads() : error creating thread client_gen ");

	err = pthread_create(&thread_gr, NULL, &plvvclient_gen, this );
	cerr << WRITE_PRETTY_FUNCTION << " creating thread generate vivaldi info " << strerror(err) <<  endl;
	if(err != 0) throw("PLVivaldiClient::threads() : error creating thread client_gen ");
	
	err = pthread_create(&thread_snd, NULL, &plvvclient_snd, this );
	cerr << WRITE_PRETTY_FUNCTION << " creating thread send vivaldi info " << strerror(err) <<  endl;
	if(err != 0) throw("PLVivaldiClient::threads() : error creating thread client_snd ");
	
	if(PlanetLabArgs::vivaldiConf() == PlanetLab::PING_NEIGHBOR)
	{
		err = pthread_create(&thread_png, NULL, &plvvpinger, this );
		cerr << WRITE_PRETTY_FUNCTION << " creating thread vivaldi pinger " << strerror(err) <<  endl;
		if(err != 0) throw("PLVivaldiClient::threads() : error creating thread plvvpinger ");
	}

	pthread_join(thread_snd, NULL);
	pthread_join(thread_gr, NULL);
	pthread_join(thread_rcv, NULL);
	
	if(PlanetLabArgs::vivaldiConf() == PlanetLab::PING_NEIGHBOR)
		pthread_join(thread_png, NULL);
}

/*-----------------------------------------------------------------------
						vivaldi client thread
------------------------------------------------------------------------- */
void* plvvclient_rcv(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi receiver thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_RECEIVER, ThreadInfo::RUN, &plvvclient_rcv, arg);

		PLVivaldiClient *plv = (PLVivaldiClient*) arg; 
		plv->client();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_RECEIVER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_RECEIVER, KILL_PROGRAM);
	}

	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab VIVALDI_CLIENT_RECEIVER vivaldi receiver thread " << endl;
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_RECEIVER, ThreadInfo::STOP);

	return NULL;
}

/*-----------------------------------------------------------------------
						vivaldi thread
------------------------------------------------------------------------- */
void
PLVivaldiClient::client()
{
	double failsafe = 0;
	cerr << WRITE_PRETTY_FUNCTION << " planet lab vivaldi thread " << endl;
	while(true)
	{
		VivaldiInfoPtr inf = Channel::muxLVivaldiClient.consumer_pop();
		if(!inf) break;
		if(!debug(SystemSpace::VIVALDI_CLIENT_RECEIVER,inf,failsafe)) break;

		int remote = PlanetLab::getRemote(inf->getEid(), node_id_);

		if(inf->getType() == VivaldiSpace::NEIGHBOR)
		{
			if(sentToServer_ == true) continue;
			//cerr << WRITE_PRETTY_FUNCTION << " receivied new info " << *inf << endl;

			lock();

			commNodes.insert(remote);
			double min_rtt = 0;
			if(PlanetLabArgs::vivaldiConf() == PlanetLab::PING_NEIGHBOR)
			{
				if(mapNodeRTT.count(remote)) min_rtt = mapNodeRTT[remote];
				if(min_rtt > 0) inf->in.rtt = min_rtt;
			}
			
			vvNode_.update(inf->in);
	
			if(vvNode_.updateData(myNodeData_) && sentToServer_ == false) 	
			{
				//cerr << WRITE_PRETTY_FUNCTION << " updated information " << endl;
				// if the update changed the coordinates then -> signal generator thread
				
				if(!neighborHood_.empty()) signal();
			
				// reply 
				VivaldiInfoPtr vvip = VivaldiInfoPtr( new VivaldiInfo(myNodeData_));
				vvip->eid = EdgeId(node_id_, remote); //PlanetLab::getRemote(inf->getEid(), node_id_));
				vvip->type = VivaldiSpace::NEIGHBOR;
				vvip->msg = VIVALDI_GROUP;
				//cerr << WRITE_PRETTY_FUNCTION << " sending reply vivaldi info to NEIGHBOR " << *vvip << endl;
				Channel::muxLVivaldiMulticast.producer_push_back(vvip);
			}
	
			unlock();
		}
		else if(inf->getType() == VivaldiSpace::FROM_SERVER)
		{
			// assign vivaldi group -> neighborHood
			//cerr << WRITE_PRETTY_FUNCTION << " vivaldi info FROM_SERVER " << *inf << endl;	
			char *buffer = (char*)inf->getBuf();
			char *token = strtok_r((char*)inf->getBuf(), " ", &buffer); 
			int ret = PlanetLab::readAddress(token, buffer, nsmVivaldiGroup_);
			ASSERTING(!nsmVivaldiGroup_.islocked());
	
			cerr << WRITE_PRETTY_FUNCTION  << " Vivaldi Group: ";		
			NodeSocketMap::iterator it, it_end;
			for(it = nsmVivaldiGroup_.unlocked_begin(), it_end = nsmVivaldiGroup_.unlocked_end(); it != it_end; ++it)
			{
				if(it->first == node_id_) continue;

				neighborHood_.insert(it->first);
				cerr << it->first << ", " ;
			}
			cerr << endl;

			ASSERTING(!neighborHood_.empty());

			// init args
			PlanetLabArgs::groupSizeVV(neighborHood_.size() + 1);
			recvFromServer_ = true;
			commNodes = commNodes + neighborHood_;
			
			// start sending to neighbors
			lock();
				signal();
			unlock();
		}
		else if(inf->getType() == VivaldiSpace::NO_TYPE)
		{
			cerr << WRITE_PRETTY_FUNCTION << " the end: " << inf->getType() << endl;
			break;
		}
		else if(inf->getType() == VivaldiSpace::NEIGHBOR_UNREACHABLE)
		{
			cerr << WRITE_PRETTY_FUNCTION << " neighbor is unreachable : " << *inf << endl;
			//if(false == sentToServer_)
			//{
				//int id = PlanetLab::getRemote(inf->getEid(), node_id_);
				//stringstream msg;
				//msg << "neighbor " << id << " is unreachable : " << *inf << endl;
				//throw ((char*) (msg.str()).c_str());
				//lock();
				//	neighborHood_.erase(id);
				//unlock();
			//}
		}
		else
		{
			cerr << WRITE_PRETTY_FUNCTION << " ERROR! can't recognize type " << *inf << endl;
			//throw(" vivaldi client() : ERROR! can't recognize type in vivaldi info ");
		}
	}

	// -- Vivaldi client is ending, wake up remaining threads --
	lock();
		signal();
	unlock();
}

/*-----------------------------------------------------------------------
						vivaldi client generator thread
------------------------------------------------------------------------- */
void* plvvclient_gen(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab client generator thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_GENERATOR, ThreadInfo::RUN, &plvvclient_gen, arg);

		PLVivaldiClient *plv = (PLVivaldiClient*)arg; 
		plv->generator();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_GENERATOR, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_GENERATOR, KILL_PROGRAM);
	}

	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab client generator thread " << endl;
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_GENERATOR, ThreadInfo::STOP);

	return NULL;
}

/*-----------------------------------------------------------------------
		vivaldi thread - generates vivaldi information to neighbors
------------------------------------------------------------------------- */
void
PLVivaldiClient::generator()
{
	// for sending Vivaldi information to neighbors
	cerr << WRITE_PRETTY_FUNCTION << " ready to send vivaldi headers to neighbors " << endl;
	bool first= true;
	
	locked_wait();
	if(!VivaldiSpace::endsafe(SystemSpace::VIVALDI_CLIENT_GENERATOR) || neighborHood_.empty()) 
	{
		unlock();
		return;
	}
		
	neighborHood_ = neighborHood_ - node_id_; 
	cerr << WRITE_PRETTY_FUNCTION << " sending vivaldi info to neighbors " << neighborHood_ << endl;
	unlock();
	
	ASSERTING(!neighborHood_.empty());
	if(neighborHood_.empty()) throw("VivaldiClient: neighborHood is empty! ");
	
	time_t start_time = ::time(NULL); 
	if(start_time <= 0) throw("time() returned " + string((char*)start_time));

	time_t interval_time = std::min(PlanetLabArgs::linkAlgoTimeSec(), PlanetLabArgs::linkAlgoIntervalSec());
	time_t total_time = 0;

	// generates vivaldi data for n seconds
	while(::time(NULL) - start_time <= PlanetLabArgs::linkAlgoTimeSec())
	{
		ASSERTING(::time(NULL) > 0);

		for(VertexSet::iterator vit = neighborHood_.begin(), vit_end = neighborHood_.end(); vit != vit_end; ++vit)
		{
			int id = *vit;
			if(!first)
			{
				//cerr << WRITE_PRETTY_FUNCTION << " waiting for my info update " << endl;
				locked_timed_wait(1);
				unlock();
			}
			
			if(!VivaldiSpace::endsafe(SystemSpace::VIVALDI_CLIENT_GENERATOR)) return;
			if(::time(NULL) - start_time > PlanetLabArgs::linkAlgoTimeSec()) break;
			
			lock();
				VivaldiInfoPtr vvip = VivaldiInfoPtr( new VivaldiInfo(myNodeData_));
			unlock(); 

			vvip->eid = EdgeId(node_id_, id);
			vvip->type = VivaldiSpace::NEIGHBOR;
			vvip->msg = VIVALDI_GROUP;
			//cerr << WRITE_PRETTY_FUNCTION << " sending vivaldi info to NEIGHBOR " << *vvip << endl;

			Channel::muxLVivaldiMulticast.producer_push_back(vvip);

			// periodic logging to server
			if(::time(NULL) - start_time > interval_time + total_time)
			{
				total_time = total_time + interval_time; //(::time(NULL) - start_time);
				logNodeCoordsToServer();
			}
		}
		first = false;
	}

	// now: vivaldi is done -> send data to server
	if(PlanetLabArgs::linkAlgoTimeSec() >= interval_time + total_time)
		logNodeCoordsToServer();

	sentToServer_ = true;
}

void PLVivaldiClient::logNodeCoordsToServer()
{
	cerr << WRITE_PRETTY_FUNCTION << " logging node coords to server "  << endl;

	lock(); 
		VivaldiInfoPtr vvip = VivaldiInfoPtr( new VivaldiInfo(myNodeData_));
	unlock();
	vvip->eid = EdgeId(node_id_, PlanetLabArgs::serverId()); 
	vvip->type = VivaldiSpace::TO_SERVER;

	SocketMessagePtr smp;
	PlanetLab::createSocketMessage(smp, vvip, VIVALDI_GROUP, SocketMessage::VIVALDI_SERVER_TYPE, PlanetLabArgs::serverId());
	SocketChannel::muxLSocketMessageToServer.push_back(smp); 

	int pipe_cs = SystemSpace::getPipe(SystemSpace::CLIENT_SOCKET, SystemSpace::WRITE);
	int err = write(pipe_cs, &smp->type, sizeof(smp->type));
	if(err <= 0) 
	{
		cerr << WRITE_PRETTY_FUNCTION << " write returned " << err << " pipe: " << pipe_cs << endl;
		throw("VivaldiServer: client socket pipe error ");
	}
}

/*-----------------------------------------------------------------------
		vivaldi thread - sends vivaldi information to multicast socket
------------------------------------------------------------------------- */
void* plvvclient_snd(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << " started planet lab vivaldi sending thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_SENDER, ThreadInfo::RUN, &plvvclient_snd, arg);

		PLVivaldiClient *plv = (PLVivaldiClient*)arg; 
		plv->sender();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_SENDER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_SENDER, KILL_PROGRAM);
	}
	
	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab vivaldi sending thread" << endl;
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_SENDER, ThreadInfo::STOP);

	return NULL;
}

void
PLVivaldiClient::sender()
{
	cerr << WRITE_PRETTY_FUNCTION << " sender is up" << endl;

	while(true)
	{	
		VivaldiInfoPtr vip = Channel::muxLVivaldiMulticast.consumer_pop();
		if(!vip) break;
		if(!SystemSpace::debug()) break;
		if(true == sentToServer_) break;
		
		//cerr << WRITE_PRETTY_FUNCTION << " popped : " << *vip << endl;
		int remote = PlanetLab::getRemote(vip->getEid(), node_id_); 
		SocketInfoPtr sip;
		if(!getSocketInfo(remote, sip)) continue;
		
		ASSERTING(sip);
		//cerr << WRITE_PRETTY_FUNCTION << " socket info: " << *sip << endl;

		usleep(10000);

		SocketMessage::Type t;
		if(vip->getType() == VivaldiSpace::NEIGHBOR) t = SocketMessage::VIVALDI_CLIENT_TYPE;
		else if(vip->getType() == VivaldiSpace::TO_SERVER) t = SocketMessage::VIVALDI_SERVER_TYPE;
					
		SocketMessagePtr smp = SocketMessagePtr( new SocketMessage() );
		smp->sip = SocketInfoPtr( new SocketInfo( *sip ) );
		smp->target = remote;  
		smp->type = t;
		smp->vvhp = VivaldiHeaderPtr( new VivaldiHeader(*vip) );

		SocketChannel::muxLSocketMessageMultiCast.push_back(smp);
		int pipe_nl = SystemSpace::getPipe(SystemSpace::MULTICAST_SOCKET, SystemSpace::WRITE);
		int err = write(pipe_nl, &smp->type, sizeof(smp->type));
		if(err <= 0) 
		{
			cerr << WRITE_PRETTY_FUNCTION << " write returned " << err << " pipe: " << pipe_nl << endl;
			throw("vivaldiClient::sender(): write returned <= 0");
		}
		
		if(!VivaldiSpace::endsafe(SystemSpace::VIVALDI_CLIENT_SENDER)) break;
	}
}

/*-----------------------------------------------------------------------
						vivaldi client thread
------------------------------------------------------------------------- */
void* plvvpinger(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi pinger thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_PINGER, ThreadInfo::RUN, &plvvpinger, arg);

		PLVivaldiClient *plv = (PLVivaldiClient*) arg; 
		plv->pinger();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_PINGER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_CLIENT_PINGER, KILL_PROGRAM);
	}

	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab VIVALDI_CLIENT_PINGER vivaldi receiver thread " << endl;
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_CLIENT_PINGER, ThreadInfo::STOP);

	return NULL;
}

void PLVivaldiClient::pinger()
{
	// wait for neighbor list to arrive from the server
	locked_wait();
	unlock();

	VertexSet nodeSet = commNodes;
	while(!nodeSet.empty())
	{	
		for(VertexSet::iterator it = nodeSet.begin(), it_end = nodeSet.end(); it != it_end; ++it)
		{
			if(!SystemSpace::debug()) break;
			if(true == sentToServer_) break;

			double min_rtt = getRoundTripTime(*it);
			if(min_rtt > 0)
			{
				lock();
					mapNodeRTT.insert(pair<int, double>(*it, min_rtt));
				unlock();
			}
	
			usleep(100000);
		}

		if(commNodes.size() != nodeSet.size())	
		{
			lock();
				nodeSet = commNodes;
			unlock();
		}
				
		if(!SystemSpace::debug()) break;
		if(true == sentToServer_) break;
	}
}

double PLVivaldiClient::getRoundTripTime(int remote)
{
	SocketInfoPtr sip;
	if(!getSocketInfo(remote, sip)) return 0.0;
			
	double min_rtt = 0;
	if(PlanetLabArgs::vivaldiConf() == PlanetLab::PING_NEIGHBOR)
	{
		min_rtt = PlanetLab::my_ping(sip->getAddress(), 1);
		//cerr << WRITE_PRETTY_FUNCTION << " received ping min_rtt " << min_rtt << " from : " << sip->getAddress() << endl;
	}
	//else use TCP_INFO estimated RTT (already in vip.in.rtt)
	
	return min_rtt;
}

bool PLVivaldiClient::getSocketInfo(int remote, SocketInfoPtr &sip)
{
	NodeSocketMap::const_iterator sit = nsmVivaldiGroup_.public_find(remote); 
	if(sit == nsmVivaldiGroup_.unlocked_end()) 
	{	
		// use socket info first
		if(SystemSpace::getNodeAddress(remote, sip)) 
		{
			ASSERTING(sip);
			if(sip->getStatus() == SocketInfo::CONNECTED) 
			{
				cerr << WRITE_PRETTY_FUNCTION << " inserting " << remote << ":" << *sip << endl;
				nsmVivaldiGroup_.insertSocket(remote, sip);
			}
		}
		if(!requestedInfo.contains(remote) && requestNodeAddress(remote)) requestedInfo.insert(remote);

		return false;
	}
	
	sip = sit->second;
	return true;
}


bool
PLVivaldiClient::requestNodeAddress(int remote)
{
	if(PlanetLabArgs::serverId() < 0) return false;
	
	// request information from server
	GroupChangePtr gcp = GroupChangePtr( new GroupChange() );
	gcp->setEid(node_id_, PlanetLabArgs::serverId());
	gcp->setMsg(NODE_ADDRESS_INFO);
	stringstream buf;
	PlanetLab::writeAddress(buf, VertexSet(remote));
	gcp->buf.set((buf.str()).c_str());
	Channel::muxLGroupChange.producer_push_back(gcp);

	return true;
}

/*-----------------------------------------------------------------------


			  		vivaldi server thread

		  
------------------------------------------------------------------------- */
void* plvvserver(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER, ThreadInfo::RUN, &plvvserver, arg);
		
		VVWrapper *vvw = (VVWrapper*)arg;
		if(vvw == NULL) throw("plvvserver: VVWrapper == NULL");
		
		PLVivaldiServer plv(vvw); 
		plv.start(); 
		plv.join();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER, KILL_PROGRAM);
	}
	
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER, ThreadInfo::STOP);
	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab vivaldi thread " << endl;

	return NULL;
}
/*-----------------------------------------------------------------------
					PLVivaldiServer constructor
------------------------------------------------------------------------- */
PLVivaldiServer::PLVivaldiServer(VVWrapper *vvw) : pipe_mc_(vvw->pipe_mc), pipe_vv_(vvw->pipe_vv), node_id_(vvw->node_id)
												, global_mesh_(vvw->global_mesh), cs_server_mutex(*(vvw->cs_mutex)), mesh_updated(*(vvw->condition))
{ 
	if(pthread_mutex_init(&cs_mutex, NULL) == -1) 
			cerr << WRITE_PRETTY_FUNCTION << " pthread mutex init error " << strerror(errno) << endl;	

	vvNode_ = VivaldiNode(getDim(), VivaldiSpace::maxNodeErr); 

	groupSizeVV_ = PlanetLabArgs::groupSizeVV();
}

/*-----------------------------------------------------------------------
			starting the vivaldi server threads
------------------------------------------------------------------------- */
void PLVivaldiServer::start()
{
	int err = pthread_create(&thread_gr, NULL, &plvvserver_groups, this );
	cerr << WRITE_PRETTY_FUNCTION << "creating thread create groups" << strerror(err) << endl;

	err = pthread_create(&thread_rcv, NULL, &plvvserver_rcv, this );
	cerr << WRITE_PRETTY_FUNCTION << "creating thread create groups" << strerror(err) << endl;

	err = pthread_create(&thread_log, NULL, &plvvserver_log, this );
	cerr << WRITE_PRETTY_FUNCTION << "creating thread log node coordinates" << strerror(err) << endl;
	
	if(0)
	{
		TimedEvent* tep = new TimedEvent(SystemSpace::STOP_GROUP_COMMUNICATION, PlanetLabArgs::linkAlgoTimeSec() + 180);
		pthread_t thread_timed;
		int err = pthread_create(&thread_timed, NULL, &system_timed_thread, tep);
		ASSERTING(err != -1);
		pthread_detach(thread_timed);
	}
}

void PLVivaldiServer::join()
{
	pthread_join(thread_gr, NULL); 
	pthread_join(thread_rcv, NULL); 
	pthread_join(thread_log, NULL);

	GroupChangePtr gcp = GroupChangePtr( new GroupChange() );
	gcp->setEid(-1, node_id_); 					
	gcp->setMsg(READY_GROUP_COMMUNICATION);
	Channel::muxLGroupChange.producer_push_back(gcp);
}

/*-----------------------------------------------------------------------
			starting the vivaldi server threads
------------------------------------------------------------------------- */
void* plvvserver_rcv(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_RECEIVER, ThreadInfo::RUN, &plvvserver_rcv, arg);
		
		PLVivaldiServer *plv = (PLVivaldiServer*)arg; 
		plv->server();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_RECEIVER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_RECEIVER, KILL_PROGRAM);
	}
	
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_RECEIVER, ThreadInfo::STOP);
	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab vivaldi thread " << endl;

	return NULL;
}

/*-----------------------------------------------------------------------
						vivaldi server thread
	thread for creating the vivaldi groups as group_mesh_ increases in size
	distribute group to clients 
------------------------------------------------------------------------- */
void
PLVivaldiServer::server()
{
	cerr << WRITE_PRETTY_FUNCTION << " planet lab vivaldi thread " << endl;
	
	double failsafe = 0;
	while(true)
	{
		VivaldiInfoPtr inf = Channel::muxLVivaldiServer.consumer_pop();
		if(!inf) break;
		if(!VivaldiSpace::debug(SystemSpace::VIVALDI_SERVER, inf, failsafe)) break;
		if(!SystemSpace::debug()) break;

		if(inf->getType() == VivaldiSpace::TO_SERVER) 
		{
			// -- debug --
			cerr << WRITE_PRETTY_FUNCTION << " received vivaldi info TO_SERVER " << *inf << " coords.size() " << inf->getCoords().size() << endl;	
			ASSERTING(PlanetLab::getRemote(inf->getEid(), node_id_) == inf->getId());
			// -- debug end --
			
			lock();
				// -- debug validity of coordinates --
				if(!appMembers_.contains(inf->getId())) checkValidity(inf);
				// -- end debug validity --

				mapNodeCoords[inf->getId()] = inf->getNodeData();
				appMembers_.insert(inf->getId());
			unlock();
		}
		else
		{
			cerr << WRITE_PRETTY_FUNCTION << " ERROR! can't recognize type " << inf->getType() << endl;
			//throw(" client() : ERROR! can't recognize type in vivaldi info ");
		}
	}

	
	// -- Vivaldi server is finished, wakeup remaining threads --
	lock_server();
		signal();
	unlock_server();
}

/*-----------------------------------------------------------------------
			starting the vivaldi spg log thread
------------------------------------------------------------------------- */
void* plvvserver_log(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_LOGGER, ThreadInfo::RUN, &plvvserver_log, arg);
		
		PLVivaldiServer *plv = (PLVivaldiServer*)arg; 
		plv->logNodeCoords();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_LOGGER, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_LOGGER, KILL_PROGRAM);
	}
	
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_LOGGER, ThreadInfo::STOP);
	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab vivaldi thread " << endl;

	return NULL;
}

void
PLVivaldiServer::logNodeCoords()
{
	time_t leverage_time = 20;
	time_t interval_time = std::min(PlanetLabArgs::linkAlgoTimeSec(), PlanetLabArgs::linkAlgoIntervalSec());
	time_t total_time = 0;
	
	time_t start_time = ::time(NULL);
	time_t log_time = interval_time + leverage_time;

	while(true)
	{
		while(::time(NULL) - start_time < log_time)
		{
			sleep(1);
			if(!SystemSpace::debug()) 
			{
				cerr << WRITE_PRETTY_FUNCTION << " Logger is terminating " << endl;
				return;
			}
		}
		
		// log node coords every n minutes
		total_time += interval_time;
		logNodeCoords(total_time);
		
		if(total_time > PlanetLabArgs::linkAlgoTimeSec() + leverage_time) break;
		if(!SystemSpace::debug()) break;
	
		start_time = ::time(NULL);
		log_time = interval_time;
	}

	// end vivaldi
	Channel::muxLVivaldiServer.producer_push_back(VivaldiInfoPtr());
}

void
PLVivaldiServer::logNodeCoords(int seconds)
{
	cerr << WRITE_PRETTY_FUNCTION << " logging node coords " << seconds << endl;

	if(appMembers_.empty()) return;
	
	lock();
	
	using namespace boost::numeric::ublas;
	int maxNodeId_ = *(appMembers_.rbegin());
	distMatrix = matrix<double>(maxNodeId_ + 2, maxNodeId_ + 2);
	for(MapNodeCoords::iterator it = mapNodeCoords.begin(), it_end = mapNodeCoords.end(); it != it_end; ++it)
	{
		Coordinates X(it->second.coords);
		vertex_descriptorN x = it->second.id;
			
		for(MapNodeCoords::iterator it_in = mapNodeCoords.begin(), it_in_end = mapNodeCoords.end(); it_in != it_in_end; ++it_in)
		{
			vertex_descriptorN y = it_in->second.id;
			if(x == y)
			{
				distMatrix(x, y) = 0;
			}
			else
			{
				Coordinates Y(it_in->second.coords);
				distMatrix(x, y) = X.distance(Y);
					
				VVEdgeData ed;
				ed.weight = distMatrix(x,y);	
				ed.id = pair<int,int>(x,y);
					
				lock_server();
					updateMesh(ed);
				unlock_server();
			}
		}	
	}
	unlock();

	lock_all();
		ofstream fs(PlanetLabArgs::traceFile(), ios::app);
		writeStats(*global_mesh_, seconds, fs);
		fs.close();
	unlock_all();

	cerr << WRITE_PRETTY_FUNCTION << " Global graph is finished, " << global_mesh_->V.size() << " nodes " << endl; 
	//global_mesh_->printGraph(cerr);
}

void
PLVivaldiServer::checkValidity(VivaldiInfoPtr inf)
{
	// -- validity of statistics, 0 is valid, 1 is invalid --
	int valid_coords = 1;
	std::vector<double> coords = inf->getCoords();
	for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
	{
		double w = (double)*vit;
		//cerr << w << ", " ;
		if(w > 0 && w < (numeric_limits<double>::max)()) 
		{
			valid_coords = 0;
			//cerr << " valid coords " << w << " == " << *vit << endl;
		}
	}

	PLServerBase* sp = (PLServerBase*) SystemSpace::getObject(SystemSpace::SERVER);
	NodeSocketMap& addressMap = sp->getAddressMap();
	ofstream of("/hom/knuthelv/results/pl_nodes.hosts", ios::app);
	of << valid_coords << " " << addressMap[inf->getId()]->getAddress() << endl;
	of.close();
	// -- end validity of statistics, 0 is valid, 1 is invalid --
}

void
PLVivaldiServer::writeStats(const TreeStructure &T, const double &seconds, ostream &ostr)
{
	// inside critical section
	ostr << "-V s -t " << seconds << " -g " << PlanetLabArgs::groupSizeVV() << " -d " << getDim() << endl;

	for(map<int, VertexSet>::iterator it = vivaldiGroups_.begin(), it_end = vivaldiGroups_.end(); it != it_end; ++it)
	{
		VertexSet group = it->second;
		vertex_descriptorN v = it->first;
			
		ostr << "-V g -id " << v << " -m " ;
		for(VertexSet::iterator it_in = group.begin(), it_in_end = group.end(); it_in != it_in_end; ++it_in)
		{
			if(*it_in == v) continue;
			ostr << *it_in << " " ;
		}
		ostr << endl;
	}
		
	for(MapNodeCoords::iterator it = mapNodeCoords.begin(), it_end = mapNodeCoords.end(); it != it_end; ++it)
	{
		Coordinates X(it->second.coords);
		vertex_descriptorN x = it->second.id;
		
		ostr << "-V c -id " << x << " -c " ;

		for(vector<double>::iterator vit = it->second.coords.begin(), vit_end = it->second.coords.end(); vit != vit_end; ++vit)
		{
			ostr << *vit << " " ;
		}
		ostr << endl;	
	}	

	for(VertexSet::const_iterator it = T.V.begin(), it_end = T.V.end(); it != it_end; it++)
	{
		vertex_descriptorN u = *it;
		if(!appMembers_.contains(u)) continue;

		for(VertexSet::const_iterator it_in = it, it_in_end = T.V.end(); it_in != it_in_end; it_in++)
		{
			if(it_in == it) continue;
				
			vertex_descriptorN v = *it_in;
			if(!appMembers_.contains(v)) continue;

			pair<edge_descriptorN, bool> ep = edge(u,v,T.g);
			ASSERTING(ep.second);
			
			ostr << "-V e -e " << ep.first << " -lt " << T.g[ep.first].weight << endl;
		}
	}
}

/*-----------------------------------------------------------------------
					vivaldi group server thread
------------------------------------------------------------------------- */
void* plvvserver_groups(void* arg)
{
	cerr << WRITE_PRETTY_FUNCTION << "started planet lab vivaldi group thread " << endl;
	
	try
	{
		SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_GROUPS, ThreadInfo::RUN, &plvvserver_groups, arg);

		PLVivaldiServer *plv = (PLVivaldiServer*)arg; 
		plv->groups();
	}
	catch(const char *in)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_GROUPS, KILL_PROGRAM, in);
	}
	catch(...)
	{
		SystemSpace::exception(SystemSpace::VIVALDI_SERVER_GROUPS, KILL_PROGRAM);
	}
	
	SystemSpace::notifySystemMonitor(SystemSpace::VIVALDI_SERVER_GROUPS, ThreadInfo::STOP);
	cerr << WRITE_PRETTY_FUNCTION << "terminated planet lab vivaldi group thread " << endl;

	return NULL;
}

/*-----------------------------------------------------------------------
					vivaldi group server thread
	sleep until woken up because group_mesh_ has been updated... 
		-> new members have arrived
------------------------------------------------------------------------- */
void
PLVivaldiServer::groups()
{
	cerr << WRITE_PRETTY_FUNCTION << " group updater/generator is up " << endl;

	while(true)
	{
		cerr << WRITE_PRETTY_FUNCTION << " waiting for mesh update " << strerror(errno) << endl;

		int rc = pthread_mutex_lock(&cs_server_mutex);
		pthread_cond_wait(&mesh_updated, &cs_server_mutex);

		if(!VivaldiSpace::endsafe(SystemSpace::VIVALDI_SERVER_GROUPS) || !SystemSpace::endsafe())
		{
			pthread_mutex_unlock(&cs_server_mutex);
			break;
		}
		pthread_mutex_lock(&cs_mutex);
			
		VertexSet newMembers = global_mesh_->V - prev_mesh_nodes_ - 0;
		if(newMembers.size() >= groupSizeVV_) 	prev_mesh_nodes_ = global_mesh_->V;	
		
		pthread_mutex_unlock(&cs_mutex);
		pthread_mutex_unlock(&cs_server_mutex);

		cerr << WRITE_PRETTY_FUNCTION << " ----------------------------> mesh updated " << newMembers << endl;

		if(newMembers.size() >= groupSizeVV_) createRandomGroups(newMembers, groupSizeVV_);

		if(!VivaldiSpace::endsafe(SystemSpace::VIVALDI_SERVER_GROUPS) || !SystemSpace::endsafe())
			break;

		// TODO
		break;
	}
}

void
PLVivaldiServer::createRandomGroups(VertexSet &newMembers, int sz)
{
	cerr << WRITE_PRETTY_FUNCTION << " creating groups " << newMembers << " : " << endl;

	int max_num = newMembers.size() + 1;
	for(VertexSet::iterator it = newMembers.begin(), it_end = newMembers.end(); it != it_end; ++it)
	{
		VertexSet group(*it);
			
		while(group.size() < sz)
		{
			int v = VivaldiSpace::randomInt(max_num);
			if(newMembers.count(v)) group.insert(v);
		}

		vivaldiGroups_.insert(pair<int,VertexSet>(*it, group));
	}
	
	for(map<int, VertexSet>::iterator it = vivaldiGroups_.begin(), it_end = vivaldiGroups_.end(); it != it_end; ++it)
	{
		VertexSet group = it->second;
		vertex_descriptorN v = it->first;
			
		GroupChangePtr gcp = GroupChangePtr( new GroupChange() );
		gcp->setEid(node_id_, v);
		gcp->setMsg(CREATE_VIVALDI_GROUP);
		
		stringstream buf;
		buf << " -N";
		for(VertexSet::iterator it_in = group.begin(), it_in_end = group.end(); it_in != it_in_end; ++it_in)
		{
			if(*it_in == v) continue;
			buf << " ( " << *it_in << ",000.000.000.000,0000 )" ;
		}
		
		gcp->buf.set((buf.str()).c_str());
		Channel::muxLGroupChange.producer_push_back(gcp);
	}
}

void
PLVivaldiServer::createGroups(VertexSet &newMembers)
{
	for(VertexSet::iterator it = newMembers.begin(), it_end = newMembers.end(); it != it_end; ++it)
	{
		GroupChangePtr gcp = GroupChangePtr( new GroupChange() );
		gcp->setEid(node_id_, *it);
		gcp->setMsg(CREATE_VIVALDI_GROUP);
		
		stringstream buf;
		buf << " -N";
		for(VertexSet::iterator it_in = newMembers.begin(), it_in_end = newMembers.end(); it_in != it_in_end; ++it_in)
		{
			if(*it_in == *it) continue;
			buf << " ( " << *it_in << ",000.000.000.000,0000 )" ;
		}
		
		gcp->buf.set((buf.str()).c_str());
		Channel::muxLGroupChange.producer_push_back(gcp);
	}
}

void
PLVivaldiServer::updateMesh(const VVEdgeData &ed)
{
	// do operations on global_mesh_
	EdgeProp ep;
	ep.id 		= ed.id;
	ep.weight 	= std::max(0.1, ed.weight);
	ep.cost		= 1;
	ep.delay 	= std::max(0.1, ed.weight);
	ep.bw		= 1;
	ep.length	= std::max(0.1, ed.weight);

	global_mesh_->updateEdge(ep.id, ep);
}

/*-----------------------------------------------------------------------

						namespace VivaldiSpace
	
------------------------------------------------------------------------- */
namespace VivaldiSpace
{

/*-----------------------------------------------------------------------
			debug, enabling distributed Ctrl+c signal interrupt
------------------------------------------------------------------------- */
bool debug(SystemSpace::ThreadID id, VivaldiInfoPtr vip, double &failsafe)
{
	if(failsafe++ > (std::numeric_limits<double>::max)()-2) return false;
	if(!vip) return false;
	if(!SystemSpace::debug()) return false;
	
	return true;
}

bool endsafe(SystemSpace::ThreadID id)
{
	if(!SystemSpace::debug()) return false;
	if(!SystemSpace::endsafe()) return false;

	return true;
}

int getDim() 
{
	return DIMENSION;
}

}; // namespace VivaldiSpace


