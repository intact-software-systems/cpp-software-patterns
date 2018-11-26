#ifndef ProcessFactoryInterface_h_IS_INCLUDED
#define ProcessFactoryInterface_h_IS_INCLUDED

#include "IntactMiddleware/IncludeExtLibs.h"

using namespace std;

#include "IntactMiddleware/Export.h"
namespace IntactMiddleware
{
	/**
	* An interface to the ProcessFactory, which is used by the SystemManager.
	*/
    class DLL_STATE ProcessFactoryInterface : public RmiInterface
	{
	public:

		virtual ~ProcessFactoryInterface() {}

		/**
		* Constants used to identify each mathod provided by this interface
		* They have to be unique within each interface. Values < 10 are reserved.
		*/
		enum MethodID {
			Method_CreateComponent = 10,
			Method_ComponentCreated = 11,
			Method_KillAll = 12,
			Method_GetComponent = 13,
			Method_KillComponent = 14,
			Method_CreateHostComponent = 15,
			Method_IsComponentRunning = 16,
			Method_GetProcessResources = 17,
			Method_GetNodeResources = 18,
			Method_GetAllComponentResources = 19,
			Method_GetMapComponentResources = 20,
			Method_CreateExtHostComponent = 21
		};

		static string InterfaceID;
	
		enum InterfaceId
		{
			ProcessFactoryInterfaceId = 177
		};
		
		virtual int GetInterfaceId() const { return ProcessFactoryInterface::ProcessFactoryInterfaceId; }


		/**
		* Creates a component.
		*
		* @param componentType		the type of the component, typically "BeamformerManager"  
		* @param instanceName		the name of the new instance, typically "BeamformerManager2"  
		* @return InterfaceHandle	The InterfaceHandle of the created component.
		*/
		virtual InterfaceHandle CreateComponent(const string& componentType, const string& instanceName) = 0;

		/**
		* Starts a component by creating a new process using the hostInformation. Called by SystemManager.
		*
		* @param hostInformation	Contains the information needed to create the component. 
		* @param lookupServer		The LookupServer information.
		* @return					String containing (potential) error messages.
		*/
		virtual string CreateComponent(HostInformation &hostInformation, HostInformation lookupServer) = 0;


		/**
		* Starts a component by creating a new process using the hostInformation. Called by Components.
		*
		* @param hostInformation	Contains the information needed to create the component. 
		* @param lookupServer		The LookupServer information.
		* @return					String containing (potential) error messages.
		*/
		virtual string CreateExtComponent(HostInformation &hostInformation, HostInformation lookupServer) = 0;

		/**
		* Checks if a component is still running.
		*
		* @param hostInformation		The HostInformation for the component to check the running state.
		* @return int					1 if running, 0 if not running.
		*/
		virtual int IsComponentRunning(HostInformation hostInformation) = 0;

		/**
		* Returns a handle to the given component.  If the component does not exist, an exception will be thrown.
		*
		* @param componentType  the type of the component, typically "BeamformerManager"  
		* @param instanceName   the name of the new instance, typically "BeamformerManager2"  
		*/
		virtual InterfaceHandle GetComponent(const string& componentType, const string& instanceName) = 0;

		/**
		* Returns the resources the given processId uses on the Component Factory host.
		*
		* @param componentName		Name of component to retrieve consumed resources.
		* @return HostResources		The current resource consumption for componentName.
		*/
		virtual HostResources GetProcessResources(string componentName) = 0;

		/**
		* Returns the resource consumption on the Component Factory host.
		*
		* @return HostResources		The resource consumption on the host.	
		*/
		virtual HostResources GetNodeResources() = 0;

		/**
		* Returns the resource consumption for all processes and the node. 
		*
		* @return string		String containing all resources for all components. 
		*/
		virtual string GetAllComponentResources() = 0;

