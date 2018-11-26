#ifndef RxData_Cache_Lifespan_h_IsIncluded
#define RxData_Cache_Lifespan_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{

class DLL_STATE Lifespan
{
private:
    // ----------------------------------
    // Attributes
    // ----------------------------------

    Duration duration_;

    // ----------------------------------
    // Constructor
    // ----------------------------------

public:
    /**
     * Constructor sets the cache lifespan to maximum.
     */
    Lifespan()
        : duration_(Duration::Infinite())
    { }

    /**
     * User defined lifespan
     *
     * @param duration
     */
    Lifespan(Duration dur)
        : duration_(dur)
    { }

    ~Lifespan()
    { }

    // ----------------------------------
    // Getters
    // ----------------------------------

    const Duration& AsDuration() const
    {
        return duration_;
    }

    /**
     * Returns the number of milliseconds since January 1, 1970, 00:00:00 GMT
     * represented by the internal <tt>Date</tt> object.
     *
     * @return  the number of milliseconds since January 1, 1970, 00:00:00 GMT
     *          represented by the internal date.
     */
    int64 InMillis() const
    {
        return duration_.InMillis();
    }

    /**
     * @return infinite Lifespan.
     */
    static Lifespan Infinite()
    {
        return Lifespan(Duration::Infinite());
    }
};

}

#endif

