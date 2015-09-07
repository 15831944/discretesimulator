#ifndef NC_VECTOR
#define NC_VECTOR

#include "NcUtility\NcExportImport.h"


class NCUTILITYEXPORT NcVector
{
	
public:
	NcVector() {vx = 0.0; vy = 0.0; vz = 0.0;}
	NcVector(double x, double y, double z) {vx = x; vy = y; vz = z; }

	/*~NcVector();*/
	/*inline double x() const{return vx;}
	inline double y() const{return vy;}
	inline double z() const{return vz;}*/

	inline const double& x() const{return vx;}
	inline const double& y() const{return vy;}
	inline const double& z() const{return vz;}

	inline const NcVector operator - (const NcVector &rhs) const;		//vector subtraction
	inline const NcVector operator + (const NcVector &rhs) const;		//vector addition
    inline double operator * (const NcVector &rhs) const;				//vector dot product
	inline const NcVector operator ^ (const NcVector& rhs) const;		//vector cross product
	inline const NcVector operator * (double rhs) const;				//scalar product
	inline double& operator[](const int index);
	inline const double& operator[](const int index) const;
	inline double length() const;										//length of the vector
    
	


private:
	double vx;
	double vy;
	double vz;
	inline double normalize();		//convert this vector to unit vector
	
	

};


NCUTILITYEXPORT NcVector get_v(double []);
NCUTILITYEXPORT double get_area(NcVector v0, NcVector v1, NcVector v2);
#endif
