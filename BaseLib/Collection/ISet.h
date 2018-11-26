#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/Synchronization.h"

namespace BaseLib { namespace Collection {

/**
 * @brief The ISet class
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 */
template <typename T>
class ISet : public std::set<T>
             , public Templates::Lockable<Mutex>
{
private:
    typedef std::set<T> Set;

public:
    ISet() : Set()
    { }

    ISet(const ISet<T>& orig) : Set(orig)
    { }

    ISet(const std::set<T>& orig) : Set(orig)
    { }

    ISet(T orig) : Set()
    {
        Set::insert(orig);
    }

    /** Create an VertexSet from all integers in the given array.
     */
    ISet(int length, T* array)
        : Set()
    {
        for(; length > 0; length--)
        {
            Set::insert(array[length - 1]);
        }
    }

    /** Compute the union of this set and the VertexSet other.
     *  Store the result in this set.
     */
    void setUnion(const ISet& other)
    {
        Set out;
        std::set_union(
            this->begin(), this->end(),
            other.begin(), other.end(),
            std::insert_iterator<Set>(out, out.begin()));
        *(Set*) this = out;
    }

    /** Compute the difference of this set with the VertexSet other.
     *  Store the result in this set.
     */
    void setDiff(const ISet& other)
    {
        Set out;
        std::set_difference(
            this->begin(), this->end(),
            other.begin(), other.end(),
            std::insert_iterator<Set>(out, out.begin()));
        *(Set*) this = out;
    }

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    ISet getUnion(const ISet& other) const
    {
        ISet result;
        std::set_union(
            this->begin(), this->end(),
            other.begin(), other.end(),
            std::insert_iterator<Set>(result, result.begin()));
        return result;
    }

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    ISet getDiff(const ISet& other) const
    {
        ISet result;
        std::set_difference(
            this->begin(), this->end(),
            other.begin(), other.end(),
            std::insert_iterator<Set>(result, result.begin()));
        return result;
    }


    /** Compute the intersection of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    ISet getIntersection(const ISet& other) const
    {
        ISet result;
        std::set_intersection(
            this->begin(), this->end(),
            other.begin(), other.end(),
            std::insert_iterator<Set>(result, result.begin()));
        return result;
    }

    /** Test whether the integer i is in this set.
     */
    bool contains(const T& i) const
    {
        return (this->find(i) != this->end());
    }

    /** Test whether any of the integers in VertexSet s are in this set.
     */
    bool contains(const ISet& s) const
    {
        ISet r;
        ISet::SetDiff(s, *this, r);
        return (r.empty());
    }

    /**
     * @brief SetUnion
     * @param l
     * @param r
     * @param out
     */
    static void SetUnion(const ISet& l, const ISet& r, ISet& out)
    {
        out.clear();
        std::set_union(
            l.begin(), l.end(),
            r.begin(), r.end(),
            std::insert_iterator<Set>(out, out.begin()));
    }

    /**
     * @brief SetDiff
     * @param l
     * @param r
     * @param out
     */
    static void SetDiff(const ISet& l, const ISet& r, ISet& out)
    {
        out.clear();
        std::set_difference(
            l.begin(), l.end(),
            r.begin(), r.end(),
            std::insert_iterator<Set>(out, out.begin()));
    }

    /**
     * @brief SetUnion
     * @param l
     * @param r
     * @return
     */
    static ISet SetUnion(const ISet& l, const ISet& r)
    {
        return l.getUnion(r);
    }

    /**
     * @brief SetDiff
     * @param l
     * @param r
     * @return
     */
    static ISet SetDiff(const ISet& l, const ISet& r)
    {
        return l.getDiff(r);
    }

    /**
     * @brief IsIn
     * @param i
     * @param s
     * @return
     */
    static bool IsIn(const T& i, const ISet& s)
    {
        return s.contains(i);
    }

    /** Test whether this set is empty.
     */
    inline bool empty() const
    {
        return Set::empty();
    }

    /** Compute the union of this set and the VertexSet other.
     *  Store the result in this set.
     */
    inline ISet& operator+=(const ISet& other)
    {
        setUnion(other);
        return *this;
    }

    /** Compute the difference of this set and the VertexSet other.
     *  Store the result in this set.
     */
    inline ISet& operator-=(const ISet& other)
    {
        setDiff(other);
        return *this;
    }

    /** Compute the union of this set and a set containing the
     *  integer other.
     *  Store the result in this set.
     */
    inline ISet& operator+=(T other)
    {
        Set::insert(other);
        return *this;
    }

    /** Compute the difference of this set and a set containing the
     *  integer other (remove other from this set).
     *  Store the result in this set.
     */
    inline ISet& operator-=(T other)
    {
        Set::erase(other);
        return *this;
    }

    /**
     * @brief operator <<
     * @param ostr
     * @param s
     * @return
     */
    friend std::ostream& operator<<(std::ostream& ostr, const ISet& s)
    {
        s.print(ostr);
        return ostr;
    }

    /**
     * @brief print
     * @param ostr
     */
    void print(std::ostream& ostr) const
    {
        typename Set::const_iterator it  = this->begin();
        typename Set::const_iterator end = this->end();

        for(; it != end; it++)
        {
            ostr << *it << " ";
        }
    }

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline ISet plus(const ISet& other) const
    {
        return getUnion(other);
    }

    /** Compute the union of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline ISet with(const ISet& other) const
    {
        return getUnion(other);
    }

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline ISet minus(const ISet& other) const
    {
        return getDiff(other);
    }

    /** Compute the difference of this set and the VertexSet other.
     *  Return the result. Keep this set unchanged.
     */
    inline ISet without(const ISet& other) const
    {
        return getDiff(other);
    }
};

/*-----------------------------------------------------------------------
        inline functions
------------------------------------------------------------------------- */
template <typename T>
inline ISet<T> operator+(const ISet<T>& l, const ISet<T>& r)
{
    return l.getUnion(r);
}

template <typename T>
inline ISet<T> operator-(const ISet<T>& l, const ISet<T>& r)
{
    return l.getDiff(r);
}

/**
 * @brief The ISetTS class
 */
template <typename T>
class ISetTS : public ISet<T>
{
public:
    ISetTS();
    ISetTS(const ISetTS& orig);
    ISetTS(const ISet<T>& orig);
    ISetTS(T orig);
    ISetTS(int length, T* array);
    ISetTS(double ts);
    ISetTS(double ts, const ISet<T>& orig);
    ISetTS(double ts, T orig);
    ISetTS(double ts, int length, T* array);

