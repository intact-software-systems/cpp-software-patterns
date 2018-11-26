#ifndef BaseLib_ProtectedVar_h_IsIncluded
#define BaseLib_ProtectedVar_h_IsIncluded

#include <ostream>

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/WaitCondition.h"

namespace BaseLib
{
// Template Protected Var - use it for creating mutexed protected numeric variables
template<class T> class ProtectedVar
{
    T               var_;
    mutable Mutex   mutex_;

public:
    ProtectedVar(T var)
        : var_(var)
    {
    }

    ProtectedVar()
        : var_( (T) 0 )
    { }


public:
	// ------------------------------------------------
	//  operator manipulations
	// ------------------------------------------------

	// Pre-increment
	/*ProtectedVar<T>& operator++()
	{
        MutexLocker lock(&mutex_);
		this->var_++;
		return *this;
	}

    // Post-increment
	ProtectedVar<T> operator++(int)
	{
        MutexLocker lock(&mutex_);
        ProtectedVar<T> result(*this);
        ++(*this);
		return result;
	}

    // Pre-decrement
	ProtectedVar<T>& operator--()
	{
        MutexLocker lock(&mutex_);
		this->var_--;
		return *this;
	}

    // Post-decrement
	ProtectedVar<T> operator--(int)
	{
        MutexLocker lock(&mutex_);

        ProtectedVar<T> result(*this);
        --(*this);
		return result;
	}

    ProtectedVar<T>& operator+=(const T &other)
    {
        MutexLocker lock(&mutex_);
        this->var_ = this->var_ + other;
        return *this;
    }

    ProtectedVar<T> operator+(const T &other) const
    {
        MutexLocker lock(&mutex_);
        return ProtectedVar<T>(*this) += other;
    }

public:
	// ------------------------------------------------
	//  operator comparisons
	// ------------------------------------------------
	bool operator==(T rvalue) const
	{
        MutexLocker lock(&mutex_);

		return (this->var_ == rvalue);
	}

	bool operator!=(const T &rvalue) const
	{
        MutexLocker lock(&mutex_);
		return this->var_ != rvalue;
	}

	bool operator<(const T &rvalue) const
	{
        MutexLocker lock(&mutex_);
		return this->var_ < rvalue;
	}

	bool operator<=(const T &rvalue) const
	{
        MutexLocker lock(&mutex_);
		return this->var_ <= rvalue;
	}

	bool operator>(const T &rvalue) const
	{
        MutexLocker lock(&mutex_);
		return this->var_ > rvalue;
	}

	bool operator>=(const T &rvalue) const
	{
        MutexLocker lock(&mutex_);
		return this->var_ >= rvalue;
	}

    T get() const
    {
        MutexLocker lock(&mutex_);
        return var_;
    }*/

public:
    inline ProtectedVar<T>& operator= ( T i );
    inline ProtectedVar<T>& operator+=( T i );
    inline ProtectedVar<T>& operator-=( T i );

    inline bool operator==( T i );
    inline bool operator!=( T i );

    inline bool operator<(const T &i) const
    {
        MutexLocker lock(&mutex_);
        return var_ < i;
    }

    inline bool operator<=(const T &i) const
    {
        MutexLocker lock(&mutex_);
        return var_ <= i;
    }

    inline bool operator>(const T &i) const
    {
        MutexLocker lock(&mutex_);
        return var_ > i;
    }

    inline bool operator>=(const T &i) const
    {
        MutexLocker lock(&mutex_);
        return var_ >= i;
    }

    inline T get() const
    {
        MutexLocker lock(&mutex_);
        return var_;
    }

    inline T operator++();
    inline T operator++(int);
    inline T operator--();
    inline T operator--(int);


    inline T replace(T nval);

    inline operator T();    // implicit typecast operator

    friend std::ostream& operator<<(std::ostream& ostr, const ProtectedVar<T> &i)
    {
        ostr << i.var_;
        return ostr;
    }

    inline void print( std::ostream& ostr ) const;

};

	template<class T>
		inline ProtectedVar<T>& ProtectedVar<T>::operator=( T i )
		{
            MutexLocker lock(&mutex_);
			var_ = i;
			return *this;
		}

	template<class T>
		inline ProtectedVar<T>& ProtectedVar<T>::operator+=(T i)
		{
            MutexLocker lock(&mutex_);
			var_+=i;
			return *this;
		}

	template<class T>
		inline ProtectedVar<T>& ProtectedVar<T>::operator-=(T i)
		{
            MutexLocker lock(&mutex_);
			var_-=i;
			return *this;
		}

	template<class T>
		inline bool ProtectedVar<T>::operator==( T i )
		{
            MutexLocker lock(&mutex_);
			return ( var_ == i );
		}

	template<class T>
		inline bool ProtectedVar<T>::operator!=( T i )
		{
            MutexLocker lock(&mutex_);
			return ( var_ != i );
		}

	template<class T>
		inline T ProtectedVar<T>::operator++()
		{
            MutexLocker lock(&mutex_);
			T ret = ++var_;
			return ret;
		}

	template<class T>
		inline T ProtectedVar<T>::operator++(int)
		{
            MutexLocker lock(&mutex_);
			T ret = var_++;
			return ret;
		}

