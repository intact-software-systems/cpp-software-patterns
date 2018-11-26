#include "IntactInfra/ConfigurationFile.h"
#include "IntactInfra/IncludeExtLibs.h" 
#include <fstream>
#include <set>
#include <wchar.h>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

namespace IntactInfra
{

using namespace std;
using namespace xercesc;

template <class ContainerT>
class Tokenizer 
{
public:
    static void tokenize(const std::string& str, ContainerT& tokens, const std::string& delimiters = " ", const bool trimEmpty = false) 
    { 
        std::string::size_type pos, lastPos = 0; 
        while(true) 
        { 
            pos = str.find_first_of(delimiters, lastPos); 
            if(pos == std::string::npos) 
            { 
                pos = str.length(); 

                if(pos != lastPos || !trimEmpty) 
                    tokens.push_back(typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos-lastPos )); 

                break; 
            } 
            else 
            { 
                if(pos != lastPos || !trimEmpty) 
                    tokens.push_back(typename ContainerT::value_type(str.data()+lastPos, (typename ContainerT::value_type::size_type)pos-lastPos )); 
            } 

            lastPos = pos + 1; 
        }
    }
}; 

typedef vector<string> StringVector;
typedef Tokenizer<StringVector> StringTokenizer;


ConfigurationFile::ConfigurationFile()
{
    initXML();
}


ConfigurationFile::ConfigurationFile(const string& fileName)
{
    initXML();
    loadFile(fileName);
}


void ConfigurationFile::initXML()
{
    pathSeparator = "/";
    XMLPlatformUtils::Initialize();
    XMLCh* tmp = toXMLCh("LS");
    domImpl = (DOMImplementation*)
        DOMImplementationRegistry::getDOMImplementation(tmp);
    delete[] tmp;
#if 1 //XERCES_3
    domParser =
        domImpl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);
#else
   domParser =
       domImpl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);
#endif
    xmlDocument = NULL;
}


ConfigurationFile::~ConfigurationFile()
{
    if (domParser)
        domParser->release();
    XMLPlatformUtils::Terminate();
}


void ConfigurationFile::loadFile(const string& fileName)
{
    if (xmlDocument)
        throw runtime_error("Configuration file already loaded");

#if 1// USE_GCC
	IDEBUG() << "WARNING!!!! Should house ChangeDirectorySeparator!!!";
	string nativeName = fileName;
	//IDEBUG() << " file : " << nativeName;

	//string nativeName = FileStream::ChangeDirectorySeparator(fileName, '/');
#else
    string nativeName = FileStream::ChangeDirectorySeparator(fileName, '\\');
#endif
	
	xmlDocument = domParser->parseURI(nativeName.c_str());
    if (! xmlDocument) {
        cerr << "Unable to parse configuration file '" << fileName << "'!"
             << endl;
        throw runtime_error("Unable to load configuration file");
    }
}

void ConfigurationFile::loadFileFromString(const string& xmlContent)
{
    if (xmlDocument)
        throw runtime_error("Configuration file already loaded");
#if 1
	try
	{
		xercesc::MemBufInputSource *source = new MemBufInputSource((const XMLByte*)xmlContent.c_str(), strlen(xmlContent.c_str()), "xml-content", false);
		Wrapper4InputSource *wrapper = new Wrapper4InputSource(source, false);

		IDEBUG() << "WARNING!!!! xmlDocument = domParser->parse(wrapper); may fail!!!";
		xmlDocument = domParser->parse(wrapper);
		delete source;
		delete wrapper;
	}
	catch(const DOMException &toCatch)
	{
		char *message = XMLString::transcode(toCatch.msg);
		cout << "loadFileFromString(xmlContent): Exception " << message << endl;
		XMLString::release(&message);
		throw runtime_error("loadFileFromString(xmlContent): Failed to load xmlContent!");
	}

#else
	string xmlFile;
#if 1 //def USE_GCC
	// use temporary file /tmp/*.xml
	stringstream xmlFileStream;
	xmlFileStream << "/tmp/tempXmlCommands." << rand() << ".xml";
	xmlFile = xmlFileStream.str();
#else
	stringstream xmlFileStream;
	xmlFileStream << "tempXmlCommands." << rand() << ".xml";
	xmlFile = xmlFileStream.str();
#endif
	std::ofstream ofile(xmlFile.c_str());	// -> creates temporary file
	ofile << xmlContent;
	ofile.close();

	loadFile(xmlFile);
	remove(xmlFile.c_str());
#endif
}

