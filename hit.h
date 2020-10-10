#ifndef HIT_H
#define HIT_H
//
#include "vector.h"
//
class Shape;
class Hit
{
public:
	Hit();
	~Hit();
	//
public:
	V Normal;
	V Position;
	V Color;
	Shape* Obj;
	double t;
};
//
#endif