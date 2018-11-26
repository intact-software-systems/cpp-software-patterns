#ifndef BaseLib_InstanceHandle_h_IsIncluded
#define BaseLib_InstanceHandle_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

typedef HANDLE_TYPE_NATIVE InstanceHandleType;

const InstanceHandleType 	HANDLE_NIL = HANDLE_NIL_NATIVE;

/* --------------------------------------------------------------
Type used to represent the identity of a data-object whose
changes in value are communicated by the RTPS protocol.

In DDS, the value of the fields labeled as key within the
data uniquely identify each data-object.

In RTPS::Cache::HistoryCache: Identifies the instance of the
data-object to which the change applies.
----------------------------------------------------------------*/
class DLL_STATE InstanceHandle
{
public:
    InstanceHandle(InstanceHandleType handle = HANDLE_NIL);
    virtual ~InstanceHandle();

public:
    InstanceHandleType  GetHandle()                                 const   { return handle_; }
    void                SetHandle(const InstanceHandleType &handle)         { handle_ = handle; }

public:
    // ------------------------------------------------
    //  Convenience Functions
    // ------------------------------------------------
    InstanceHandle GetPrevious() const
    {
        return InstanceHandle(handle_ - 1);
    }

    InstanceHandle GetNext() const
    {
        return InstanceHandle(handle_ + 1);
    }

    bool IsNil() const
    {
        return handle_ == HANDLE_NIL;
    }

public:
    // ------------------------------------------------
    //  operator manipulations
    // ------------------------------------------------

    // Pre-increment
    InstanceHandle& operator++()
    {
        this->handle_++;
        return *this;
    }

    // Post-increment
    InstanceHandle operator++(int)
    {
        InstanceHandle result(*this);
        ++(*this);
        return result;
    }

    // Pre-decrement
    InstanceHandle& operator--()
    {
        this->handle_--;
        return *this;
    }

    // Post-decrement
    InstanceHandle operator--(int)
    {
        InstanceHandle result(*this);
        --(*this);
        return result;
    }

    InstanceHandle& operator+=(const InstanceHandle &other)
    {
        this->handle_ = this->handle_ + other.handle_;
        return *this;
    }

    InstanceHandle operator+(const InstanceHandle &other) const
    {
        return InstanceHandle(*this) += other;
    }

public:
    // ------------------------------------------------
    //  operator comparisons
    // ------------------------------------------------
    bool operator==(const InstanceHandle &rvalue) const
    {
        return this->handle_ == rvalue.handle_;
    }

    bool operator!=(const InstanceHandle &rvalue) const
    {
        return this->handle_ != rvalue.handle_;
    }

    bool operator<(const InstanceHandle &rvalue) const
    {
        return this->handle_ < rvalue.handle_;
    }

    bool operator<=(const InstanceHandle &rvalue) const
    {
        return this->handle_ <= rvalue.handle_;
    }

    bool operator>(const InstanceHandle &rvalue) const
    {
        return this->handle_ > rvalue.handle_;
    }

    bool operator>=(const InstanceHandle &rvalue) const
    {
        return this->handle_ >= rvalue.handle_;
    }

public:
    friend std::ostream& operator<<(std::ostream &ostr, const BaseLib::InstanceHandle &handle)
    {
        ostr << handle.GetHandle();
        return ostr;
    }

public:
    // ------------------------------------------------
    //  static functions
    // ------------------------------------------------
    static InstanceHandle NIL()
    {
        static InstanceHandle handle(HANDLE_NIL);
        return handle;
    }

    static unsigned int SIZE()
    {
        static unsigned int elementSize = 4; // long = 4 bytes
        return elementSize;
    }

    static InstanceHandle GenerateNext(const InstanceHandle &prev)
    {
        InstanceHandle next(prev.GetNext());
        return next;
    }

    static InstanceHandle Create(InstanceHandleType handle)
    {
        return InstanceHandle(handle);
    }

private:
    InstanceHandleType handle_;
};

//typedef std::vector<InstanceHandleType> InstanceHandleTypeSeq;
typedef std::vector<InstanceHandle>     InstanceHandleSeq;
typedef std::set<InstanceHandle>        InstanceHandleSet;

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const BaseLib::InstanceHandleSeq &handleSeq);

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const BaseLib::InstanceHandleSet &handleSeq);

} // namespace BaseLib

//DLL_STATE inline std::ostream& operator<<(std::ostream &ostr, const BaseLib::InstanceHandle &handle)
//{
//	ostr << handle.GetHandle();
//	return ostr;
//}


#endif // BaseLib_InstanceHandle_h_IsIncluded