void ConfigurationFile::saveFile(const string& fileName, DOMElement* currentNode)
{
    if (! xmlDocument)
        throw runtime_error("No configuration file loaded");

#if 1 //USE_GCC
	IDEBUG() << "WARNING!!!! Should house ChangeDirectorySeparator!!!";
    string nativeName = fileName; // FileStream::ChangeDirectorySeparator(fileName, '/');
#else
    string nativeName = FileStream::ChangeDirectorySeparator(fileName, '\\');
#endif

	if (currentNode == NULL)
		currentNode = xmlDocument->getDocumentElement();    

#if 1// XERCES_3
    DOMLSSerializer* serializer = domImpl->createLSSerializer();
    DOMConfiguration* cfg = serializer->getDomConfig();
    cfg->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    if (! serializer->writeToURI(xmlDocument->getDocumentElement(),
                                 toXMLCh(nativeName.c_str())))
    {
        throw runtime_error("Unable to write XML file");
    }
#else
    DOMWriter* serializer = domImpl->createDOMWriter();
    serializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    LocalFileFormatTarget target(nativeName.c_str());
    if (! serializer->writeNode((XMLFormatTarget*) &target,
                                *xmlDocument->getDocumentElement()))
    {
        throw runtime_error("Unable to write XML file");
    }
#endif

}

std::string ConfigurationFile::writeToString(DOMElement* currentNode)
{
	if (! xmlDocument)
	{
        //throw runtime_error("No configuration file loaded");
		return string("");
	}

	if (currentNode == NULL)
		currentNode = xmlDocument->getDocumentElement();    

#if 1 //XERCES_3
	//throw runtime_error("ConfigurationFile::writeToString(DOMElement*): Not implemented for XERCES_3");
	//return string("");
    
	DOMLSSerializer* serializer = domImpl->createLSSerializer();
    DOMConfiguration* cfg = serializer->getDomConfig();
    cfg->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    XMLCh *ch = serializer->writeToString(currentNode);
	if(ch == NULL) return string("");

	return fromXMLCh(ch);
#else
    DOMWriter* serializer = domImpl->createDOMWriter();
    serializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	XMLCh *ch = serializer->writeToString(*currentNode);
	if(ch == NULL) return string("");

	return fromXMLCh(ch);
#endif
}


ConfigurationNodeList
ConfigurationFile::getConfigurationNodes(const string& path, DOMElement* currentNode) const
{
    ConfigurationNodeList result;
    (void) lookup(&result, currentNode, path);
    return result;
}


ConfigurationNode ConfigurationFile::getConfigurationNode(const string& path, DOMElement* currentNode) const
{
    ConfigurationNode emptyNode;
    ConfigurationNodeList result;

    (void) lookup(&result, currentNode, path);

    if (result.size() > 0)
        return result.front();
    else
        return emptyNode;
}

ConfigurationNode ConfigurationFile::getConfigurationNode(xercesc::DOMElement* currentNode) const
{
    if (! xmlDocument)
		throw runtime_error("No configuration file loaded");

	if(currentNode == NULL) return ConfigurationNode();

	ConfigurationNode node;
    DOMNamedNodeMap* lst = currentNode->getAttributes();
	if(lst == NULL) return ConfigurationNode();

    for (int i = 0; i < (int) lst->getLength(); i++) 
	{
        DOMAttr* a = (DOMAttr*) lst->item(i);
		if(a == NULL) continue;

        node[fromXMLCh(a->getName())] = fromXMLCh(a->getValue());
    }

	return node;
}

