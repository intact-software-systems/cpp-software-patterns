#ifndef IntactInfra_ConfigurationFile_h_IsIncluded
#define IntactInfra_ConfigurationFile_h_IsIncluded

#include <vector>
#include <iostream>
#include <list>
#include <map>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include"IntactInfra/Export.h"

namespace IntactInfra {

    /**
    * A configuration node is a key/value map for an XML node.
    * So if you have a tag like this: <example a="1" b="2" c="3"/>
    * the corresponding ConfigurationNode will look like this:
    *
    *    ConfigurationNode example;
    *    example["a"] = "1";
    *    example["b"] = "2";
    *    example["c"] = "3";
    */
    typedef std::map<std::string, std::string> ConfigurationNode;

    /**
    * A configuration node list is a standard C++ list of
    * ConfigurationNode objects.
    */
    typedef std::list<ConfigurationNode> ConfigurationNodeList;

   /**
    * A DOMElementList is a list of pointers to DOMElement objects.
    * It is used when retreiving nodes matching a given path.
    */
    typedef std::list<xercesc::DOMElement*> DOMElementList;

    /**
    * @brief XML configuration file reader.
    *
    * The Configuration class is used to access XML configuration files.
    * It takes paths to an XML element, and returns its attributes as a
    * standard C++ std::map.
    *
    * Simple example of how to use this:
    *
    *     using namespace IntactInfra;
    *
    *     ConfigurationFile configFile("Test.xml");
    *     ConfigurationNode node =
    *         configFile.getConfigurationNode("Test/example");
    *     std::cout << node["config"] << endl;
    *
    * This will print out "success", given the following XML file:
    *
    *     <Test><example config="success"/></Test>
    */
    class DLL_STATE ConfigurationFile {

    public:

        /**
        * Creates an empty ConfigurationFile instance. You may fill this
        * later by calling loadFile(). Until then, every call to
        * getConfigurationNodes() and getConfigurationNode() will return
        * empty C++ maps.
        */
        ConfigurationFile();

        /**
        * Creates an ConfigurationFile instance loaded with the content of
        * the given configuration file. Throws an exception if the file
        * doesn't exist or can't be parsed.
        *
        * @param fileName path to the XML configuration file
        */
        ConfigurationFile(const std::string& fileName);

        ~ConfigurationFile();

        /**
        * Loads the content of the given configuration file. This will only
        * work once, and subsequent calls will result in an exception. To
        * load another configuration file, just make a new object.
        * Also throws an exception if the file doesn't exist or can't be
        * parsed.
        *
        * @param fileName path to the XML configuration file
        */
        void loadFile(const std::string& fileName);

        /**
        * Saves the current in-memory configuration file to a disk file.
        * Used together with addConfigurationNode(), this can be used to
        * write XML files.
        *
        * @param fileName path to where the new XML configuration file
        *                 is written
        */
        void saveFile(const std::string& fileName, xercesc::DOMElement* currentNode = NULL);


		/**
        * Saves the content in string xmlContent to a temporary file, and then 
		* loads the configuration file. This will only
        * work once, and subsequent calls will result in an exception. To
        * load another configuration file, just make a new object.
        * Also throws an exception if the file doesn't exist or can't be
        * parsed.
        *
        * @param xmlContent		xml code
        */
		void loadFileFromString(const std::string& xmlContent);


		/**
		* Writes the xml-code identified by DOMElement* currentNode to a string,
		* and returns it.
		*
		* @return string		The xml-code for the DOMELement*
		*/
		std::string writeToString(xercesc::DOMElement* currentNode = NULL);


		/**
		* Returns a ConfigurationNode that contains the (attribute,value) pairs that
		* are identified by DOMElement* currentNod.
        *
        * @param xercesc::DOMElement*		element that contains the xml-code.
        * @return ConfigurationNode			containing (attribute,value) pairs
        */
		ConfigurationNode getConfigurationNode(xercesc::DOMElement* currentNode) const;


		/**
		* Use DOMElements hisCurrentNode to add or update namespaces and (attribute,value) pairs
		* of this ConfigurationFile object. Example:
		*
		* hisCurrentNode represents:
		* <Test>
		*	<example id="d"/>
		* </Test>
		*
		* thisCurrentNode represents:
		* <Test>
		*	<example id2="d"/>
		*   <foo bar="blid" />
		* </Test>
		*
		* Result after addOrUpdate(hisCurrentNode, thisCurrentNode):
		* <Test>
		*	<example id="d" id2="d"/>
		*   <foo bar="blid" />
		* </Test>
		*
		* @param hisCurrentNode		DOMElement* from an external/other ConfigurationFile object.
		* @param thisCurrentNode	DOMElement* from this ConfigurationFile object.
		*/
		void addOrUpdate(xercesc::DOMElement* hisCurrentNode, xercesc::DOMElement* thisCurrentNode);


