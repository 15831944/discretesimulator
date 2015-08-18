#ifndef NC_VECTOR
#define NC_VECTOR

#include "NcUtility\NcUtilityGlobal.h"


class NCUTILITYEXPORT NcVector
{
	
public:
	NcVector() {vx = 0.0; vy = 0.0; vz = 0.0;}
	NcVector(double x, double y, double z) {vx = x; vy = y; vz = z; }

	~NcVector();
	inline double x() const;
	inline double y() const;
	inline double z() const;

	inline const NcVector operator - (const NcVector &rhs) const;		//vector subtraction
	inline const NcVector operator + (const NcVector &rhs) const;		//vector addition
    inline double operator * (const NcVector &rhs) const;				//vector dot product
	inline const NcVector operator ^ (const NcVector& rhs) const;		//vector cross product
	inline const NcVector operator * (double rhs) const;				//scalar product
    inline double length() const;										//length of the vector
	inline double normalize();											//convert this vector to unit vector
    static NcVector get_v(double []);
	static double get_area(NcVector v0, NcVector v1, NcVector v2);


	double vx;
	double vy;
	double vz;

};

#endif
