#ifndef SIM_TIME_H
#define SIM_TIME_H

#include <iostream>
#include <set>

struct SimEvent
{
    enum EventType
    {
        Error = 0,
        GroupChange,
        Callback
    };

    EventType _type;
    double    _timestamp;

public:
    SimEvent( EventType type, double t );
    virtual ~SimEvent();

    inline EventType type() const {
        return _type;
    }
};

/** An implementation operator< for the multiset<SimEvent*>.
 *  It specifies an order that is based only on the timestamp. All other
 *  information in the SimEvent or its child class is ignored where
 *  temporal ordering is concerned.
 */
struct SimEventCmp
{
    inline bool operator()( SimEvent* const & l, SimEvent* const & r ) const
    {
        return ( l->_timestamp < r->_timestamp );
    }
};

/** This class implements time in this discrete event simulation.
 *  As in all discrete event simulation, simulated time passes in jumps between
 *  retrieval of events from an event queue, while the time that is required for
 *  computing the simulation is zero simulated time.
 */
class SimTime
{
    /*	Discrete event simulators tend to implement their event queues using a stable
     *  tree. Conveniently, the multiset in STL is exactly such a thing. Insert a
     *  value A and the STL multiset will store it in a tree in such a way that
     *  for all B with B<A, B is located left in the tree, while all with B>A are
     *  stored right in the tree. The order for A==B is undefined, just like in
     *  typical discrete event simulators.
     *
     *  The question is: what is the operator<? It is simulation time, and it is
     *  stored in every SimEvent object. The SimEvent contains more information but
     *  that is ignored for ordering.
     */
    typedef std::multiset<SimEvent*,SimEventCmp>           Queue;
    typedef std::multiset<SimEvent*,SimEventCmp>::iterator Queue_it;

public:
    /** Put a SimEvent into the global event queue
     */
    static void insert( SimEvent* ev );

    /** Get the next event from the global event queue. If that object has a
     *  timestamp higher than the current simulation time, set the current simulation
     *  time to that timestamp.
     */
    static SimEvent* get( );

    /** Return the current simulation time.
     */
    static double Now( );

    /** Reset the simulation time to 0. Called once after a partial simulation
     *  was run to create the initial state.
     */
    static void reset( );

    /** Set an absolute time for ending the simulation
     */
    static void setDeadline( double d );

    static void dump( std::ostream& ostr );

private:
    static Queue   _q;
    static double  _now;
    static double* _deadline;
};

/** This is the most frequently used event.
 *  It encodes the time when the given node changes group membership.
 *  These values are read from the .ljt that is input to the simulation.
 */
class EvGroupChange : public SimEvent
{
public:
    EvGroupChange( double ts, int node, int oldgroup, int newgroup )
        : SimEvent( GroupChange, ts )
        , _node( node )
        , _oldgroup( oldgroup )
        , _newgroup( newgroup )
    {
    }

    void extractInfo( int& node, int& oldgroup, int& newgroup ) const
    {
        node     = _node;
        oldgroup = _oldgroup;
        newgroup = _newgroup;
    }

	int newGroup() const { return _newgroup; }
	int oldGroup() const { return _oldgroup; }
	int node()     const { return _node; }
	
private:
    int _node;
    int _oldgroup;
    int _newgroup;
};

/** All other kinds of events that must be handled in the simulation.
 *  This class is obviously abstract and must be implemented for the
 *  different algorithms if necessary.
 */
class EvCallback : public SimEvent
{
public:
    EvCallback( double ts )
        : SimEvent( Callback, ts )
    { }

    virtual void callme( ) = 0;
};

#endif /* SIM_TIME_H */

