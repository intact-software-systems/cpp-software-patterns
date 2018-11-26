#ifndef BaseLib_Templates_AnyKeyType_h_Included
#define BaseLib_Templates_AnyKeyType_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/BaseTypes.h"

#include <algorithm>
#include <typeinfo>
#include <cstring>

namespace BaseLib { namespace Templates
{

/**
 * @brief The AnyKey class
 */
class AnyKey
{
public:
    AnyKey() : content_(new Holder<bool>(false))
    { }

    template<typename ValueType>
    AnyKey(const ValueType & value)
        : content_(new Holder<ValueType>(value))
    { }

    AnyKey(const AnyKey & other)
        : content_(other.content_ ? other.content_->Clone() : 0)
    { }

    virtual ~AnyKey()
    {
        delete content_;
    }

public:
    AnyKey & swap(AnyKey & rhs)
    {
        std::swap(content_, rhs.content_);
        return *this;
    }

    template<typename ValueType>
    AnyKey & operator=(const ValueType & rhs)
    {
        AnyKey(rhs).swap(*this);
        return *this;
    }

    AnyKey & operator=(AnyKey rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    static AnyKey Empty()
    {
        static AnyKey empty;
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
    bool operator==(const AnyKey &rvalue) const
    {
        return *(this->content_) == *(rvalue.content_);
    }

    bool operator!=(const AnyKey &rvalue) const
    {
        return *(this->content_) != *(rvalue.content_);
    }

    bool operator>(const AnyKey &rvalue) const
    {
        return *(this->content_) > *(rvalue.content_);
    }

    bool operator>=(const AnyKey &rvalue) const
    {
        return *(this->content_) >= *(rvalue.content_);
    }

    bool operator<(const AnyKey &rvalue) const
    {
        return *(this->content_) < *(rvalue.content_);
    }

    bool operator<=(const AnyKey &rvalue) const
    {
        return *(this->content_) <= *(rvalue.content_);
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

        virtual bool operator<(const PlaceHolder &) const = 0;

        virtual bool operator<=(const PlaceHolder &) const = 0;

        virtual bool operator>(const PlaceHolder &) const = 0;

        virtual bool operator>=(const PlaceHolder &) const = 0;
    };

    template<typename ValueType>
    class Holder : public PlaceHolder
                 , public BaseLib::Templates::ComparableImmutableType<ValueType>
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
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held == this->held;
        }

        virtual bool operator!=(const PlaceHolder &other) const
        {
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held != this->held;
        }

        virtual bool operator<(const PlaceHolder &other) const
        {
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held < this->held;
        }

        virtual bool operator<=(const PlaceHolder &other) const
        {
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held <= this->held;
        }

        virtual bool operator>(const PlaceHolder &other) const
        {
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held > this->held;
        }

        virtual bool operator>=(const PlaceHolder &other) const
        {
            const AnyKey::Holder<ValueType> *holderOther = Holder::HolderCast(&other);

            ASSERT(holderOther);
            if(!holderOther) return false;

            return holderOther->held >= this->held;
        }

        static const AnyKey::Holder<ValueType>* HolderCast(const PlaceHolder *const operand)
        {
            return operand &&
                    operand->Type() == typeid(ValueType)
                    ? static_cast< const AnyKey::Holder<ValueType> *const >(operand)
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
    friend ValueType * AnyKeyCast(AnyKey *);

    template<typename ValueType>
    friend ValueType * UnsafeAnyKeyCast(AnyKey *);

    PlaceHolder * content_;
};

template<typename ValueType>
ValueType * AnyKeyCast(AnyKey * operand)
{
    return operand &&
        #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
            std::strcmp(operand->type().name(), typeid(ValueType).name()) == 0
        #else
            operand->type() == typeid(ValueType)
        #endif
            ? &static_cast<AnyKey::Holder<ValueType> *>(operand->content_)->held
            : 0;
}

template<typename ValueType>
inline const ValueType * AnyKeyCast(const AnyKey * operand)
{
    return AnyKeyCast<ValueType>(const_cast<AnyKey *>(operand));
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
inline ValueType * UnsafeAnyKeyCast(AnyKey * operand)
{
    return &static_cast<AnyKey::Holder<ValueType> *>(operand->content_)->held;
}

template<typename ValueType>
inline const ValueType * UnsafeAnyKeyCast(const AnyKey * operand)
{
    return UnsafeAnyKeyCast<ValueType>(const_cast<AnyKey *>(operand));
}

}}

#endif
