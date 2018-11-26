/***************************************************************************
                          vivaldi.h -  description
                             -------------------
    begin                : Thu Jul 19 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef NetworkInformationLib_VivaldiCore_IsIncluded 
#define NetworkInformationLib_VivaldiCore_IsIncluded

#include <iostream>

#ifdef USE_GCC
#include <sys/utsname.h>
#endif

#include <vector>
#include <assert.h>
#include <cstdlib>
#include <math.h>
#include <cmath>
#include <limits>

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{

namespace VivaldiSpace 
{
	static double cc=0.25;
	static double ce=0.5;
	static double maxNodeErr=1.0;
	static double minNodeErr=0.0;

	//double randomDouble(double max);
	//int randomInt(double max);


	static double randomDouble(double max)
	{
		return (max * rand() / (RAND_MAX + 1.0));
	}

	static int randomInt(double max)
	{
		return (int) (max * rand() / (RAND_MAX + 1.0));
	}

} // namespace VivaldiSpace

/*-----------------------------------------------------------------------
					class Coordinates
------------------------------------------------------------------------- */

class DLL_STATE Coordinates
{
public:

	typedef std::vector<double> Vector;
		
public:   
	Coordinates() 
	{
 	    l_one_norm = false;
		l_inf_norm = false;
    }
 	
    Coordinates(const Vector &coordinates) 
	{
 	    l_one_norm = false;
		l_inf_norm = false;

        this->coordinates = coordinates;
    }
    
    inline bool add(const Coordinates &remoteNode, Coordinates &newCoord) 
	{
        Vector remoteCoords = remoteNode.getCoordinates();
        if(remoteCoords.size()!= this->coordinates.size()) return false;

		Vector sum = Vector(remoteCoords.size());
        for(int i=0;i<remoteCoords.size();i++) 
		{
            double first_c = coordinates[i]; 
            double second_c = remoteCoords[i]; 
			sum[i] = first_c+second_c;
        }

		newCoord = Coordinates(sum);
		return true;
    }

    inline bool substract(const Coordinates &remoteNode, Coordinates &newCoord) 
	{
        Vector remoteCoords = remoteNode.getCoordinates();
        if(remoteCoords.size()!= this->coordinates.size()) return false;
     
	 	Vector diff = Vector(remoteCoords.size());
        for(int i=0;i<remoteCoords.size();i++) 
		{
            double first_c = coordinates[i]; 
            double second_c = remoteCoords[i]; 

			diff[i] = first_c-second_c;
        }
     
	 	newCoord = Coordinates(diff);
		return true;
    }
    
	inline double norm() 
	{
        double sum=0;
        if(l_inf_norm)
        {
          	double temp=0.0f;  
			for(int i=0;i<coordinates.size();i++)
          	{
				double coords = std::abs(coordinates[i]); 
            	if(temp<coords) temp=coords;
          	}
          	return temp;
        }   
        else
        {
        	if(l_one_norm)
        	{
          		double temp=0.0f;  
          		for(int i=0;i<coordinates.size();i++)
          		{
            		double coords = std::abs(coordinates[i]); 
            		temp+=coords;
          		}
          		return temp;
	        }
    	    else
        	{    
        		for(int i=0;i<coordinates.size();i++) 
				{
            		double coords = coordinates[i]; 
            		sum = sum + coords*coords;
        		}
		        return (double)std::sqrt(sum);
        	}
        }
    }
	
   
    inline double distance(Coordinates remoteNode) 
	{
        Coordinates diff;
		bool success = substract(remoteNode, diff);
		assert(success);

        return norm(diff);
    }
    
    inline Coordinates multiply(double scale) 
	{
        Vector coord = Vector(coordinates.size());
        for(int i=0;i<coordinates.size();i++) 
		{
            double x = scale * coordinates[i]; 
			coord[i] = x;
        }
        return Coordinates(coord);
    }

	inline void adjustCoordinates(int scale) 
	{
        Vector v = Vector(coordinates.size());
        //cerr << coordinates.size() << endl;
        for(int i=0;i<coordinates.size();i++) 
		{
			double newPosition = coordinates[i] + scale * VivaldiSpace::randomDouble(1.0f); 
            //cerr << "New Positions " << newPosition << endl;
			v[i] = newPosition;
        }
        this->setCoordinates(v);
    }
    
	inline Coordinates getDirectonality(Coordinates remoteNode) 
	{
        Coordinates diff;
		bool success = substract(remoteNode, diff);
		assert(success);

        //print(diff, cerr); 
        double norm = diff.norm();
        //cerr << norm << endl;
        while(norm == 0.0) 
		{
            adjustCoordinates(1);
            bool success = substract(remoteNode, diff);
			assert(success);

            norm = diff.norm();
        }
        
        diff = diff.multiply(1/norm);
        return diff;
    }
    