        /**
        * Returns list of all nodes matching the given path. This is used
        * if you have several nodes with the same tag name, like this:
        *
        *    <Test>
        *      <example id="a"/>
        *      <example id="b"/>
        *      <example id="c"/>
        *    </Test>
        *
        * Use getConfigurationNodes("Test/example") to return the three nodes.
        * If no nodes are found, an empty list will be returned.
        *
        * @param path full path to nodes
        * @return list of matching nodes, or empty list if none found
        */
        ConfigurationNodeList
        getConfigurationNodes(const std::string& path,
                              xercesc::DOMElement* currentNode = NULL) const;

        /**
        * Appends a new configuration node in a specified path. Given the
        * following XML file ..
        *
        *    <Test>
        *      <example id="a"/>
        *      <example id="b"/>
        *    </Test>
        *
        * .. and the following code ..
        *
        *    ConfigurationFile cfgFile("Test.xml");
        *    ConfigurationNode newNode;
        *    newNode["id"] = "c";
        *    cfgFile.addConfigurationNode("Test/example", &newNode);
        *    cfgFile.saveFile("Test.xml");
        *
        * .. we will get this XML file ..
        *
        *    <Test>
        *      <example id="a"/>
        *      <example id="b"/>
        *      <example id="c"/>
        *    </Test>
        *
        * @param path where we will append the new configuration node
        * @param newNode the new configuration node. If not specified, an
        *                empty (i.e. no attributes) configuration node will
        *                be appended.
        */
        void addConfigurationNode(const std::string& path,
                                  ConfigurationNode* newNode = NULL);

        /**
        * Returns list of all nodes matching the given path.
        *
        * @param path full path to nodes
        * @return list of matching nodes, or empty list if none found
         */
        DOMElementList getDOMElements(const std::string& path, xercesc::DOMElement* currentNode=NULL) const;

        /**
        * Returns the first node matching the given path. If the path is
        * not found in the loaded XML file, or no XML has been loaded yet,
        * an empty map is returned. Use the hasConfigurationNode() to make
        * the distinction between an empty tag (e.g. <example/>), and one
        * that's missing.
        *
        * @param path full path to node
        * @return first matching node, or an empty map if not found
        */
        ConfigurationNode getConfigurationNode(const std::string& path, xercesc::DOMElement* currentNode=NULL) const;

        /**
        * Returns true if at least one node matches the given path.
        *
        * @param path full path to node
        * @return true if matching node exists, false otherwise
        */
        bool hasConfigurationNode(const std::string& path, xercesc::DOMElement* currentNode=NULL) const;

#ifdef DISABLED
        /**
        * Returns the value of the given parameter in the first node matching path. If no node matches
        * the given path or the desired node does not contain a value for the parameter a default value 
        * is returned instead.
        *
        * @param nodeName full path to node
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of first matching node or a defaultValue
        */
        std::string stringValue(const char* nodeName, const char* paramName, const std::string defaultValue = std::string()) const;

        /**
        * Returns the value of the given parameter in the first node matching path. If no node matches
        * the given path or the desired node does not contain a value for the parameter a default value 
        * is returned instead.
        *
        * @param nodeName full path to node
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of first matching node or a defaultValue
        */
        double doubleValue(const char* nodeName, const char* paramName, double defaultValue) const;

        /**
        * Returns the value of the given parameter in the first node matching path. If no node matches
        * the given path or the desired node does not contain a value for the parameter a default value 
        * is returned instead.
        *
        * @param nodeName full path to node
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of first matching node or a defaultValue
        */
        int intValue(const char* nodeName, const char* paramName, int defaultValue) const;

        /**
        * Returns the value of the given parameter in the first node matching path. If no node matches
        * the given path or the desired node does not contain a value for the parameter a default value 
        * is returned instead.
        *
        * The following parameter values give "true" as the returned value: true, on or yes.
        *
        * @param nodeName full path to node
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of first matching node or a defaultValue
        */
        bool boolValue(const char* nodeName, const char* paramName, bool defaultValue) const;

#endif

        /**
        * Returns the values of a parameter as a vector. White space is used to seperate items from
        * the attribute value. I.e. param="abc def" into "abc", "def".
        */
		static void stringList(const ConfigurationNode& configNode, const char* paramName, std::vector<std::string>& container);