    bool contains(T i) const;
    bool contains(const ISet<T>& s) const;
    bool contains(const ISetTS& s) const;

    ISetTS plus(const ISet<T>& other) const;
    ISetTS minus(const ISet<T>& other) const;
    ISetTS without(const ISet<T>& other) const;

    inline void setTS(double ts)
    {
        _ts = ts;
    }

    inline void setNewTS(double ts)
    {
        if(ts > _ts) _ts = ts;
    }

    inline double getTS() const
    {
        return _ts;
    }

    bool earlier(const ISetTS& other) const;
    bool later(const ISetTS& other) const;

    friend std::ostream& operator<<(std::ostream& ostr, const ISetTS& s)
    {
        s.print(ostr);
        ostr << " (t " << s.getTS() << ")";
        return ostr;
    }

private:
    double _ts;
};

template <typename T>
ISetTS<T>::ISetTS()
    : ISet<T>()
    , _ts(0)
{ }

template <typename T>
ISetTS<T>::ISetTS(double ts)
    : ISet<T>()
    , _ts(ts)
{ }

template <typename T>
ISetTS<T>::ISetTS(const ISetTS& orig)
    : ISet<T>(orig)
    , _ts(orig._ts)
{ }

template <typename T>
ISetTS<T>::ISetTS(const ISet<T>& orig)
    : ISet<T>(orig)
    , _ts(0)
{ }

template <typename T>
ISetTS<T>::ISetTS(double ts, const ISet<T>& orig)
    : ISet<T>(orig)
    , _ts(ts)
{ }

template <typename T>
ISetTS<T>::ISetTS(T orig)
    : ISet<T>(orig)
    , _ts(0)
{ }

template <typename T>
ISetTS<T>::ISetTS(double ts, T orig)
    : ISet<T>(orig)
    , _ts(ts)
{ }

template <typename T>
ISetTS<T>::ISetTS(int length, T* array)
    : ISet<T>(length, array)
    , _ts(0)
{ }

template <typename T>
ISetTS<T>::ISetTS(double ts, int length, T* array)
    : ISet<T>(length, array)
    , _ts(ts)
{ }

template <typename T>
bool ISetTS<T>::contains(T i) const
{
    return ISet<T>::contains(i);
}

template <typename T>
bool ISetTS<T>::contains(const ISet<T>& s) const
{
    return ISet<T>::contains(s);
}

template <typename T>
bool ISetTS<T>::contains(const ISetTS<T>& s) const
{
    return ISet<T>::contains(s);
}

template <typename T>
ISetTS<T> ISetTS<T>::plus(const ISet<T>& other) const
{
    ISetTS result(*this);
    result.setUnion(other);
    return result;
}

template <typename T>
ISetTS<T> ISetTS<T>::minus(const ISet<T>& other) const
{
    ISetTS result(*this);
    result.setDiff(other);
    return result;
}

template <typename T>
ISetTS<T> ISetTS<T>::without(const ISet<T>& other) const
{
    ISetTS result(*this);
    result.setDiff(other);
    return result;
}

template <typename T>
bool ISetTS<T>::earlier(const ISetTS<T>& other) const
{
    return _ts < other._ts;
}

template <typename T>
bool ISetTS<T>::later(const ISetTS<T>& other) const
{
    return _ts > other._ts;
}

}}



