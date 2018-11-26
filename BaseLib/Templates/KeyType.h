#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Templates/BaseTypes.h"

namespace BaseLib { namespace Templates {

/**
 * Use std::tuple in some way
 */
template <typename T>
class Key1
    : public ComparableImmutableType<T>
      , public Templates::IdMethod<T>
{
public:
    Key1(const T& id)
        : ComparableImmutableType<T>(id)
    { }

    Key1()
        : ComparableImmutableType<T>()
    { }

    virtual ~Key1()
    { }

    virtual T Id() const
    {
        return this->delegate();
    }
};

//    typedef std::tuple<std::string,int,char> key_t;
//    struct KeyHash : public std::unary_function<Key1, std::size_t>
//    {
//        std::size_t operator()(const Key1& k) const
//        {
//            return std::get<0>(k)[0] ^ std::get<1>(k) ^ std::get<2>(k);
//        }
//    };
//    struct KeyEqual : public std::binary_function<Key1, Key1, bool>
//    {
//        bool operator()(const Key1& v0, const Key1& v1) const
//        {
//            return (
//                        std::get<0>(v0) == std::get<0>(v1) &&
//                        std::get<1>(v0) == std::get<1>(v1) &&
//                        std::get<2>(v0) == std::get<2>(v1)
//                        );
//        }
//    };
//    struct data
//    {
//        std::string x;
//    };
//    typedef std::unordered_map<Key1,data,KeyHash,KeyEqual> Key1Map;
//    int test()
//    {
//        Key1Map m;
//        data d;
//        d.x = "test data";
//        m[std::make_tuple("abc",1,'X')] = d;
//        auto itr = m.find(std::make_tuple(std::string("abc"),1,'X'));
//        if (m.end() != itr)
//        {
//            std::cout << "x: " << itr->second.x;
//        }
//        return 0;
//    }

/**
 * Use std::tuple in some way
 */
template <typename T, typename U>
class Key2
    : public ComparableImmutableType<std::pair<T, U>>
      , public Templates::IdMethod<std::pair<T, U>>
{
public:
    Key2(const T& t, const U& u)
        : ComparableImmutableType<std::pair<T, U>>(std::pair<T, U>(t, u))
    { }

    Key2()
        : ComparableImmutableType<std::pair<T, U>>(std::pair<T, U>())
    { }

    virtual ~Key2()
    { }

    virtual std::pair<T, U> Id() const
    {
        return this->delegate();
    }

    const T& first() const
    {
        return this->delegate().first;
    }

    const U& second() const
    {
        return this->delegate().second;
    }

    // ---------------------------------------------
    // Printing data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << "Key2(" << first() << "," << second() << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Key2<T, U>& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<Key2<T, U>> data)
    {
        ostr << data->toString();
        return ostr;
    }
};

template <typename T, typename U, typename V>
class Key3
    : public ComparableImmutableType<std::tuple<T, U, V>>
      , public Templates::IdMethod<std::tuple<T, U, V>>
{
public:
    Key3(const T& t, const U& u, const V& v)
        : ComparableImmutableType<std::tuple<T, U, V>>(std::tuple<T, U, V>(t, u, v))
    { }

    Key3()
        : ComparableImmutableType<std::tuple<T, U, V>>(std::tuple<T, U, V>())
    { }

    virtual ~Key3()
    { }

    virtual std::tuple<T, U, V> Id() const
    {
        return this->delegate();
    }

    const T& first() const
    {
        return std::get<0>(this->delegate());
    }

    const U& second() const
    {
        return std::get<1>(this->delegate());
    }

    const V& third() const
    {
        return std::get<2>(this->delegate());
    }

    // ---------------------------------------------
    // Printing data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << "Key3(" << first() << "," << second() << "," << third() << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Key3<T, U, V>& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<Key3<T, U, V>> data)
    {
        ostr << data->toString();
        return ostr;
    }
};

}}
