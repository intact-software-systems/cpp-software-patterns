#ifndef BaseLib_CommonClasses_h_Included
#define BaseLib_CommonClasses_h_Included

#include <stdlib.h>
#include <string>
#include <ostream>
//#include <cstdlib>

#include "BaseLib/Export.h"

namespace BaseLib
{
class DLL_STATE ClientTriplet
{
public:
    ClientTriplet();
    ClientTriplet(int socket, int port, std::string host);
    virtual ~ClientTriplet();

    bool operator<(const ClientTriplet &other) const;
    bool operator==(const ClientTriplet &other) const;

    std::string	GetHost() const;
    int			GetPort() const;
    int			GetSocket() const;

private:
    int socket_;
    int port_;
    std::string host_;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const ClientTriplet &triplet);

} // namespace BaseLib

#endif  // BaseLib_CommonClasses_h_Included