void ConfigurationFile::addOrUpdate(xercesc::DOMElement* hisCurrentNode, xercesc::DOMElement* thisCurrentNode)
{
    if (! xmlDocument)
		throw runtime_error("No configuration file loaded");
	
	if(hisCurrentNode == NULL || thisCurrentNode == NULL) return;

    DOMNodeList* thisChildren = thisCurrentNode->getChildNodes();
	DOMNodeList* hisChildren = hisCurrentNode->getChildNodes();

	ASSERT(thisChildren);
	ASSERT(hisChildren);

	std::set<string> tagsInHis;
    for(unsigned int i = 0; i < thisChildren->getLength(); i++) 
	{
        DOMElement* thisChild = (DOMElement*) thisChildren->item(i);
		ASSERT(thisChild);

		for(unsigned int j = 0; j < hisChildren->getLength(); j++) 
		{
		    DOMElement* hisChild = (DOMElement*) hisChildren->item(j);
			ASSERT(hisChild);
			
			if(XMLString::compareString(thisChild->getTagName(), hisChild->getTagName()) == 0)  //found matching tag/namespace!
			{
				// -- begin debug --
				//cerr << "Matching tags: \"" << fromXMLCh(thisChild->getTagName()) << "\" == \"" << fromXMLCh(hisChild->getTagName()) << "\"" << endl;
				// -- end debug --

				tagsInHis.insert(fromXMLCh(hisChild->getTagName()));
				
				//TODO: make more efficient? 
				//if(hisChild->hasChildNodes())
				addOrUpdate(hisChild, thisChild);

				xercesc::DOMNamedNodeMap *thisNamedNodeMap = thisChild->getAttributes();
				xercesc::DOMNamedNodeMap *hisNamedNodeMap = hisChild->getAttributes();

				if(hisNamedNodeMap == NULL) continue;
				else if(thisNamedNodeMap == NULL && hisNamedNodeMap == NULL) continue;
				else if(thisNamedNodeMap == NULL && hisNamedNodeMap != NULL) 
					// -> should never hit because tags are initially identical
				{
					// -- debug!! --
					cerr << "TODO: Add values from hisNamedNodeMap -> they are missing in thisNamedNodeMap" << endl;
					for(unsigned int l = 0; l < hisNamedNodeMap->getLength(); l++)
					{
						xercesc::DOMNode *hisDomNode = hisNamedNodeMap->item(l);
						if(hisDomNode == NULL) continue;
					
						cerr << "His: (" << fromXMLCh(hisDomNode->getNodeName()) << "," << fromXMLCh(hisDomNode->getNodeValue()) << ") " << endl;
					}
					ASSERT(hisNamedNodeMap == NULL);
					// -- debug!! --
				}
				else // search for: replace existing (attribute, value), add new (attribute, value)
				{
					std::set<string> valuesInHis;
					// First: replace existing value -> (attribute, value) pair
					for(unsigned int k = 0; k < thisNamedNodeMap->getLength(); k++) 
					{
						xercesc::DOMNode *thisDomNode = thisNamedNodeMap->item(k);
						if(thisDomNode == NULL) continue;

						//cerr << "This: (" << fromXMLCh(thisDomNode->getNodeName()) << "," << fromXMLCh(thisDomNode->getNodeValue()) << ") " << endl;
						for(unsigned int l = 0; l < hisNamedNodeMap->getLength(); l++)
						{
							xercesc::DOMNode *hisDomNode = hisNamedNodeMap->item(l);
							if(hisDomNode == NULL) continue;
						
							//cerr << "His: (" << fromXMLCh(hisDomNode->getNodeName()) << "," << fromXMLCh(hisDomNode->getNodeValue()) << ") " << endl;
							if(XMLString::compareString(thisDomNode->getNodeName(), hisDomNode->getNodeName()) == 0) // -> attribute/node-name identical
							{
								valuesInHis.insert(fromXMLCh(hisDomNode->getNodeName()));

								if(XMLString::compareString(thisDomNode->getNodeValue(), hisDomNode->getNodeValue()) != 0) // values differ -> replace value
									thisDomNode->setNodeValue(hisDomNode->getNodeValue());
							}
						}
					}

					// Second: insert (attribute,value) from hisNamedNodeMap not in thisNamedNodeMap
					for(unsigned int k = 0; k < hisNamedNodeMap->getLength(); k++)
					{
						xercesc::DOMNode *hisDomNode = hisNamedNodeMap->item(k);
						if(hisDomNode == NULL) continue;
						if(valuesInHis.count(fromXMLCh(hisDomNode->getNodeName()))) continue;

						//cerr << "Importing node (" << fromXMLCh(hisDomNode->getNodeName()) << "," << fromXMLCh(hisDomNode->getNodeValue()) << ") " << endl;
						thisChild->setAttribute(hisDomNode->getNodeName(), hisDomNode->getNodeValue());
					}
				}
		    }
		}
    }
	
	// Insert new namespaces -> nodes in "hisNodes" that are not present in "thisNodes"
	for(unsigned int j = 0; j < hisChildren->getLength(); j++) 
	{
	    DOMElement* hisChild = (DOMElement*) hisChildren->item(j);
		ASSERT(hisChild);
		if(tagsInHis.count(fromXMLCh(hisChild->getTagName())) > 0) continue;
		
		xercesc::DOMNamedNodeMap *hisNamedNodeMap = hisChild->getAttributes();
		if(hisNamedNodeMap == NULL) continue;

		try
		{
			xercesc::DOMNode *importedNode = xmlDocument->importNode(hisChild, true);
			ASSERT(importedNode);
			thisCurrentNode->appendChild(importedNode);

			// -- begin debug --
			//cout << "Added new node : " << endl << writeToString(importedNode) << endl;
			//cout << "Added new! This current node : " << endl << writeToString(thisCurrentNode) << endl;
			// -- end debug --
		}
		catch(std::runtime_error &msg)
		{
			cerr << "ConfigurationFile::addOrUpdate(DOMElement*, DOMElement*): WARNING! Cannot append child: (" << fromXMLCh(hisChild->getNodeName()) << ") msg: " << msg.what() << endl;
		}
		catch(...)
		{
			cerr << "ConfigurationFile::addOrUpdate(DOMElement*, DOMElement*): WARNING! Cannot append child: (" << fromXMLCh(hisChild->getNodeName()) << ")" << endl;
		}
    }
}

