///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#ifndef RAYTRACE_H
#define RAYTRACE_H
////////////////////////////////////////////////////////////////////////////////
#define EPSILON 0.00001

#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include "hit.h"
#include <random>
#include "vector.h"
using namespace std;


const double PI = 3.14159;
const double ONE_PI = 0.31830988618;

class Shape;
extern std::mt19937_64 gen;
extern std::uniform_real_distribution<> random;
extern double erand48(unsigned short int xsubi[3]);
class Ray 
{
	private:
		V m_Origin;
		V m_Dir;
	public:
		void SetDir(V v) { m_Dir=v; }
		void SetOrigin(V v) { m_Origin=v; }
		const V & GetOrigin() const { return m_Origin; }
		const V & GetDir() const { return m_Dir; }
};

class Light {
	private:
		V m_pos;
		V m_Id;
		Shape * shape;
	public:
		Light(){};
		Light(const Light*l)
		{
			m_pos=l->m_pos;
			m_Id=l->m_Id;
		}
		Shape * GetShape(){return shape;}
		void SetShape(Shape *s){shape=s;}
		const V & GetPos() const { return m_pos; }
		const V & GetDiffuse() const { return m_Id; }
		void SetPos(V v) {  m_pos=v; }
		void SetDiffuse(V v) { m_Id=v;}
};



class Camera
{
	private:
		V m_eyePos;
		Q m_orientation;
		double ry,rx;
	public:
		void SetEyePos(V v) { m_eyePos=v; }
		const V & GetEyePos() const { return m_eyePos; }
		void SetCameraOrientation(Q q) { m_orientation=q; }
		const Q & GetOrientation() const { return m_orientation; }
		void SetRy(double t){ry=t;}
		const double & GetRy() const { return ry; }
		void SetRx(double t){rx=t;}
		const double & GetRx() const { return rx; }
};


class Scene 
{
	public:
		Scene();
		static unsigned short Xi[3];
		bool RayTrace( Ray& ray, Hit& hit);
		int width, height;
		std::vector<Shape*> shapeList;
		std::vector<Light*> lightList;
		V ambient;
		Camera camera;
		void Command(const string c, const vector<double> f,
					 const vector<string> strings);

		V sample(V N,double  cTheta,double Phi)
		{
			double sTheta = sqrt(1- cTheta*cTheta);		 // Sine of Theta
			V k = V(sTheta*cos(Phi), sTheta*sin(Phi), cTheta);  // Vector centered on Z
			Q q = QuaternionV2Z(N).conj();	 // Quaternion to rotate Z to N	return q.rotate(k)				
			return q.rotate(k); // k rotated to N's frame
		}

		void TraceImage(V* image, const int pass);
	private:
		
};

extern Scene* SCENE;

#endif