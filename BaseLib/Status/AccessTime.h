#pragma once

#include"BaseLib/Timestamp.h"
#include"BaseLib/Templates/MethodInterfaces.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status {

class DLL_STATE AccessTime
    : public ::BaseLib::Templates::ModifyMethod
    , public ::BaseLib::Templates::NotModifyMethod
    , public ::BaseLib::Templates::ReadMethod
    , public ::BaseLib::Templates::StartMethod
{
public:
    AccessTime();
    virtual ~AccessTime();

    // ------------------------------------------------------
    // Interfaces
    // ------------------------------------------------------

    virtual void Modify();
    virtual void NotModify();
    virtual void Read();
    virtual void Start();

    // ------------------------------------------------------
    // Access absolute timestamps
    // ------------------------------------------------------

    const Timestamp& CreatedAt() const;
    const Timestamp& ModifiedAt() const;
    const Timestamp& ReadAt() const;
    const Timestamp& WrittenAt() const;
    const Timestamp& StartedAt() const;
    const Timestamp& LastAccessedAt() const;

    // ------------------------------------------------------
    // Compute time since a point in time
    // ------------------------------------------------------

    Duration TimeSinceCreated() const;
    Duration TimeSinceModified() const;
    Duration TimeSinceRead() const;
    Duration TimeSinceAccessed() const;
    Duration TimeSinceStarted() const;

private:
    Timestamp createdTime_;
    Timestamp startTime_;
    Timestamp modifiedTime_;
    Timestamp readTime_;
};

}}