void ConfigurationFile::addConfigurationNode(const string& path,
                                             ConfigurationNode* newNode)
{
    if (! xmlDocument) {
        string rootTag(path, 0, path.find(pathSeparator));
        xmlDocument = domImpl->createDocument(0, toXMLCh(rootTag), 0);
    }

    DOMElement* currentNode = xmlDocument->getDocumentElement();
    string remainingPath = path;

    bool finalTag = false;
    while (! finalTag) {
        // Separate next tag name in the path
        int idx = remainingPath.find(pathSeparator);
        if (idx == (int) string::npos)
            finalTag = true;

        string nextTag(remainingPath, 0, idx);
        remainingPath = string(remainingPath, idx + pathSeparator.length());
        // Skip the document/root tag
        if (nextTag == string(path, 0, idx))
            continue;
        XMLCh* findTag = toXMLCh(nextTag);

        if (finalTag) {
            // Always create the final tag in the path
            DOMElement* node = xmlDocument->createElement(findTag);
            currentNode->appendChild(node);
            currentNode = node;
        } else {
            // Loop through all children, and remember the last matching tag
            DOMElement* lastChild = NULL;
            DOMNodeList* children = currentNode->getChildNodes();
            for (unsigned int i = 0; i < children->getLength(); i++) {
                DOMElement* child = (DOMElement*) children->item(i);
                if (!XMLString::compareString(child->getTagName(), findTag)) {
                    // Found!
                    lastChild = child;
                }
            }

            if (lastChild) {
                // Continue traversing tree on the last tag found
                currentNode = lastChild;
            } else {
                // No matching tag found, so we'll create one now
                lastChild = xmlDocument->createElement(findTag);
                currentNode->appendChild(lastChild);
                currentNode = lastChild;
            }
        }
    }

    if (newNode) {
        // Fill the newly created DOMElement from the ConfigurationNode
        ConfigurationNode::iterator iter;
        for (iter = newNode->begin(); iter != newNode->end(); iter++) {
            currentNode->setAttribute(toXMLCh((*iter).first),
                                      toXMLCh((*iter).second));
        }
    }

}

