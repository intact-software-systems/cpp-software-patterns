#ifndef BaseLib_Templates_CountingHandle_h_Included
#define BaseLib_Templates_CountingHandle_h_Included

#include"BaseLib/InstanceHandle.h"
#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Templates/Lifecycle.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The Reference class
 *
 * Only allow objects on the heap
 *
 * TODO: - Implement "Counting Handle" reference counter
 *       - This is incomplete and doesn't work.
 */
class Reference : public DisposeMethod
{
public:
    Reference()
        : handle_(new InstanceHandle(0))
    {}
    Reference(const Reference &ref)
        : handle_(ref.handle_)
    {
        ++handle_;
    }

private:
    ~Reference()
    {
        if(--handle_ == 0)
        {
            //Dispose();
        }
    }

private:
    InstanceHandle *handle_;
};

}}

#endif
