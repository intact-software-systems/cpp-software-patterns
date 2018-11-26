/*
 * DomainId.h
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Elements_DomainId_h_IsIncluded
#define DDS_Elements_DomainId_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/CommonDefines.h"

#include"DDS/Export.h"

namespace DDS { namespace Elements
{
typedef DOMAINID_TYPE_NATIVE    DomainIdType;
const   DomainIdType 	        DOMAIN_NIL = HANDLE_NIL_NATIVE;

/* --------------------------------------------------------------
Type used to represent the identity of an application domain.
----------------------------------------------------------------*/
class DLL_STATE DomainId
        : public BaseLib::Templates::ComparableMutableType<DOMAINID_TYPE_NATIVE>
        , public NetworkLib::NetObjectBase
{
public:
    DomainId(DomainIdType handle = DOMAIN_NIL);
    virtual ~DomainId();

    /**
     * @brief Write
     * @param writer
     */
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32(this->delegate());
    }

    /**
     * @brief Read
     * @param reader
     */
    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        this->Set(reader->ReadInt32());
    }

    // ------------------------------------------------
    //  Convenience Functions
    // ------------------------------------------------

    bool IsNil() const
    {
        return this->delegate() == DOMAIN_NIL;
    }

    // ------------------------------------------------
    //  static functions
    // ------------------------------------------------
    static DomainId NIL()
    {
        static DomainId handle(DOMAIN_NIL);
        return handle;
    }

    static unsigned int SIZE()
    {
        static unsigned int elementSize = 4; // long = 4 bytes
        return elementSize;
    }

    // ------------------------------------------------
    //  friend functions
    // ------------------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const DomainId &t)
    {
        ostr << t.delegate();
        return ostr;
    }
};

}}

#endif
