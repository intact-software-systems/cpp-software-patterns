#ifndef DCPS_Factory_DCPSRuntime_h_IsIncluded
#define DCPS_Factory_DCPSRuntime_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"
namespace DCPS
{

/**
 * @brief The DCPSRuntime class
 */
class DLL_STATE DCPSRuntime : protected BaseLib::Templates::Lockable<Mutex>
{
private:
    DCPSRuntime();
    virtual ~DCPSRuntime();

    friend class Singleton<DCPSRuntime>;

public:
    /**
     * @brief Instance
     * @return
     */
    static DCPSRuntime& Instance();

    /**
     * @brief Initialize
     * @return
     */
    static bool Initialize();

private:
    /**
     * @brief instance_
     */
    static Singleton<DCPSRuntime> instance_;

    /**
     * @brief isInitialized_
     */
    bool isInitialized_;
};

}

#endif
