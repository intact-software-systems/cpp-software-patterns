/***************************************************************************
                          vertex_set.h  -  description
                             -------------------
    begin                : Wed Feb 15 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "vertex_set.h"
#include <iterator>

//#include "debug.h"

using namespace std;

ostream& operator<<( ostream& ostr, const VertexSet& s )
{
    s.print( ostr );
    return ostr;
}

void VertexSet::print( ostream& ostr ) const
{
    VertexSet_cit it,end;
    it  = this->begin();
    end = this->end();
    for( ; it!=end; it++ )
    {
        ostr << *it << " ";
    }
}

void VertexSet::setUnion( const VertexSet& other )
{
    std::set<int> out;
    std::set_union( this->begin(), this->end(),
               other.begin(), other.end(),
               insert_iterator<std::set<int> >(out,out.begin()) );
    *(std::set<int>*)this = out;
}

void VertexSet::setDiff( const VertexSet& other )
{
    std::set<int> out;
    std::set_difference( this->begin(), this->end(),
                    other.begin(), other.end(),
                    insert_iterator<std::set<int> >(out,out.begin()) );
    *(std::set<int>*)this = out;
}

VertexSet VertexSet::getUnion( const VertexSet& other ) const
{
    VertexSet result;
    std::set_union( this->begin(), this->end(),
               other.begin(), other.end(),
               insert_iterator<std::set<int> >(result,result.begin()) );
    return result;
}

VertexSet VertexSet::getDiff( const VertexSet& other ) const
{
    VertexSet result;
    std::set_difference( this->begin(), this->end(),
                    other.begin(), other.end(),
                    insert_iterator<std::set<int> >(result,result.begin()) );
    return result;
}
VertexSet VertexSet::getIntersection( const VertexSet& other ) const
{
    VertexSet result;
    std::set_intersection( this->begin(), this->end(),
                      other.begin(), other.end(),
                      insert_iterator<std::set<int> >(result,result.begin()) );
    return result;
}

bool VertexSet::contains( int i ) const
{
    return ( this->find( i ) != this->end() );
}

bool VertexSet::contains( const VertexSet& s ) const
{
    VertexSet r;
    ::setDiff( s, *this, r );
    return ( r.empty() );
}

void setUnion( const VertexSet& l, const VertexSet& r, VertexSet& out )
{
    out.clear();
    std::set_union( l.begin(), l.end(),
               r.begin(), r.end(),
               insert_iterator<std::set<int> >(out,out.begin()) );
}

void setDiff(  const VertexSet& l, const VertexSet& r, VertexSet& out )
{
    out.clear();
    std::set_difference( l.begin(), l.end(),
                    r.begin(), r.end(),
                    insert_iterator<std::set<int> >(out,out.begin()) );
}

const VertexSetTS NullVertexSetTS;

/*-----------------------------------------------------------------------
		VertexSetTS
------------------------------------------------------------------------- */
VertexSetTS::VertexSetTS( )
    : VertexSet( )
    , _ts( 0 )
{ }

VertexSetTS::VertexSetTS( double ts )
    : VertexSet( )
    , _ts( ts )
{ }

VertexSetTS::VertexSetTS( const VertexSetTS& orig )
    : VertexSet( orig )
    , _ts( orig._ts )
{ }

VertexSetTS::VertexSetTS( const VertexSet& orig )
    : VertexSet( orig )
    , _ts( 0 )
{ }

VertexSetTS::VertexSetTS( double ts, const VertexSet& orig )
    : VertexSet( orig )
    , _ts( ts )
{ }

VertexSetTS::VertexSetTS( int orig )
    : VertexSet( orig )
    , _ts( 0 )
{ }

VertexSetTS::VertexSetTS( double ts, int orig )
    : VertexSet( orig )
    , _ts( ts )
{ }

VertexSetTS::VertexSetTS( int length, int* array )
    : VertexSet( length, array )
    , _ts( 0 )
{ }

VertexSetTS::VertexSetTS( double ts, int length, int* array )
    : VertexSet( length, array )
    , _ts( ts )
{ }

bool VertexSetTS::contains( int i ) const
{
    return VertexSet::contains(i);
}
bool VertexSetTS::contains( const VertexSet& s ) const
{
    return VertexSet::contains(s);
}
bool VertexSetTS::contains( const VertexSetTS& s ) const
{
    return VertexSet::contains(s);
}

VertexSetTS VertexSetTS::plus( const VertexSet& other ) const
{
    VertexSetTS result( *this );
    result.setUnion( other );
    return result;
}

VertexSetTS VertexSetTS::minus( const VertexSet& other ) const
{
    VertexSetTS result( *this );
    result.setDiff( other );
    return result;
}

VertexSetTS VertexSetTS::without( const VertexSet& other ) const
{
    VertexSetTS result( *this );
    result.setDiff( other );
    return result;
}

bool VertexSetTS::earlier( const VertexSetTS& other ) const
{
    return _ts < other._ts;
}

bool VertexSetTS::later( const VertexSetTS& other ) const
{
    return _ts > other._ts;
}

ostream& operator<<( ostream& ostr, const VertexSetTS& s )
{
    s.print( ostr );
    ostr << " (t " << s.getTS() << ")";
    return ostr;
}

const VertexSet NullVertexSet;