	inline bool isValid() 
	{
        Vector v = this->getCoordinates() ;
        for(int i=0;i<v.size();i++) 
		{
            double val = v[i]; 
			if(val >= (std::numeric_limits<double>::max)() || val < 0)
                return false;
        }
        return true;
    }
    
	// get/set/is
    inline void setL_inf_norm(bool val) 					{ l_inf_norm = val; }
    inline void setL_one_norm(bool val)						{ l_one_norm =val; }
    inline bool isL_inf_norm() 						const 	{ return l_inf_norm; }
    inline bool isL_one_norm() 						const 	{ return l_one_norm; }

	inline Vector getCoordinates() 					const 	{ return coordinates; }
    inline void setCoordinates(Vector coordinates) 		 	{ this->coordinates = coordinates; }
	
	inline void print(const Coordinates &coord, ostream& ostr) 
	{
		ostr << "(";
    	for (int i=0;i<coord.getCoordinates().size();i++)
        	 ostr << coord.getCoordinates()[i] << "\t" ;
		ostr << ")" << endl;
	}
	
	inline void print(ostream& ostr) 
	{
		ostr << "(";
    	for (int i=0;i< this->getCoordinates().size();i++)
        	 ostr << this->getCoordinates()[i] << "\t" ;
		ostr << ")" << endl;
	}
	inline Vector getCoordinates() { return this->coordinates; }


	friend std::ostream& operator<<(std::ostream& ostr, const Coordinates &coord)
	{
		ostr << "(";
		for (int i=0;i<coord.getCoordinates().size();i++)
			ostr << coord.getCoordinates()[i] << "\t" ;
		ostr << ")";
		return ostr;
	}


private:
	
    inline double norm(Coordinates node) 
	{
        double sum=0;
        Vector coordinates = node.getCoordinates();
        if(l_inf_norm)
        {
			double temp=0.0f;  
			for(int i=0;i<coordinates.size();i++)
          	{
            	double coords = std::abs(coordinates[i]); 
            	if(temp<coords) temp=coords;
			}
    	    return temp;
        }   
        else
        {
        	if(l_one_norm)
        	{
          		double temp=0.0f;  
          		for(int i=0;i<coordinates.size();i++)
          		{
            		double coords = std::abs(coordinates[i]); 
            		temp+=coords;
          		}
				return temp;
	        }
    	    else
    	    {    
        		for(int i=0;i<coordinates.size();i++) 
				{
            		double coords = coordinates[i]; 
            		sum = sum + coords*coords;
	        	}
    		    return (double)std::sqrt(sum);
        	}
        }
    }

private:
	
    bool l_one_norm;
    bool l_inf_norm;
    
	Vector coordinates;
};

//std::ostream& operator<<(std::ostream& ostr, const Coordinates &coord);

/*-----------------------------------------------------------------------
				classes for node (vertex) and edge data
------------------------------------------------------------------------- */
struct DLL_STATE VVNodeData : public MicroMiddleware::RmiObjectBase
{
public:
	VVNodeData() : id(0), err(0.0f), rtt(0.0f)
	{}
	VVNodeData(int nodeId, int dimension) : id(nodeId), err(0.0f), rtt(0.0f)
	{
		if(dimension <= 0) 
		{
			throw("VVNodeData(nodeId, dimension): ERROR! Dimension is <= 0!");
		}
		assert(dimension > 0);
		assert(nodeId >= 0);

		for(int i = 0; i < dimension; i++)
			coords.push_back(0.0f);
	}
	~VVNodeData()
	{}

public:
	inline void SetNodeId(const int &nodeId) 					{ id = nodeId; }
	inline void SetCoords(const std::vector<double> &c)			{ coords = c; }
	inline void SetErr(const double &e)							{ err = e; }
	inline void SetRtt(const double &r)							{ rtt = r; }

	inline int GetNodeId()								const	{ return id; }
	inline const std::vector<double>& GetCoords()		const  	{ return coords; }
	inline double GetErr()								const 	{ return err; }
	inline double GetRtt()								const 	{ return rtt; }

public:
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteInt32(id);
		writer->WriteDouble(err);
		writer->WriteDouble(rtt);
	
		writer->WriteInt32(coords.size());
		for(std::vector<double>::iterator it = coords.begin(), it_end = coords.end(); it != it_end; ++it)
			writer->WriteDouble(*it);
	}
	
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		id = reader->ReadInt32();
		err = reader->ReadDouble();
		rtt = reader->ReadDouble();

		int sz = reader->ReadInt32();
		for(int i = 0; i < sz; i++)
			coords.push_back(reader->ReadDouble());
	}

	virtual char* toRaw()
	{
		return NULL;
	}

	friend std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords)
	{
		ostr << "(" ;
		for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
		{
			ostr << *vit << "," ;
		}
		ostr << ")"; 

		return ostr;
	}

	friend std::ostream& operator<<(std::ostream& ostr, const VVNodeData &vv)
	{
		ostr << "id " << vv.GetNodeId() << " err " << vv.GetErr() << " rtt " << vv.GetRtt() << " coords "; // << vv.GetCoords();
		
		const std::vector<double> &coords = vv.GetCoords();
		ostr << "(" ;
		for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
		{
			ostr << *vit << "," ;
		}
		ostr << ")"; 
		
		return ostr;
	}

