#ifndef DDS_Elements_TopicKey_h_IsIncluded
#define DDS_Elements_TopicKey_h_IsIncluded

#include"DDS/CommonDefines.h"
#include"DDS/Export.h"

namespace DDS { namespace Elements
{

/**
 * @brief TopicKeyType
 */
typedef std::vector<long> TopicKeyType;

/**
 * @brief The TopicKey class is a DDS topic key to distinguish entries.
 */
class DLL_STATE TopicKey : public BaseLib::Templates::ComparableImmutableType< DDS::Elements::TopicKeyType >
{
public:
    TopicKey();
    TopicKey(DDS::Elements::TopicKeyType key);
    TopicKey(const BaseLib::InstanceHandle &handle);

    virtual ~TopicKey();

    friend std::ostream& operator <<(std::ostream &ostr, const TopicKey &key)
    {
        bool isFirst = true;

        for(std::vector<long>::const_iterator it = key.delegate().begin(), it_end = key.delegate().end(); it != it_end; ++it)
        {
            if(isFirst) ostr << "(" << *it;
            else ostr << "," << *it;

            isFirst = false;
        }

        if(!key.delegate().empty())
            ostr << ")";

        return ostr;
    }
};

}}

#endif