bool ConfigurationFile::hasConfigurationNode(const std::string& path, DOMElement* currentNode) const
{
    ConfigurationNodeList result;
    return lookup(&result, currentNode, path);
}

DOMElementList ConfigurationFile::getDOMElements(const std::string& path, DOMElement* currentNode) const
{
    DOMElementList result;
    (void) lookup(&result, currentNode, path);
    return result;
}


XMLCh* ConfigurationFile::toXMLCh(const char* s) const
{
    int len = strlen(s);
    XMLCh* tmp = new XMLCh[len+1];
    if (tmp) {
        XMLString::transcode(s, tmp, len);
        tmp[len] = '\0';
    }
    return tmp;
}


XMLCh* ConfigurationFile::toXMLCh(const string& s) const
{
    return toXMLCh(s.c_str());
}


string ConfigurationFile::fromXMLCh(const XMLCh* s) const
{
	if(s == NULL) return string();

	unsigned int len = 0;
#if 1//USE_GCC
	len = wcslen((const wchar_t*)s);
#else
	len = wcslen(s);
#endif
	
	if(len > 4096)
	{
		char *tmp = new char[len];
		XMLString::transcode(s, tmp, sizeof(tmp)-1);
		
		string c(tmp);
		delete [] tmp;
		return c;		
	}
	else
	{
		char tmp[4096];
		XMLString::transcode(s, tmp, sizeof(tmp)-1);
		return string(tmp);
	}
}


bool ConfigurationFile::lookup(ConfigurationNodeList* resultSet,
                               DOMElement* currentNode, const string& path) const
{
    if (! xmlDocument)
        return false;

    // If this is the terminal node, map its attributes and return
    if (path == "" && currentNode != NULL) {
        ConfigurationNode confNode;
        DOMNamedNodeMap* lst = currentNode->getAttributes();
        for (int i = 0; i < (int) lst->getLength(); i++) {
            DOMAttr* a = (DOMAttr*) lst->item(i);
            confNode[fromXMLCh(a->getName())] = fromXMLCh(a->getValue());
        }
        if (! confNode.empty()) {
            resultSet->push_back(confNode);
        }

        return true;
    }

    // Find first separator substring
    int idx = path.find(pathSeparator);
    // Remove the next tag from the path
    string remainingPath = "";
    if (idx != (int) string::npos) {
        remainingPath = string(path, idx + pathSeparator.length());
    }
    // The first tag is always the document root
    if (currentNode == NULL) {
        return lookup(resultSet, xmlDocument->getDocumentElement(),
                      remainingPath);
    }

    // Go through all child nodes to current node, looking
    // for one that matches the next tag in the path
    XMLCh* findTag = toXMLCh(string(path, 0, idx));
    DOMNodeList* lst = currentNode->getChildNodes();
    bool foundAny = false;
    for (int i = 0; i < (int) lst->getLength(); i++) {
        DOMElement* nextNode = (DOMElement*) lst->item(i);
       if (!XMLString::compareString(nextNode->getTagName(), findTag)) {
            // Found! Recursively run down the tree starting at this child
            foundAny = lookup(resultSet, nextNode, remainingPath);
        }
    }
    delete[] findTag;

    return foundAny;
}

