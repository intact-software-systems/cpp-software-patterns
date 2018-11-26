#include "BaseLib/CommonClasses.h"

namespace BaseLib
{

std::ostream& operator<<(std::ostream &ostr, const ClientTriplet &triplet)
{
	ostr << "(" << triplet.GetHost() << "," << triplet.GetPort() << "," << triplet.GetSocket() << ")";
	return ostr;
}

ClientTriplet::ClientTriplet() : socket_(0), port_(0)
{

}
ClientTriplet::ClientTriplet(int socket, int port, std::string host)
	: socket_(socket)
	, port_(port)
	, host_(host)
{

}

ClientTriplet::~ClientTriplet()
{

}

bool ClientTriplet::operator<(const ClientTriplet &other) const
{
	return socket_ < other.GetSocket() ||
		(socket_ == other.GetSocket() && port_ < other.GetPort()) ||
		(socket_ == other.GetSocket() && port_ == other.GetPort() && host_.length() < other.GetHost().length());
}

bool ClientTriplet::operator==(const ClientTriplet &other) const
{
	if(host_ == other.GetHost() &&
			port_ == other.GetPort() &&
			socket_ == other.GetSocket())
		return true;

	return false;
}

std::string ClientTriplet::GetHost()	const { return host_; }
int			ClientTriplet::GetPort()	const { return port_; }
int			ClientTriplet::GetSocket()	const { return socket_; }

} // namespace BaseLib