public:
	int id;
	double err;
	double rtt;
	std::vector<double> coords;
};

struct DLL_STATE VVEdgeData
{
public:
	VVEdgeData() : id(-1,-1), weight(0.0f)
	{}
	VVEdgeData(std::pair<int, int> edgeId, double w) : id(edgeId), weight(w)
	{}
	~VVEdgeData()
	{}

public:
	inline void SetEdgeId(const std::pair<int, int>  &edgeId) 	{ id = edgeId; }
	inline void SetWeight(const double &w)						{ weight = w; }

	inline const std::pair<int, int>& GetEdgeId()		const	{ return id; }
	inline double GetWeight()							const 	{ return weight; }

public:
	std::pair<int, int> id;
	double weight;
};

/*-----------------------------------------------------------------------
				class VivaldiNode
------------------------------------------------------------------------- */
class DLL_STATE VivaldiNode
{
public:
	typedef std::vector<double> Vector;

public:

    VivaldiNode() : myError(0), coords() {}
    VivaldiNode(int dimensions, double initialError) 
	{
        myError = initialError;
        bootstrap(dimensions);
    }
    
    inline void setCoords(Coordinates coords) 
	{
        this->coords = coords;
    }
    
    inline void bootstrap(int dimensions) 
	{
		Vector coordinates = Vector(dimensions);
		for(int i = 0; i < dimensions; i++)
			coordinates[i] = 0.0f; 
        this->setCoords(Coordinates(coordinates));
    }
	
	inline void update(const VVNodeData &params)
	{
        double rtt 					 = params.rtt; 
        double remoteNodeErr 		 = params.err; 
		Coordinates remoteCoordinate = Coordinates(params.coords);
        
        if(isValid(rtt) && remoteCoordinate.isValid()) 
		{
            if(rtt < 0) return;
            if(myError + remoteNodeErr == 0) return;
			
            double w  		= myError/(myError+remoteNodeErr);
            double diffErr 	= rtt-coords.distance(remoteCoordinate);
            double es 		= std::abs(diffErr)/rtt;
            double ce 		= VivaldiSpace::ce;
            double err 		= es*ce*w+myError*(1-ce*w);
            double delta 	= VivaldiSpace::cc*w;

			//cerr << " vivaldi node updates : " << endl;
			//cerr << " Delta: " << delta << endl;
            
            Coordinates coord1 = coords.getDirectonality(remoteCoordinate);
            //coord1.print(cerr); 
            coord1 = coord1.multiply(delta*diffErr);
			//coord1.print(cerr); 
            Coordinates afterAdjustment;
			bool success = coords.add(coord1, afterAdjustment);
			assert(success);
			//afterAdjustment.print(cerr);            
			
			if(isValid(err) && afterAdjustment.isValid()) 
			{
                coords = afterAdjustment;
                double minErr = VivaldiSpace::minNodeErr;
                double maxErr = VivaldiSpace::maxNodeErr;
				if(err>=minErr)
					if(err<=maxErr) myError = err;
					else myError = maxErr;
				else myError = minErr;
            }
        }
        
        
    }
      
    inline Coordinates getCoords()	const { return coords;}
    inline double getMyError() 		const { return myError;}

	inline bool updateData(VVNodeData &nd)  
	{ 
		VVNodeData td;
		td.coords 	= coords.getCoordinates();
		td.err    	= getMyError();
		bool eq = true;

		if(td.err != nd.err) eq = false;
		
		for(Vector::iterator it = td.coords.begin(), it_end = td.coords.end(), 
							 it_in = nd.coords.begin(), it_in_end = nd.coords.end(); 
							 it != it_end && it_in != it_in_end && eq == true; 
							 ++it, ++it_in)
		{
			if(*it != *it_in) 
			{
				eq = false;
				break;
			}

			//cerr << "." ;
		}

		if(eq) return false;
		
		nd.coords = td.coords;
		nd.err    = td.err;
		
		return true;
	}
	
private:
	inline bool isValid(double val) 
	{
        //if(Double.isInfinite(val)|| Double.isNaN(val)||(val<0) )
		if(val >= (std::numeric_limits<double>::max)() || val < 0 ) return false;
        else return true;
    }

private:
	double myError;
    Coordinates coords;
};

} // end namespace NetworkInformationLib

#endif //PLANETLAB_VIVALDI_IMPLEMENTATION_KHV

