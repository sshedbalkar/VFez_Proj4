#ifndef SHAPE_H
#define SHAPE_H

#include "vector.h"
#include "raytrace.h"
#include "hit.h"
#include "LightModel.h"

enum SHAPE_TYPE{SPHERE,BOX,TRIANGLE,CYLINDER, NOSHAPE};
class Shape 
{
	public:
		Shape(){isLight=false;shapeType=NOSHAPE;}
		virtual bool Intersect(Ray & r,  double &t, Hit& h)=0;
		void SetLighting( LightModel  *p){lm=p;}
		LightModel * GetLightModel() { return lm; }
		const V & GetPos() const { return pos; }
		void SetPos(V);
		void SetIsLight(bool b){isLight=b;}
		bool GetIsLight(){return isLight;}
		SHAPE_TYPE shapeType;
	protected:
		virtual ~Shape(){};
		V pos;
		
		LightModel *lm;
		bool isLight;
	private:
		
		
};


class Sphere : public Shape
{
	public:	
		bool Intersect(Ray & r, double &t, Hit& h);
		void SetRadius(double);
		double GetRadius()  { return radius; }

	private:
		double radius;
		

};

class Box : public Shape
{
	public:	
		
		bool Intersect(Ray & r,  double &t, Hit& h);
		void SetMin(V v){min=v;}
		void SetMax(V v){max=v;}
		const V & GetMin() const { return min; }
		const V & GetMax() const { return max; }
	private:
		V min, max;
};


class Cylinder : public Shape
{
	public:	
		bool Intersect(Ray & r,  double &t, Hit& h);
		void SetBase(V v){p=v;}
		void SetTop(V v){q=v;}
		void SetRadius(double f ){r=f;}
		const V & GetBase() const { return p; }
		const V & GetTop() const { return q; }
		const double & GetRadius() const { return r; }
	private:
		V p,q;
		double r;
		

};

class Triangle : public Shape
{
	public:	
		bool Intersect(Ray & r,  double &t, Hit& h);
		void SetPoint1(V v){p1=v;}
		void SetPoint2(V v){p2=v;}
		void SetPoint3(V v){p3=v;}
		const V & GetP1() const { return p1; }
		const V & GetP2() const { return p2; }
		const V & GetP3() const { return p3; }
		V E1,E2,Normal;
	private:
		V p1,p2,p3;

};

#endif