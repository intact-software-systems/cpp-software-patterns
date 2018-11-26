#ifndef USE_GCC

#ifndef Pdh_Query_h_IS_INCLUDED
#define Pdh_Query_h_IS_INCLUDED

#include "ProcessFactory/IncludeExtLibs.h"

#include <map>
#include <pdh.h>  
#include <pdhmsg.h>
//#include <windows.h>

using namespace std;

namespace ProcessFactorySpace 
{

#define MAX_RAW_VALUES                        1//20

class PdhQuery
{
protected:
      /* user defined struct that the PDH calls use, i define it local to the class
      to avoid any undue namespace pollution*/
      typedef struct _tag_PDHCounterStruct 
      {
            HCOUNTER hCounter;      
            int nNextIndex;         
            int nOldestIndex;       
            int nRawCount;          
            PDH_RAW_COUNTER a_RawValue[MAX_RAW_VALUES]; 
      }PDHCOUNTERSTRUCT, *PPDHCOUNTERSTRUCT;

      /* query handle */
      HQUERY query_handle;

	  typedef map<string, int>				MapCounterToValue32;
	  typedef map<string, long long>		MapCounterToValue64;
	  typedef map<string, double>			MapCounterToValueDouble;

	  typedef map<string, vector<string> >  MapWildCardToCounters;
	  MapWildCardToCounters mapWildCardToCounters;

      /* list of names and counters */
      int count;
      char **names;
      PDHCOUNTERSTRUCT *counters;

      int last_error;
      char last_error_str[100];

      bool upSizeNames(char *name);
      bool upSizeCounters();
      bool downSizeNames();
      bool downSizeCounters();

      int lookUpIndex(char *name);

public:
      PdhQuery();
      ~PdhQuery();

      /* sets the object up */
      bool init();

	  bool addWildCardCounter(const string &wildCardPath, vector<string> &counterNames = vector<string>());

      /* adds a counter, pass it the fully qualified counter name */
      bool addCounter(char *name);

      /* update the counters */
      bool updateCounters();

	  bool getWildCardCounters(const string &wildCardPath, MapCounterToValue32 &mapCounterToValue);
	  bool getWildCardCounters(const string &wildCardPath, MapCounterToValue64 &mapCounterToValue);
	  bool getWildCardCounters(const string &wildCardPath, MapCounterToValueDouble &mapCounterToValue);

	  string getWildCardKey(const string &counter);

      /* gets the value of a counter */
	  long getCounter(char *name);
	  long long getCounter64(char *name);
	  double getCounterDouble(char *name);


      /* dont free the pointer returned, its managed in the class */
      char *getLastErrorString();
};

}

#endif // Pdh_Query_h_IS_INCLUDED

#endif // #ifndef USE_GCC

