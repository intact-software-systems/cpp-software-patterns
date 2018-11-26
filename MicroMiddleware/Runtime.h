#ifndef MicroMiddleware_Runtime_h_IsIncluded
#define MicroMiddleware_Runtime_h_IsIncluded

#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE Runtime
{
private:
	static InterfaceHandle lookupServerHandle;
	static InterfaceHandle localHostHandle;
	static HostInformation componentHostInformation;

public:
	/**
	 * True if middleware has been initialized, false if otherwise.
	 */
	static bool Initialized;

	/**
	 * A static method that must be called by an application before using the 
	 * LightweightMiddleware.
	 */
	static void Initialize();

	/**
	 * A static method that must be called by an application before using the 
	 * LightweightMiddleware.
	 */
	static void Initialize(int arg, char* argv[]);

	/**
	 * A static method that must be called by an application or DLL before shutting down
	 * in order to free resources used by the LightweightMiddleware.
	 *
	 * @param maxWaitCounter    number of quarter seconds to wait before exiting
	 */
	static void Release(int maxWaitCounter=40);

	/**
	 * Check if command line arguments contains LookupServer info.
	 *
	 * @param argc       the number of arguments on the command line for the process
	 * @param argv       an array of strings containing the command line arguments
	 * @return           true if LookupServer arguments are parts of the command line arguments
	 */
	static bool HasLookupServerInfo(int argc, char* argv[]);

	/**
	 * Extract the InterfaceHandle to the LookupServer
	 *
	 * @param argc       the number of arguments on the command line for the process (including the file name of the process)
	 * @param argv       an array of strings containing the command line arguments
	 * @return           the LookupServer InterfaceHandle
	 */
	static InterfaceHandle GetLookupServerHandle(int argc, char* argv[]);

	/**
	 * Return the static private InterfaceHandle Runtime::lookupServerHandle to the LookupServer.
	 *
	 * @return           the LookupServer InterfaceHandle
	 */
	static InterfaceHandle GetLookupServerHandle();		

	/**
	 * Return the static private InterfaceHandle Runtime::settingsManagerHandle to the SettingsManager.
	 *
	 * @return           the SettingsManager InterfaceHandle
	 */
	static InterfaceHandle GetSettingsManagerHandle();

	/**
	 * Extract the HostInformation from file-name retrieved from GetInstanceName
	 *
	 * @param componentName		The component's name
	 * @return					The component's HostInformation
	 */
	static HostInformation GetHostInformation(string instanceName);

	/**
	 * Extract the HostInformation from arguments, and initializes the static InterfaceHandle localHostHandle,
	 * and the static HostInformation componentHostInformation.
	 *
	 * @param argc       the number of arguments on the command line for the process (including the file name of the process)
	 * @param argv       an array of strings containing the command line arguments
	 * @return			The component's HostInformation
	 */
	static HostInformation GetHostInformation(int argc, char* argv[]);

	/**
	 * Return the static private HostInformation Runtime::componentHostInformation to the Component.
	 *
	 * @return           the HostInformation for the Component.
	 */
	static HostInformation GetHostInformation();

	static std::string GetComponentName();

	/**
	 * After logging in to the MembershipManager update Runtime's static HostInformation.
	 *
	 * @param hostInfo	 The current running process's HostInformation.
	 */
	static void SetHostInformation(HostInformation hostInfo);

	/**
	 * Extract the static InterfaceHandle localHostHandle.
	 *
	 * @return			The component's HostInformation
	 */
	static InterfaceHandle GetInterfaceHandle();

	/**
	 * Extract the instance name from the command line arguments.
	 *
	 * @param argc       the number of arguments on the command line for the process (including the file name of the process)
	 * @param argv       an array of strings containing the command line arguments
	 * @return           the name of the instance
	 */
	static string GetInstanceName(int argc, char* argv[]);

	/**
	 * This method make the current process shutdown in an orderly fashion. Typically used when detecting
	 * a fatal error within another thread than the main thread. Currently the effect of this method
	 * is the same as sending the process a SIGTERM signal.
	 */
	static void ShutDown();
};

} // namespace MicroMiddleware


#endif