bool ConfigurationFile::lookup(DOMElementList* resultSet,
                               DOMElement* currentNode, const string& path) const
{
    // If this is the terminal node, map its attributes and return
    if (path == "" && currentNode != NULL) {
        resultSet->push_back(currentNode);
        return true;
    }

    // Find first separator substring
    int idx = path.find(pathSeparator);
    // Remove the next tag from the path
    string remainingPath = "";
    if (idx != (int) string::npos) {
        remainingPath = string(path, idx + pathSeparator.length());
    }

    // The first tag is always the document root
    if (currentNode == NULL) {
        return lookup(resultSet, xmlDocument->getDocumentElement(), remainingPath);
    }

    // Go through all child nodes to current node, looking
    // for one that matches the next tag in the path
    XMLCh* findTag = toXMLCh(string(path, 0, idx));
    DOMNodeList* lst = currentNode->getChildNodes();
    bool foundAny = false;
    for (int i = 0; i < (int) lst->getLength(); i++) {
        DOMElement* nextNode = (DOMElement*) lst->item(i);
        if (!XMLString::compareString(nextNode->getTagName(), findTag)) {
            // Found! Recursively run down the tree starting at this child
            foundAny = lookup(resultSet, nextNode, remainingPath);
        }
    }
    delete[] findTag;
    return foundAny;
}


string ConfigurationFile::stringValue(const ConfigurationNode& configNode, const char* paramName, const string defaultValue)
{
    string result = defaultValue;
    ConfigurationNode::const_iterator it = configNode.find(paramName);
    if(it != configNode.end()) result = it->second;
    return result;
}

int ConfigurationFile::intValue(const ConfigurationNode& configNode, const char* paramName, int defaultValue)
{
    int result = defaultValue;
    string value=stringValue(configNode, paramName);
    if(value.length() > 0) result = atoi(value.c_str());
    return result;
}

bool ConfigurationFile::boolValue(const ConfigurationNode& configNode, const char* paramName, bool defaultValue)
{
    bool result = defaultValue;
    string value=stringValue(configNode, paramName);
    if(value.length() > 0) {
        if(value=="yes" || value=="on" || value=="true")
            result = true;
        else 
            result = false;
    }
    return result;
}

double ConfigurationFile::doubleValue(const ConfigurationNode& configNode, const char* paramName, double defaultValue)
{
    double result = defaultValue;
    string value=stringValue(configNode, paramName);
    if(value.length() > 0) result = atof(value.c_str());
    return result;
}

#ifdef DISABLED

string ConfigurationFile::stringValue(const char* nodeName, const char* paramName, const string defaultValue) const
{
    return stringValue(NULL, nodeName, paramName, defaultValue);
}

double ConfigurationFile::doubleValue(const char* nodeName, const char* paramName, double defaultValue) const
{
    return doubleValue(NULL, nodeName, paramName, defaultValue);
}

int ConfigurationFile::intValue(const char* nodeName, const char* paramName, int defaultValue) const
{
    return intValue(NULL, nodeName, paramName, defaultValue);
}

bool ConfigurationFile::boolValue(const char* nodeName, const char* paramName, bool defaultValue) const
{
    return boolValue(NULL, nodeName, paramName, defaultValue);
}

#endif

void ConfigurationFile::stringList(const ConfigurationNode& configNode, const char* paramName, StringVector& container)
{
	string str = stringValue(configNode, paramName);
    StringTokenizer::tokenize(str, container, " \t\n\r", true);
}

void ConfigurationFile::doubleList(const ConfigurationNode& configNode, const char* paramName, vector<double>& container)
{
	StringVector elements;
	stringList(configNode, paramName, elements);

	StringVector::iterator it;
	for(it=elements.begin() ; it != elements.end() ; it++)
		container.push_back(atof(it->c_str()));
}

void ConfigurationFile::floatList(const ConfigurationNode& configNode, const char* paramName, vector<float>& container)
{
	StringVector elements;
	stringList(configNode, paramName, elements);

	StringVector::iterator it;
	for(it=elements.begin() ; it != elements.end() ; it++)
		container.push_back((float) atof(it->c_str()));
}

