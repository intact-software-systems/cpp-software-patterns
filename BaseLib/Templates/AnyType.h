#ifndef BaseLib_Templates_AnyType_h_Included
#define BaseLib_Templates_AnyType_h_Included

#include"BaseLib/CommonDefines.h"

#include <algorithm>
#include <typeinfo>
#include <cstring>

namespace BaseLib { namespace Templates
{

/**
 * @brief The Any class
 */
class Any
{
public:
    Any() : content_(new Holder<bool>(false))
    { }

    template<typename ValueType>
    Any(const ValueType & value)
        : content_(new Holder<ValueType>(value))
    { }

    Any(const Any & other)
        : content_(other.content_ ? other.content_->Clone() : 0)
    { }

    virtual ~Any()
    {
        delete content_;
    }

public:
    Any & swap(Any & rhs)
    {
        std::swap(content_, rhs.content_);
        return *this;
    }

    template<typename ValueType>
    Any & operator=(const ValueType & rhs)
    {
        Any(rhs).swap(*this);
        return *this;
    }

    Any & operator=(Any rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    static Any Empty()
    {
        static Any empty;
        return empty;
    }

public:
    bool empty() const
    {
        return !content_;
    }

    const std::type_info & type() const
    {
        return content_ ? content_->Type() : typeid(void);
    }

    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const Any &rvalue) const
    {
        return *(this->content_) == *(rvalue.content_);
    }

    bool operator!=(const Any &rvalue) const
    {
        return *(this->content_) != *(rvalue.content_);
    }


private: // types

    class PlaceHolder
    {
    public:
        virtual ~PlaceHolder()
        { }

    public:
        virtual const std::type_info & Type() const = 0;

        virtual PlaceHolder * Clone() const = 0;

        virtual bool operator==(const PlaceHolder &) const = 0;

        virtual bool operator!=(const PlaceHolder &) const = 0;
    };

    template<typename ValueType>
    class Holder : public PlaceHolder
    {
    public:
        Holder(const ValueType & value)
            : held(value)
        { }

    public:
        virtual const std::type_info & Type() const
        {
            return typeid(ValueType);
        }

        virtual PlaceHolder * Clone() const
        {
            return new Holder(held);
        }

        virtual bool operator==(const PlaceHolder &other) const
        {
            const Any::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            //ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held == this->held;
        }

        virtual bool operator!=(const PlaceHolder &other) const
        {
            const Any::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            //ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held != this->held;
        }

        static const Any::Holder<ValueType>* HolderCast(const PlaceHolder *const operand)
        {
            return operand &&
                    operand->Type() == typeid(ValueType)
                    ? static_cast< const Any::Holder<ValueType> *const >(operand)
                    : 0;
        }

    public:
        ValueType held;

    private:
        // intentionally left unimplemented
        Holder & operator=(const Holder &);
    };

private:
    template<typename ValueType>
    friend ValueType * AnyCast(Any *);

    template<typename ValueType>
    friend ValueType * UnsafeAnyCast(Any *);

    PlaceHolder * content_;
};

class bad_any_cast : public std::bad_cast
{
public:
    virtual const char * what() const throw()
    {
        return "boost::bad_any_cast: "
                "failed conversion using boost::any_cast";
    }
};

template<typename ValueType>
ValueType * AnyCast(Any * operand)
{
    return operand &&
        #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
            std::strcmp(operand->type().name(), typeid(ValueType).name()) == 0
        #else
            operand->type() == typeid(ValueType)
        #endif
            ? &static_cast<Any::Holder<ValueType> *>(operand->content_)->held
            : 0;
}

template<typename ValueType>
inline const ValueType * AnyCast(const Any * operand)
{
    return AnyCast<ValueType>(const_cast<Any *>(operand));
}

//template<typename ValueType>
//ValueType AnyCast(Any & operand)
//{
//    typedef BOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;
//#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
//    // If 'nonref' is still reference type, it means the user has not
//    // specialized 'remove_reference'.
//    // Please use BOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION macro
//    // to generate specialization of remove_reference for your class
//    // See type traits library documentation for details
//    BOOST_STATIC_ASSERT(!is_reference<nonref>::value);
//#endif
//    nonref * result = AnyCast<nonref>(&operand);
//    if(!result)
//        boost::throw_exception(bad_any_cast());
//    return *result;
//}

//template<typename ValueType>
//inline ValueType AnyCast(const Any & operand)
//{
//    typedef BOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;
//#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
//    // The comment in the above version of 'any_cast' explains when this
//    // assert is fired and what to do.
//    BOOST_STATIC_ASSERT(!is_reference<nonref>::value);
//#endif
//    return AnyCast<const nonref &>(const_cast<Any &>(operand));
//}

// Note: The "unsafe" versions of any_cast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template<typename ValueType>
inline ValueType * UnsafeAnyCast(Any * operand)
{
    return &static_cast<Any::Holder<ValueType> *>(operand->content_)->held;
}

template<typename ValueType>
inline const ValueType * UnsafeAnyCast(const Any * operand)
{
    return UnsafeAnyCast<ValueType>(const_cast<Any *>(operand));
}

}}

#endif