        /**
        * Returns the values of a parameter as a vector of doubles.
        */
		static void doubleList(const ConfigurationNode& configNode, const char* paramName, std::vector<double>& container);

        /**
        * Returns the values of a parameter as a vector of floats.
        */
		static void floatList(const ConfigurationNode& configNode, const char* paramName, std::vector<float>& container);

        /**
        * Returns the values of a parameter as a vector of ints.
        */
		static void intList(const ConfigurationNode& configNode, const char* paramName, std::vector<int>& container);

        /**
        * Returns the values of a parameter as a vector of bools.
        */
		static void boolList(const ConfigurationNode& configNode, const char* paramName, std::vector<bool>& container);

		/**
        * Returns the values of a parameter as a vector. White space is used to seperate items from
        * the attribute value. I.e. param="abc def" into "abc", "def".
        */
        void stringList(const char* nodeName, const char* paramName, std::vector<std::string>& container, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the values of a parameter as a vector of doubles.
        */
        void doubleList(const char* nodeName, const char* paramName, std::vector<double>& container, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the values of a parameter as a vector of floats.
        */
        void floatList(const char* nodeName, const char* paramName, std::vector<float>& container, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the values of a parameter as a vector of ints.
        */
        void intList(const char* nodeName, const char* paramName, std::vector<int>& container, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the values of a parameter as a vector of bools.
        */
        void boolList(const char* nodeName, const char* paramName, std::vector<bool>& container, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the value of the given parameter in the first child node of the DOMElement that matches the given path.
        */
        std::string stringValue(const char* nodeName, const char* paramName, const std::string defaultValue = std::string(), xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the value of the given parameter in the first child node of the DOMElement that matches the given path.
        */
        double doubleValue(const char* nodeName, const char* paramName, double defaultValue, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the value of the given parameter in the first child node of the DOMElement that matches the given path.
        */
        int intValue(const char* nodeName, const char* paramName, int defaultValue, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the value of the given parameter in the first child node of the DOMElement that matches the given path.
        */
        bool boolValue(const char* nodeName, const char* paramName, bool defaultValue, xercesc::DOMElement* startNode=NULL) const;

        /**
        * Returns the value of the given parameter in node. If the node does not contain 
        * a value for the parameter a default value is returned instead.
        *
        * @param configNode conguration node to fetch value from
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of parameter in node or a defaultValue
        */
        static std::string stringValue(const ConfigurationNode& configNode, const char* paramName, const std::string defaultValue = std::string());

        /**
        * Returns the value of the given parameter in node. If the node does not contain 
        * a value for the parameter a default value is returned instead.
        *
        * @param configNode conguration node to fetch value from
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of parameter in node or a defaultValue
        */
        static int intValue(const ConfigurationNode& configNode, const char* paramName, int defaultValue);

        /**
        * Returns the value of the given parameter in node. If the node does not contain 
        * a value for the parameter a default value is returned instead.
        *
        * The following parameter values give "true" as the returned value: true, on or yes.
        *
        * @param configNode conguration node to fetch value from
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of parameter in node or a defaultValue
        */
        static bool boolValue(const ConfigurationNode& configNode, const char* paramName, bool defaultValue);

        /**
        * Returns the value of the given parameter in node. If the node does not contain 
        * a value for the parameter a default value is returned instead.
        *
        * @param configNode conguration node to fetch value from
        * @param paramName name of desired attribute
        * @param defaultValue default value to be used if configuration does not contain a value
        * @return value of parameter in node or a defaultValue
        */
        static double doubleValue(const ConfigurationNode& configNode, const char* paramName, double defaultValue);


    private:

        void initXML();
        XMLCh* toXMLCh(const char *s) const;
        XMLCh* toXMLCh(const std::string& s) const;
        std::string fromXMLCh(const XMLCh* s) const;
        bool lookup(ConfigurationNodeList* resultSet,
                    xercesc::DOMElement* currentNode,
                    const std::string& path) const;

        bool lookup(DOMElementList* resultSet,
                    xercesc::DOMElement* currentNode,
                    const std::string& path) const;

        std::string pathSeparator;
        xercesc::DOMImplementation* domImpl;
    //#ifdef XERCES_3
        xercesc::DOMLSParser* domParser;
    //#else
     //   xercesc::DOMBuilder* domParser;
    //#endif
        xercesc::DOMDocument* xmlDocument;

    };
}

#endif