void ConfigurationFile::intList(const ConfigurationNode& configNode, const char* paramName, vector<int>& container)
{
	StringVector elements;
	stringList(configNode, paramName, elements);

	StringVector::iterator it;
	for(it=elements.begin() ; it != elements.end() ; it++)
		container.push_back(atoi(it->c_str()));
}

void ConfigurationFile::boolList(const ConfigurationNode& configNode, const char* paramName, vector<bool>& container)
{
    StringVector elements;
    stringList(configNode, paramName, elements);

    StringVector::iterator it;
    for(it=elements.begin() ; it != elements.end() ; it++)
        if(*it=="yes" || *it=="on" || *it=="true")
            container.push_back(true);
        else 
            container.push_back(false);

}

void ConfigurationFile::stringList(const char* nodeName, const char* paramName, StringVector& container, xercesc::DOMElement* startNode) const
{
    string str = stringValue(nodeName, paramName, "", startNode);
    StringTokenizer::tokenize(str, container, " \t\n\r", true);
}

void ConfigurationFile::doubleList(const char* nodeName, const char* paramName, vector<double>& container, xercesc::DOMElement* startNode) const
{
    StringVector elements;
    stringList(nodeName, paramName, elements, startNode);

    StringVector::iterator it;
    for(it=elements.begin() ; it != elements.end() ; it++)
        container.push_back(atof(it->c_str()));
}

void ConfigurationFile::floatList(const char* nodeName, const char* paramName, vector<float>& container, xercesc::DOMElement* startNode) const
{
    StringVector elements;
    stringList(nodeName, paramName, elements, startNode);

    StringVector::iterator it;
    for(it=elements.begin() ; it != elements.end() ; it++)
        container.push_back((float) atof(it->c_str()));
}

void ConfigurationFile::intList(const char* nodeName, const char* paramName, vector<int>& container, xercesc::DOMElement* startNode) const
{
    StringVector elements;
    stringList(nodeName, paramName, elements, startNode);

    StringVector::iterator it;
    for(it=elements.begin() ; it != elements.end() ; it++)
        container.push_back(atoi(it->c_str()));
}

void ConfigurationFile::boolList(const char* nodeName, const char* paramName, vector<bool>& container, xercesc::DOMElement* startNode) const
{
    StringVector elements;
    stringList(nodeName, paramName, elements, startNode);

    StringVector::iterator it;
    for(it=elements.begin() ; it != elements.end() ; it++)
        if(*it=="yes" || *it=="on" || *it=="true")
            container.push_back(true);
        else 
            container.push_back(false);

}

string ConfigurationFile::stringValue(const char* nodeName, const char* paramName, const string defaultValue, xercesc::DOMElement* startNode) const
{
    if(hasConfigurationNode(nodeName, startNode))
        return stringValue(getConfigurationNode(nodeName, startNode), paramName, defaultValue);
    else
        return defaultValue;
}

double ConfigurationFile::doubleValue(const char* nodeName, const char* paramName, double defaultValue, xercesc::DOMElement* startNode) const
{
    if(hasConfigurationNode(nodeName, startNode))
        return doubleValue(getConfigurationNode(nodeName, startNode), paramName, defaultValue);
    else
        return defaultValue;
}

int ConfigurationFile::intValue(const char* nodeName, const char* paramName, int defaultValue, xercesc::DOMElement* startNode) const
{
    if(hasConfigurationNode(nodeName, startNode))
        return intValue(getConfigurationNode(nodeName, startNode), paramName, defaultValue);
    else
        return defaultValue;
}

bool ConfigurationFile::boolValue(const char* nodeName, const char* paramName, bool defaultValue, xercesc::DOMElement* startNode) const
{
    if(hasConfigurationNode(nodeName, startNode))
        return boolValue(getConfigurationNode(nodeName, startNode), paramName, defaultValue);
    else
        return defaultValue;
}

}
