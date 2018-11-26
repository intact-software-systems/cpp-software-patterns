/***************************************************************************
                          vertex_set.h  -  description
                             -------------------
    begin                : Wed Feb 15 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_VERTEX_SET_KHV
#define GRAPHALGO_VERTEX_SET_KHV

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <queue>
#include <map>
#include <set>

/*-----------------------------------------------------------------------
	VertexSet -
				Extends std::set
------------------------------------------------------------------------- */
struct VertexSet : public std::set<int>
{
	VertexSet() : std::set<int>() { }
	VertexSet(const VertexSet& orig) : std::set<int>(orig) {}
	VertexSet(int orig ) : std::set<int>() 
	{
		std::set<int>::insert(orig);
	}
	
	//inline void insertVertex(SimVertexMap& simV, GraphN g, int v)
	//{
	//	VertexDescriptor vd(g[v]);
	//	simV.insert(std::pair<int, VertexDescriptor>(v, vd));
	//	insert(v);
	//}
	
	    /** Create an VertexSet from all integers in the given array.
     */
    VertexSet( int length, int* array )
        : std::set<int>( )
    {
        for( ; length>0; length-- )
            std::set<int>::insert( array[length-1] );
    }

    /** Compute the union of this set and the VertexSet other.
     *  Store the result in this set.
     */
    void setUnion( const VertexSet& other );

    /** Compute the difference of this set with the VertexSet other.
     *  Store the result in this set.
     */
    void setDiff( const VertexSet& other );

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    VertexSet getUnion( const VertexSet& other ) const;

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    VertexSet getDiff( const VertexSet& other ) const;

    /** Compute the intersection of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    VertexSet getIntersection( const VertexSet& other ) const;

    /** Test whether the integer i is in this set.
     */
    bool contains( int i ) const;

    /** Test whether any of the integers in VertexSet s are in this set.
     */
    bool contains( const VertexSet& s ) const;

    /** Test whether this set is empty.
     */
    inline bool empty( ) const {
        return std::set<int>::empty();
    }

    /** Compute the union of this set and the VertexSet other.
     *  Store the result in this set.
     */
    inline VertexSet& operator+=( const VertexSet& other ) {
        setUnion(other);
        return *this;
    }

    /** Compute the difference of this set and the VertexSet other.
     *  Store the result in this set.
     */
    inline VertexSet& operator-=( const VertexSet& other ) {
        setDiff(other);
        return *this;
    }

    /** Compute the union of this set and a set containing the
     *  integer other.
     *  Store the result in this set.
     */
    inline VertexSet& operator+=( int other ) {
        insert( other );
        return *this;
    }

    /** Compute the difference of this set and a set containing the
     *  integer other (remove other from this set).
     *  Store the result in this set.
     */
    inline VertexSet& operator-=( int other ) {
        erase( other );
        return *this;
    }

    void print( std::ostream& ostr ) const;

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline VertexSet plus( const VertexSet& other ) const    { return getUnion(other); }

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline VertexSet with( const VertexSet& other ) const    { return getUnion(other); }

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline VertexSet minus( const VertexSet& other ) const   { return getDiff(other); }

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline VertexSet without( const VertexSet& other ) const { return getDiff(other); }
};

/*-----------------------------------------------------------------------
		VertexSet typedefs
------------------------------------------------------------------------- */

//typedef std::set<int>					VertexSet;
typedef std::set<int>::iterator  	    VertexSet_it;
typedef std::set<int>::const_iterator	VertexSet_cit;
typedef std::pair<VertexSet_it,VertexSet_it> 			VertexSet_tie;

void          setUnion( const VertexSet& l, const VertexSet& r, VertexSet& dest );
inline VertexSet setUnion( const VertexSet& l, const VertexSet& r );
inline VertexSet operator+( const VertexSet& l, const VertexSet& r );

void          setDiff( const VertexSet& l, const VertexSet& r, VertexSet& dest );
inline VertexSet setDiff( const VertexSet& l, const VertexSet& r );
inline VertexSet operator-( const VertexSet& l, const VertexSet& r );

inline bool   isin( int i, const VertexSet& s ) { return s.contains(i); }

extern const VertexSet NullVertexSet;

std::ostream& operator<<( std::ostream& ostr, const VertexSet& s );

/*-----------------------------------------------------------------------
		Previously declared inline functions
------------------------------------------------------------------------- */

inline VertexSet setUnion( const VertexSet& l, const VertexSet& r )
{
    return l.getUnion( r );
}

inline VertexSet operator+( const VertexSet& l, const VertexSet& r )
{
    return l.getUnion( r );
}

inline VertexSet setDiff(  const VertexSet& l, const VertexSet& r )
{
    return l.getDiff( r );
}

inline VertexSet operator-( const VertexSet& l, const VertexSet& r )
{
    return l.getDiff( r );
}

/*-----------------------------------------------------------------------
		VertexSetTS
------------------------------------------------------------------------- */
class VertexSetTS : public VertexSet
{
public:
    VertexSetTS( );
    VertexSetTS( const VertexSetTS& orig );
    VertexSetTS( const VertexSet& orig );
    VertexSetTS( int orig );
    VertexSetTS( int length, int* array );
    VertexSetTS( double ts );
    VertexSetTS( double ts, const VertexSet& orig );
    VertexSetTS( double ts, int orig );
    VertexSetTS( double ts, int length, int* array );

    bool contains( int i ) const;
    bool contains( const VertexSet& s ) const;
    bool contains( const VertexSetTS& s ) const;

    VertexSetTS plus( const VertexSet& other ) const;
    VertexSetTS minus( const VertexSet& other ) const;
    VertexSetTS without( const VertexSet& other ) const;

    inline void setTS( double ts ) {
        _ts = ts;
    }
    inline void setNewTS( double ts ) {
        if( ts > _ts ) _ts = ts;
    }
    inline double getTS() const {
        return _ts;
    }
    bool earlier( const VertexSetTS& other ) const;
    bool later( const VertexSetTS& other ) const;

private:
    double _ts;
};

extern const VertexSetTS NullVertexSetTS;

std::ostream& operator<<( std::ostream& ostr, const VertexSetTS& s );

#endif // GRAPHALGO_VERTEX_SET_KHV


