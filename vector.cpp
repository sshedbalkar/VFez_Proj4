///////////////////////////////////////////////////////////////////////
// Provides 3-vector and quaternion classes.
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "vector.h"

Q QuaternionAA(const double angle, const V& v)
{
    double l = v.length();
    if (l < 1e-4)
        return Q(1,0,0,0);
    else
        return Q(cos(angle/2.0), v*sin(angle/2.0)/l);
}

Q QuaternionV2Z(const V& v)
{
    V z = V(0,0,1);
    V r = z.cross(v);
    double a = acos(v.dot(z)/v.length());
    return QuaternionAA(a, r);
}

const double & V::operator[](const unsigned int i) const
{
	if(i>=0 && i<4) 
	{
		if (i==0) return x;
		if (i==1) return y;
		if (i==2) return z;

	}
	return -1.0;
}



 
