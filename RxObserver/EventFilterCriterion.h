#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// Forward declarations
// ----------------------------------------------------

//template <typename Arg1>
//class FilterCriterion1;

//template <typename Arg1, typename Arg2>
//class FilterCriterion2;

//template <typename Arg1, typename Arg2, typename Arg3>
//class FilterCriterion3;

//template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
//class FilterCriterion4;

//template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
//class FilterCriterion5;

// ----------------------------------------------------
// FilterCriterion
// ----------------------------------------------------

class FilterCriterion
        : public ENABLE_SHARED_FROM_THIS(FilterCriterion)
{
public:
    virtual ~FilterCriterion() { }

    CLASS_TRAITS(FilterCriterion)

    FilterCriterion::Ptr GetPtr()
    {
        return shared_from_this();
    }

    virtual int NumArgs() const = 0;
};

typedef std::list<FilterCriterion::Ptr> FilterList;


// ----------------------------------------------------
// FilterCriterion0
// ----------------------------------------------------

class FilterCriterion0
        : public Templates::FilterMethod2<bool, Event::Ptr, Event0::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion0() { }

    CLASS_TRAITS(FilterCriterion0)

    virtual int NumArgs() const
    {
        return 0;
    }
};

// ----------------------------------------------------
// FilterCriterion1
// ----------------------------------------------------

template <typename Arg1>
class FilterCriterion1
        : public Templates::FilterMethod2<bool, Event::Ptr, typename Event1<Arg1>::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion1() { }

    CLASS_TRAITS(FilterCriterion1)

    virtual int NumArgs() const
    {
        return 1;
    }
};

// ----------------------------------------------------
// FilterCriterion2
// ----------------------------------------------------

template <typename Arg1, typename Arg2>
class FilterCriterion2
        : public Templates::FilterMethod2<bool, Event::Ptr, typename Event2<Arg1, Arg2>::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion2() { }

    CLASS_TRAITS(FilterCriterion2)

    virtual int NumArgs() const
    {
        return 2;
    }
};

// ----------------------------------------------------
// FilterCriterion3
// ----------------------------------------------------

template <typename Arg1, typename Arg2, typename Arg3>
class FilterCriterion3
        : public Templates::FilterMethod2<bool, Event::Ptr, typename Event3<Arg1, Arg2, Arg3>::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion3() { }

    CLASS_TRAITS(FilterCriterion3)

    virtual int NumArgs() const
    {
        return 3;
    }
};

// ----------------------------------------------------
// FilterCriterion4
// ----------------------------------------------------

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FilterCriterion4
        : public Templates::FilterMethod2<bool, Event::Ptr, typename Event4<Arg1, Arg2, Arg3, Arg4>::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion4() { }

    CLASS_TRAITS(FilterCriterion4)

    virtual int NumArgs() const
    {
        return 4;
    }
};

// ----------------------------------------------------
// FilterCriterion5
// ----------------------------------------------------

template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class FilterCriterion5
        : public Templates::FilterMethod2<bool, Event::Ptr, typename Event5<Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr>
        , public FilterCriterion
{
public:
    virtual ~FilterCriterion5() { }

    CLASS_TRAITS(FilterCriterion5)

    virtual int NumArgs() const
    {
        return 5;
    }
};

// ----------------------------------------------------
// FilterCriterionFactory
// ----------------------------------------------------

class FilterCriterionFactory
{
public:
    virtual ~FilterCriterionFactory() {}


    template <typename FilterType>
    static typename FilterType::Ptr To(FilterCriterion::Ptr criterion)
    {
        return std::dynamic_pointer_cast<FilterType>(criterion);
    }

//    static FilterCriterion0::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion0>(criterion);
//    }

//    template <typename Arg1>
//    static typename FilterCriterion1<Arg1>::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion1<Arg1> >(criterion);
//    }

//    template <typename Arg1, typename Arg2>
//    static typename FilterCriterion2<Arg1, Arg2>::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion2<Arg1, Arg2> >(criterion);
//    }

//    template <typename Arg1, typename Arg2, typename Arg3>
//    static typename FilterCriterion3<Arg1, Arg2, Arg3>::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion3<Arg1, Arg2, Arg3> >(criterion);
//    }

//    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
//    static typename FilterCriterion4<Arg1, Arg2, Arg3>::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion4<Arg1, Arg2, Arg3, Arg4>>(criterion);
//    }

//    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
//    static typename FilterCriterion5<Arg1, Arg2, Arg3>::Ptr To(FilterCriterion::Ptr criterion)
//    {
//        return std::dynamic_pointer_cast<FilterCriterion5<Arg1, Arg2, Arg3, Arg4, Arg5> >(criterion);
//    }
};

}}

