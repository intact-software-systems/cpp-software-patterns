#ifndef DDS_Publication_PublisherQoS_h_IsIncluded
#define DDS_Publication_PublisherQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS
{

class DLL_STATE PublisherQoS
{
public:
    PublisherQoS() {}
//	PublisherQoS(const PublisherQoS& other);

    PublisherQoS(const DDS::Policy::Presentation& presentation,
                 const DDS::Policy::Partition& partition,
                 const DDS::Policy::GroupData& groupData,
                 const DDS::Policy::EntityFactory& factoryPolicy);

public:
    template <typename POLICY>
    const POLICY& policy() const;

    template <typename POLICY>
    POLICY& policy();

public:
    void policy(const DDS::Policy::Presentation& presentation);
    void policy(const DDS::Policy::Partition& partition);
    void policy(const DDS::Policy::GroupData& groupData);
    void policy(const DDS::Policy::EntityFactory& factoryPolicy);


public:
    /**
     * @brief Default
     * @return
     */
    static PublisherQoS Default()
    {
        return PublisherQoS();
    }

private:
    DDS::Policy::Presentation    presentation_;
    DDS::Policy::Partition       partition_;
    DDS::Policy::GroupData       groupData_;
    DDS::Policy::EntityFactory   factoryPolicy_;
};

}

#endif
