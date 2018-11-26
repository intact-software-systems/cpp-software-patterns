#ifndef BaseLib_Runnable_h_IsIncluded
#define BaseLib_Runnable_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib {

/**
 * @brief The Runnable class is input to a ThreadPool that executes it based on given policies.
 */
class DLL_STATE Runnable
{
public:
    virtual ~Runnable()
    {}

    CLASS_TRAITS(Runnable)

    virtual void run() = 0;

    virtual int GetId() const
    {
        return 0;
    }

    virtual std::string GetName() const
    {
        return std::string();
    }
};

}

#endif

