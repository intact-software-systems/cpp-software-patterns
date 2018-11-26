#ifndef BaseLib_Status_AccessCount_h_Included
#define BaseLib_Status_AccessCount_h_Included

#include"BaseLib/Count.h"
#include"BaseLib/Templates/MethodInterfaces.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

class DLL_STATE AccessCount
        : public ::BaseLib::Templates::ModifyMethod
        , public ::BaseLib::Templates::NotModifyMethod
        , public ::BaseLib::Templates::ReadMethod
{
public:
    AccessCount();
    virtual ~AccessCount();

    virtual void Read();
    virtual void NotModify();
    virtual void Modify();

    const Count &NumRead() const;
    const Count &NumModified() const;
    Count NumAccessed() const;

private:
    Count readCount_;
    Count modifiedCount_;
};

}}

#endif
