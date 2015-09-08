#include <math.h>
#include "NcUtility\NcVector.h"

//NcVector::~NcVector()
//{}

const NcVector NcVector::operator - (const NcVector &rhs) const					//vector subtraction
{
	return NcVector(vx - rhs.x(),  vy - rhs.y(), vz - rhs.z());
}

const NcVector NcVector::operator + (const NcVector &rhs) const					//vector addition
{
    return NcVector(vx + rhs.x(),  vy + rhs.y(), vz + rhs.z());
}

double NcVector::operator * (const NcVector &rhs) const							//vector dot product
{
    return vx * rhs.x() + vy * rhs.y() + vz * rhs.z();
}

const NcVector NcVector::operator ^ (const NcVector& rhs) const					//vector cross product
{
    return NcVector(vy * rhs.z() - vz * rhs.y(),
                    vz * rhs.x() - vx * rhs.z() ,
                    vx * rhs.y() - vy * rhs.x());
}

const NcVector NcVector::operator * (double rhs) const							//scalar product
{
    return NcVector(vx * rhs,  vy * rhs,  vz * rhs);
}

double NcVector::length() const
{
	return sqrt(vx * vx + vy * vy + vz * vz);
}


double NcVector::normalize()												 //convert this vector to unit vector
{
    double norm = length();
    if (norm > 0.0)
    {
        double inv = 1.0/norm;
        vx *= inv;
        vy *= inv;
        vz *= inv;
    }                
    return( norm );
}


double /*NcVector::*/get_area(NcVector v0, NcVector v1, NcVector v2)
{
	NcVector e1, e2;
	e1 = v1 - v0;
	e2 = v2 - v0;
	return (e1 ^ e2).length() / 2.0;
}


//double NcVector::min(double a, double b, double c)
//{
//	double minimum;
//	minimum = a < b ? a : b;
//	minimum = minimum < c ? minimum : c;
//	return minimum;
//}
//	
//double NcVector::max(double a, double b, double c)
//{
//	double maximum;
//	maximum = a > b ? a : b;
//	maximum = maximum > c ? maximum : c;
//	return maximum;
//}


NcVector/* NcVector::*/get_v(double a[])
{
	return NcVector(a[0], a[1], 0.0);
}

double& NcVector::operator [](const int index)
{
	return const_cast<double&>(static_cast<const NcVector&>(*this)[index]);
}

const double& NcVector::operator[](const int index) const
{	if(index == 0) return vx;
	else if(index == 1) return vy;
	else if(index == 2) return vz;
	
	/*if(index == 0) return vx;
	else if(index == 1) return vy;
	else if(index == 2) return vz;*/
}