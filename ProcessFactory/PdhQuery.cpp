#include "PdhQuery.h"

#include <string>

using namespace std;

namespace ProcessFactorySpace {

#ifndef USE_GCC

/* zero the object */
PdhQuery::PdhQuery()
{
      count = 0;
      names = 0;
      counters = 0;
      last_error = 0;
}

/* close the object down */
PdhQuery::~PdhQuery()
{
      /* free the names */
      for(int i=0;i<count;i++) 
      {
            if(names[i]) free(names[i]);
      }
      if(names)free(names);
      /* free the counter array, this is not a pointer to pointer just a straight
      array */
      if(counters)free(counters);
      /* close the query handle */
      PdhCloseQuery(query_handle);
}

int PdhQuery::lookUpIndex(char *name)
{
      int retval = -1;
      for(int i=0;i<count;i++)
      {
            if(names[i]) /* care */
            {
				if(!::_stricmp(names[i],name))
                  {
                        retval = i;
                        break;
                  }
            }
      } 
      return retval;
}

bool PdhQuery::upSizeNames(char *name)
{
      bool retval = false;
      if(names = (char**)realloc(names,((count + 1) * sizeof(char*))))
      {
            names[count] = NULL;
			if(names[count] = ::_strdup(name)) retval = true;
            else 
            {
                  downSizeNames();
                  last_error = PDH_MEMORY_ALLOCATION_FAILURE; 
            }
      }
      else last_error = PDH_MEMORY_ALLOCATION_FAILURE;
      return retval;
}

bool PdhQuery::upSizeCounters()
{
      bool retval = false;
      if(counters = (PDHCOUNTERSTRUCT*)realloc(counters,
                                                                   ((count + 1) * sizeof(PDHCOUNTERSTRUCT))))
      {
            retval = true;
      }
      else last_error = PDH_MEMORY_ALLOCATION_FAILURE;
      return retval;
}

bool PdhQuery::downSizeNames()
{
      bool retval = true;
      if(names[count]) free(names[count]);
      names = (char**)realloc(names,(count * sizeof(char*)));
      return retval;
}

bool PdhQuery::downSizeCounters()
{
      bool retval = true;
      counters = (PDHCOUNTERSTRUCT*)realloc(counters,
                                                                   (count * sizeof(PDHCOUNTERSTRUCT)));
      return retval;
}

/* init the object */
bool PdhQuery::init()
{
      bool retval = false;
      if((last_error = PdhOpenQuery(NULL,1,&query_handle)) == ERROR_SUCCESS)
      {
            retval = true;
      }
      return retval;
}

bool PdhQuery::addWildCardCounter(const string &wildCardPath, vector<string> &counterNames)
{
	if(mapWildCardToCounters.count(wildCardPath) > 0)
	{
		cout << "addwildCardCounter() WARNING! wild card path exists : " << wildCardPath << endl;
		return false;
	}

	char *paths = NULL;
	unsigned long dwSize = 0;

	PDH_STATUS status = PdhExpandWildCardPath(0, wildCardPath.c_str(), paths, &dwSize, 0);
	if(status != PDH_MORE_DATA) 
	{
        cout << "addWildCardCounter() ERROR! No expand wild card data on " << wildCardPath << endl;
		return false;
	}

	dwSize = dwSize + 1;
	char *pPaths = new char[dwSize];

	status = PdhExpandWildCardPath(0, wildCardPath.c_str(), pPaths, &dwSize, 0);
	if(status != ERROR_SUCCESS)
	{
        cout << "addWildCardCounter(): expand path failed on " << wildCardPath << endl;
		return false;
	}

	LPTSTR ptr;
	ptr = pPaths;
	while(*ptr)
	{
		//cout << "addWildCardCounter(): adding counter " << ptr << endl;
		bool ret = addCounter(ptr);
		if(ret == false)
		{
			cout << "addWildCardCounter(): Couldn't add counter : " << ptr << endl;
		}
		else
		{
			counterNames.push_back(string(ptr));
		
			mapWildCardToCounters[wildCardPath].push_back(ptr);
		}

		ptr += strlen(ptr);
		ptr++;
	}

	delete pPaths;

	return true;
}


/* add a counter */
bool PdhQuery::addCounter(char *name)
{
      bool retval = false;

      if(upSizeCounters())
      {
            if((last_error = PdhAddCounter(query_handle,
                                                         name,
                                                       (DWORD)&counters[count], 
                                                       &counters[count].hCounter)) == ERROR_SUCCESS) 
            {
                  if(upSizeNames(name))
                  {
                        ++count;
                        retval = true;
                  }
                  else 
                  {
                        PdhRemoveCounter(counters[count].hCounter);
                        downSizeCounters();
                  }
            }
            else downSizeCounters();
      }
      return retval;
}

bool PdhQuery::updateCounters()
{
      bool retval = false;
      if((last_error = PdhCollectQueryData(query_handle)) == ERROR_SUCCESS) retval = true;
      return retval;
}

bool PdhQuery::getWildCardCounters(const string &wildCardPath, MapCounterToValue32 &mapCounterToValue)
{
	if(mapWildCardToCounters.count(wildCardPath) <= 0)
	{
		//cout << "getwildCardCounter(): WARNING! wild card path does not exist! " << wildCardPath << endl;
		return false;
	}

	const vector<string> &vectorCounterValues = mapWildCardToCounters[wildCardPath];
	for(vector<string>::const_iterator it = vectorCounterValues.begin(), it_end = vectorCounterValues.end(); it != it_end; ++it)
	{
		long val = getCounter((char*)it->c_str());
		string key = getWildCardKey(*it);
		mapCounterToValue.insert(pair<string, long>(key, val));
	}

	return true;
}

bool PdhQuery::getWildCardCounters(const string &wildCardPath, MapCounterToValue64 &mapCounterToValue)
{
	if(mapWildCardToCounters.count(wildCardPath) <= 0)
	{
		//cout << "getwildCardCounter(): WARNING! wild card path does not exist! " << wildCardPath << endl;
		return false;
	}

	const vector<string> &vectorCounterValues = mapWildCardToCounters[wildCardPath];
	for(vector<string>::const_iterator it = vectorCounterValues.begin(), it_end = vectorCounterValues.end(); it != it_end; ++it)
	{
		long long val = getCounter64((char*)it->c_str());
		string key = getWildCardKey(*it);
		mapCounterToValue.insert(pair<string, long long>(key, val));
	}

	return true;
}

bool PdhQuery::getWildCardCounters(const string &wildCardPath, MapCounterToValueDouble &mapCounterToValue)
{
	if(mapWildCardToCounters.count(wildCardPath) <= 0)
	{
		//cout << "getwildCardCounter(): WARNING! wild card path does not exist! " << wildCardPath << endl;
		return false;
	}

	const vector<string> &vectorCounterValues = mapWildCardToCounters[wildCardPath];
	for(vector<string>::const_iterator it = vectorCounterValues.begin(), it_end = vectorCounterValues.end(); it != it_end; ++it)
	{
		double val = getCounterDouble((char*)it->c_str());
		string key = getWildCardKey(*it);
		mapCounterToValue.insert(pair<string, double>(key, val));
	}

	return true;
}


string PdhQuery::getWildCardKey(const string &counter)
{
	stringstream value;
	bool foundStart = false;
	for(string::const_iterator it = counter.begin(), it_end = counter.end(); it != it_end; ++it)
	{
		if(*it == ')') break;
		if(foundStart == true)	
		{
			value << *it;
		}
		if(*it == '(') foundStart = true;
	}
	return value.str();
}

long PdhQuery::getCounter(char *name)
{
      int i =0;
      long retval = -1;
      if((i = lookUpIndex(name)) > -1)
      {
            PDH_FMT_COUNTERVALUE pdhFormattedValue;      
            if((last_error = PdhGetFormattedCounterValue(counters[i].hCounter,
                                                         PDH_FMT_LONG,
                                                         NULL,
                                                         &pdhFormattedValue)) == ERROR_SUCCESS)
            {
                  retval = pdhFormattedValue.longValue;
            }
      }
      return retval;
}

long long PdhQuery::getCounter64(char *name)
{
      int i =0;
      long long retval = -1;
      if((i = lookUpIndex(name)) > -1)
      {
            PDH_FMT_COUNTERVALUE pdhFormattedValue;      
            if((last_error = PdhGetFormattedCounterValue(counters[i].hCounter,
                                                         PDH_FMT_LARGE,
                                                         NULL,
                                                         &pdhFormattedValue)) == ERROR_SUCCESS)
            {
				retval = pdhFormattedValue.largeValue;
            }
      }
      return retval;
}

double PdhQuery::getCounterDouble(char *name)
{
      int i =0;
      double retval = -1;
      if((i = lookUpIndex(name)) > -1)
      {
            PDH_FMT_COUNTERVALUE pdhFormattedValue;      
            if((last_error = PdhGetFormattedCounterValue(counters[i].hCounter,
                                                         PDH_FMT_DOUBLE,
                                                         NULL,
                                                         &pdhFormattedValue)) == ERROR_SUCCESS)
            {
				retval = pdhFormattedValue.doubleValue;
            }
      }
      return retval;
}

char *PdhQuery::getLastErrorString()
{
      switch(last_error)
      {
      case ERROR_SUCCESS:
            strcpy_s(last_error_str,"There is no error");
            break;
      case PDH_INVALID_ARGUMENT:
            strcpy_s(last_error_str,"Invalid argument");
            break;
      case PDH_MEMORY_ALLOCATION_FAILURE:
            strcpy_s(last_error_str,"Memory allocation failure");
            break;
      case PDH_CSTATUS_BAD_COUNTERNAME: 
            strcpy_s(last_error_str,"The counter name path string could not be parsed "
                                            "or interpreted");
            break;
      case PDH_CSTATUS_NO_COUNTER:
            strcpy_s(last_error_str,"The specified counter was not found");
            break;
      case PDH_CSTATUS_NO_COUNTERNAME:
            strcpy_s(last_error_str,"An empty counter name path string was passed in");
            break;
      case PDH_CSTATUS_NO_MACHINE:
            strcpy_s(last_error_str,"A machine entry could not be created");
            break;
      case PDH_CSTATUS_NO_OBJECT:
            strcpy_s(last_error_str,"The specified object could not be found");
            break;
      case PDH_FUNCTION_NOT_FOUND:
            strcpy_s(last_error_str,"The calculation function for this counter could "
                                            "not be determined");
            break;
      case PDH_INVALID_HANDLE:
            strcpy_s(last_error_str,"The query handle is not valid");
            break; 
      case PDH_NO_DATA:
             strcpy_s(last_error_str,"No data for that counter");
            break;
      }
      return last_error_str;
}

#endif // #ifndef USE_GCC

} // namespace ProcessFactorySpace


