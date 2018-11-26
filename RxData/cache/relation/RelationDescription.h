#ifndef RxData_Cache_RelationDescription_h_IsIncluded
#define RxData_Cache_RelationDescription_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/relation/RelationKind.h"
#include"RxData/cache/relation/RelationName.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * @author KVik
 */
class DLL_STATE RelationDescription
{
private:
    RelationKind::Type kind_;
    RelationName name_;

public:
    /**
     * @param kind
     * @param name
     */
    RelationDescription(RelationKind::Type kind, RelationName name)
        : kind_(kind)
        , name_(name)
    { }

    virtual ~RelationDescription()
    {}

    const RelationKind::Type& Kind() const
    {
        return kind_;
    }

    void SetKind(const RelationKind::Type &kind)
    {
        kind_ = kind;
    }
};

}

#endif