		/**
		* Returns a map (pid, HostResources) of all resource information on the ProcessFactory.
		*
		* @return map<int, HostResources>	Map of all resource information (pid, HostResources).
		*/
		virtual map<int, HostResources> GetMapComponentResources() = 0;

		/**
		* This method is called by the processing hosting the component in order to notify this factory
		* about the creation of the component.
		*
		* @param hostName			the name of the computer hosting the component
		* @param mainStubPort		the port number of the stub belonging to the main interface of the component 
		* @param processID			the corresponding process ID
		* @param instanceName		the name of the instance
		* @param processFileName	the name of the process
		* @param instanceId			Used to identify the newly created component.
		*/
		virtual void ComponentCreated(
			const string& hostName, 
			int mainStubPort, 
			int processId, 
			const string& instanceName, 
			const string& processFileName,
			int instanceId) = 0;

		/**
		* Kills all instances created by this factory.
		*
		* @return int		Always returns 1.
		*/
		virtual int KillAll() = 0;

		/**
		* Kills a component created by this factory.
		*
		* @return int		1 if success, 0 if failed.
		*/
		virtual int KillComponent(string componentName) = 0;

		/**
		* This interface major version. Should be changed when methods provided by
		* this interface are removed.
		*/
		const static int MajorVersion = 1;

		/**
		* This interface minor version. Should be changed when methods provided by
		* this interface are added.
		*/
		const static int MinorVersion = 0;
	};

    /**
    * TODO class-description
    */
    class DLL_STATE ProcessFactoryProxy : public RmiClient, public ProcessFactoryInterface
	{
	public:

		RMICLIENT_COMMON(ProcessFactoryProxy);

		virtual InterfaceHandle			CreateComponent(const string& componentType, const string& instanceName);
		virtual InterfaceHandle			GetComponent(const string& componentType, const string& instanceName);
		virtual int						KillAll();
		virtual int						KillComponent(string componentName);
		virtual string					CreateComponent(HostInformation &hostInformation, HostInformation lookupServer);
		virtual string					CreateExtComponent(HostInformation &hostInformation, HostInformation lookupServer);
		virtual int						IsComponentRunning(HostInformation hostInformation);
		virtual HostResources			GetProcessResources(string componentName);
		virtual HostResources			GetNodeResources();
		virtual string					GetAllComponentResources();
		virtual map<int, HostResources>	GetMapComponentResources();

		virtual void ComponentCreated(
			const string& hostName, 
			int mainStubPort, 
			int processId,
			const string& instanceName, 
			const string& processFileName,
			int instanceId);

		/**
		* Returns the minor version of this interface.
		*
		* @return   minor version
		*/
		virtual int GetMinorVersion() const {
			return ProcessFactoryInterface::MinorVersion; 
		};

		/**
		* Returns the major version of this interface.
		*
		* @return   major version
		*/
		virtual int GetMajorVersion() const {
			return ProcessFactoryInterface::MajorVersion; 
		};

	};

    /**
    * TODO class-description
    */
    class DLL_STATE ProcessFactoryStub : public RmiServerListener
    {
    private:
        ProcessFactoryInterface* componentFactoryInterface;

    protected:
        virtual void RMInvocation(int methodID, RmiBase* stubClient);

    public:
        ProcessFactoryStub(const InterfaceHandle &handle, ProcessFactoryInterface* componentFactoryInterface, bool autoStart = true);
		virtual ~ProcessFactoryStub() {
		}
        
        virtual std::string& GetInterfaceID() const {
            return ProcessFactoryInterface::InterfaceID;
        }

		/**
		* Returns the minor version of this interface.
		*
		* @return   minor version
		*/
		virtual int GetMinorVersion() const {
			return ProcessFactoryInterface::MinorVersion; 
		};

		/**
		* Returns the major version of this interface.
		*
		* @return   major version
		*/
		virtual int GetMajorVersion() const {
			return ProcessFactoryInterface::MajorVersion; 
		};

    };


}

#endif


