#include"NcUtility\vector2d.h"


 double& vector2d::operator [](const int index)
{
	return const_cast<double&>(static_cast<const vector2d&>(*this)[index]);
	
}

const double& vector2d::operator[](const int index) const
{
	
	if(index == 0) return v[0];
	else if(index == 1) return v[1];
	/*if(index == 0) return vx;
	else if(index == 1) return vy;
	else if(index == 2) return vz;*/
}