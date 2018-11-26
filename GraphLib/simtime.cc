#include <assert.h>
#include <algorithm>

#include "simtime.h"
//#include "magic.h"

using namespace std;

SimTime::Queue SimTime::_q;
double         SimTime::_now = 0;
double*        SimTime::_deadline = NULL;

static double logtime = 100;

SimEvent::SimEvent( EventType type, double t )
    : _type(type)
    , _timestamp(t)
{ }

SimEvent::~SimEvent()
{ }

void SimTime::insert( SimEvent* ev )
{
    _q.insert( ev );
}

SimEvent* SimTime::get( )
{
    if( _q.empty() ) return NULL;
    Queue_it it = _q.begin( );
    SimEvent* ret = *it;
    _q.erase(it);
    assert( ret->_timestamp >= _now );
    _now = std::max<double>( _now, ret->_timestamp );
    if( _deadline && _now > *_deadline ) return NULL;

    if( _now > logtime )
    {
        cerr << "Time: " << _now << endl;
        //Magic::printNumbersInGroups( cerr );
        // logtime += 100;
        logtime *= 2;
    }

    return ret;
}

double SimTime::Now( )
{
    return _now;
}

void SimTime::reset( )
{
 /*   for( int i=0; i<10; i++ )
        cout << endl;
    for( int i=0; i<10; i++ )
        cout << "########################################################" << endl;
    for( int i=0; i<10; i++ )
        cout << "##                                                    ##" << endl;
        cout << "##                 Resetting SIMTIME                  ##" << endl;
    for( int i=0; i<10; i++ )
        cout << "##                                                    ##" << endl;
    for( int i=0; i<10; i++ )
        cout << "########################################################" << endl;
    for( int i=0; i<10; i++ )
        cout << endl;
*/
    _now = 0;

    // cout << __FILE__ << ":" << __LINE__ << " DEBUG EXIT" << endl;
    // exit( 0 );
}

void SimTime::setDeadline( double d )
{
    _deadline = new double(d);
}

void SimTime::dump( ostream& ostr )
{
    ostr << "Now: " << _now << " ";

    Queue_it qit, qend;
    qit  = _q.begin( );
    qend = _q.end();
    for( ;qit!=qend; qit++ )
    {
        ostr << (*qit)->_timestamp << " ";
    }
}

