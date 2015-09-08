#include"NcUtility\vector2d.h"


 double& vector2d::operator [](const int index)
{
	if(index == 0) return v[0];
	else if(index == 1) return v[1];
	
}

const double& vector2d::operator[](const int index) const
{
	return (*this)[index];
	/*if(index == 0) return vx;
	else if(index == 1) return vy;
	else if(index == 2) return vz;*/
}