	template<class T>
		inline T ProtectedVar<T>::operator--()
		{
            MutexLocker lock(&mutex_);
			T ret = --var_;
			return ret;  
		}

	template<class T>
		inline T ProtectedVar<T>::operator--(int)
		{
            MutexLocker lock(&mutex_);
			T ret = var_--;
			return ret;
		}

	template<class T>
		inline T ProtectedVar<T>::replace( T nval )
		{
            MutexLocker lock(&mutex_);
			T ret = var_;
			var_  = nval;
			return ret;
		}

	template<class T>
		inline ProtectedVar<T>::operator T()    // implicit typecast operator
		{
            MutexLocker lock(&mutex_);
			T ret = var_;
			return ret;
		}

	template<class T>
        inline void ProtectedVar<T>::print( std::ostream& ostr ) const
		{
            MutexLocker lock(&mutex_);
			ostr << var_;
		}

    //extern std::ostream& operator<< ( std::ostream& ostr, ProtectedVar<int>& pi );

	/***********************************************************************
	 * ProtectedPtr<T>
	 * - protects known pointer variables with a mutex
	 ***********************************************************************/

	/* Template Protected Var - use it for creating mutexed protected numeric variables*/
	template<class T> class ProtectedPtr
	{
		T       _x;
		Mutex _m;
	public:
		ProtectedPtr ( T i )
			:_x(i)
		{
		}

		ProtectedPtr ()
		{
			_x=(T) 0;
		}

		inline ProtectedPtr<T>& operator= ( T i );
		inline ProtectedPtr<T>& operator+=( int i );
		inline ProtectedPtr<T>& operator-=( int i );

		inline bool operator==( T i );
		inline bool operator!=( T i );

		inline T operator++();
		inline T operator++(int);
		inline T operator--();
		inline T operator--(int);

		inline T replace( T nval );

		inline operator T();    // implicit typecast operator

		//friend std::ostream& operator<< ( std::ostream&, ProtectedPtr<T>& );

		inline void print_T( std::ostream& ostr );

	};

	template<class T>
		inline ProtectedPtr<T>& ProtectedPtr<T>::operator=( T i )
		{
            MutexLocker lock(&_m);
			_x = i;
			return *this;
		}

	template<class T>
		inline ProtectedPtr<T>& ProtectedPtr<T>::operator+=(int i)
		{
            MutexLocker lock(&_m);
			_x+=i;
			return *this;
		}

	template<class T>
		inline ProtectedPtr<T>& ProtectedPtr<T>::operator-=(int i)
		{
            MutexLocker lock(&_m);
			_x-=i;
			return *this;
		}

	template<class T>
		inline bool ProtectedPtr<T>::operator==( T i )
		{
            MutexLocker lock(&_m);
			bool ret = ( _x == i );
			return ret;
		}

	template<class T>
		inline bool ProtectedPtr<T>::operator!=( T i )
		{
            MutexLocker lock(&_m);
			bool ret = ( _x != i );
			return ret;
		}

	template<class T>
		inline T ProtectedPtr<T>::operator++()
		{
            MutexLocker lock(&_m);
			T ret = ++_x;
			return ret;
		}

	template<class T>
		inline T ProtectedPtr<T>::operator++(int)
		{
            MutexLocker lock(&_m);
			T ret = _x++;
			return ret;
		}

	template<class T>
		inline T ProtectedPtr<T>::operator--()
		{
            MutexLocker lock(&_m);
			T ret = --_x;
			return ret;  
		}

	template<class T>
		inline T ProtectedPtr<T>::operator--(int)
		{
            MutexLocker lock(&_m);
			T ret = _x--;
			return ret;
		}

	template<class T>
		inline T ProtectedPtr<T>::replace( T nval )
		{
            MutexLocker lock(&_m);
			T ret = _x;
			_x    = nval;
			return ret;
		}

	template<class T>
		inline ProtectedPtr<T>::operator T()    // implicit typecast operator
		{
            MutexLocker lock(&_m);
			T ret = _x;
			return ret;
		}

	template<class T>
		inline void ProtectedPtr<T>::print_T( std::ostream& ostr )
		{
            MutexLocker lock(&_m);
			ostr << _x;
		}

	extern std::ostream& operator<< ( std::ostream& ostr, ProtectedPtr<int>& pi );

	/***********************************************************************
	 * ProtectedVarEnum<T>
	 * - protects enum variable with a mutex
	 ***********************************************************************/

	template<class T> class ProtectedVarEnum
	{
		T       _x;
		mutable Mutex _m;
	public:
		ProtectedVarEnum( T i )
			:_x(i)
		{
		}

		ProtectedVarEnum()
		{
			_x=(T) 0;
		}

		ProtectedVarEnum<T>& operator=( T i )
		{
            MutexLocker lock(&_m);
			_x = i;
			return *this;
		}

		bool operator==( T i ) const
		{
            MutexLocker lock(&_m);
			return ( _x == i );
		}

		operator T() const    // implicit typecast operator
		{
            MutexLocker lock(&_m);
			T ret = _x;
			return ret;
		}
	};

} // namespace BaseLib

#endif //

