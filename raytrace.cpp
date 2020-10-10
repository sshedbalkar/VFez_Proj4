///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "raytrace.h"
#include "Shape.h"
#include "hit.h"
#include "LightModel.h"

#ifdef _WIN32
    // Includes for Windows
    #include <windows.h>
    #include <cstdlib>
    #include <limits>
    #include <crtdbg.h>
    std::mt19937_64 gen;
    std::uniform_real_distribution<> random(0.0, 1.0);
    double erand48(unsigned short int xsubi[3]) { return random(gen); }
#else
    // Includes for Linux
    #include <stdlib.h>
    double abs(double v) { return v>0 ? v : -v; }
#endif

enum flag{NOTHING=0,LIGHT,PHONG,PHONG_BRDF,SHIRLEY,DIELECTRIC};
flag state=NOTHING;
Light *_l = new Light;
LightModel * _p;

Scene* SCENE=0;

Scene::Scene(){SCENE=this;}

void Scene::Command(const string c, const vector<double> f, const vector<string> strings)
{
    if (c == "screen") {
        width = int(f[0]);
        height = int(f[1]); }
	else if(c == "camera")
	{
		V v( f[0], f[1], f[2]);
		camera.SetEyePos (v);
		Q q(f[3],f[4],f[5],f[6]);
		camera.SetCameraOrientation(q);
		camera.SetRy(f[7]);

		double f=((double)width/(double)height)*camera.GetRy();
		camera.SetRx(f);
	}
	
	else if(c == "ambient")
	{
		V v(f[0],f[1],f[2]);
		ambient=v;
	}
	else if(c == "light")
	{
		state=LIGHT;
		V v(f[0],f[1],f[2]);
		_l->SetDiffuse(v);
	}
	else if(c == "sphere")
	{
		if (state==LIGHT)
		{
			Sphere* obj = new Sphere();
			Light *l=new Light(_l);
			V v(f[0],f[1],f[2]);
			l->SetPos(v);
			obj->SetPos(v);
			obj->SetRadius(f[3]);
			LightModel * p = new LightModel;
			obj->SetLighting(p);
			obj->SetIsLight(true);
			obj->shapeType=SPHERE;
			shapeList.push_back(obj);
			lightList.push_back(l);
			l->SetShape(obj);
		}
		else if(state==PHONG || state==PHONG_BRDF || state==SHIRLEY || state==DIELECTRIC)
		{
			Sphere* obj = new Sphere();
			V v(f[0],f[1],f[2]);
			obj->SetPos(v);
			obj->shapeType=SPHERE;
			obj->SetRadius(f[3]);
			obj->SetLighting(_p);
			shapeList.push_back(obj);
		}
		
	}
	
	else if(c == "phongbrdf")
	{
		state=PHONG_BRDF;
		V v(f[0],f[1],f[2]);
		
		PhongBRDF * p1 = new PhongBRDF;
		p1->SetDiffuse(v);
		p1->SetKd(p1->RGBtoYUV(v));
		v=V(f[3],f[4],f[5]);
		p1->SetSpecular(v);
		p1->SetKs(p1->RGBtoYUV(v));
		double d=f[6];
		p1->SetShininess(d);
		_p=p1;
		v=V(f[7],f[8],f[9]);
		p1->SetKt(v);
		p1->SetKt_Y(p1->RGBtoYUV(v));
		d=f[10];
		p1->SetIor(d);
		p1->SetKsum();
		
	}
	
	else if(c == "box")
	{
		if (state==PHONG || state==PHONG_BRDF || state==SHIRLEY )
		{
			Box* obj = new Box();
			V v(f[0],f[1],f[2]);
			obj->SetMin(v);
			
			V b=V(f[3],f[4],f[5]);
			obj->SetMax(obj->GetMin()+b);

			obj->SetPos( ((obj->GetMax() + obj->GetMin())*0.5) );
			obj->shapeType=BOX;
			obj->SetLighting(_p);
			shapeList.push_back(obj);
		}

	}
	else if(c == "cylinder")
	{
		if (state==PHONG || state==PHONG_BRDF || state==SHIRLEY)
		{
			Cylinder* obj = new Cylinder();
			V v(f[0],f[1],f[2]);
			obj->SetBase (v);
			V b=V(f[3],f[4],f[5]);
			obj->SetTop(v+b);
			obj->SetRadius(f[6]);
			obj->SetPos((obj->GetTop()+obj->GetBase())*0.5);
			obj->SetLighting(_p);
			obj->shapeType=CYLINDER;

			shapeList.push_back(obj);
		}

	}
	else if(c == "triangle")
	{
		if (state==PHONG || state==PHONG_BRDF || state==SHIRLEY)
		{
			Triangle* obj = new Triangle();
			V v(f[0],f[1],f[2]);
			obj->SetPoint1(v);
			v=V(f[3],f[4],f[5]);
			obj->SetPoint2(v);
			v=V(f[6],f[7],f[8]);
			obj->shapeType=TRIANGLE;
			obj->SetPoint3(v);
			obj->E1 = obj->GetP2() - obj->GetP1();
			obj->E2 = obj->GetP3() - obj->GetP1();
			obj->Normal = obj->E2.cross( obj->E1 ).unit();
			obj->SetLighting(_p);
			shapeList.push_back(obj);
		}

	}
    else {
        fprintf(stderr, "\n*********************************************\n");
        fprintf(stderr, "* Unknown command: %s\n", c.c_str());
        fprintf(stderr, "*********************************************\n\n"); }
}


bool Scene::RayTrace( Ray& ray, Hit& hit )
{
	double t=2000.0;
	std::vector<Shape*>::iterator it=shapeList.begin();
	for (;it!=shapeList.end();++it)
	{
		(*it)->Intersect(ray,t, hit);
	}
	
	if(hit.Obj) 
	{
		return true;
	}
	return false;
	
}
unsigned short Scene::Xi[3];


void Scene::TraceImage(V* image, const int pass)
{
	double dx,dy;

	V X,Y,Z,temp;
	temp = V(camera.GetRx(),0,0);
	Q q = camera.GetOrientation();
	q = q.conj();
	X = q.rotate(temp);
	temp = V(0,camera.GetRy(),0);
	Y = q.rotate(temp);
	temp = V(0,0,-1);
	Z = q.rotate(temp);
	
		
	#pragma omp parallel for schedule(dynamic, 1) private(Xi)
	for (int y = 0; y < height; ++y) 
	{ 
		for (int x = 0; x < width; ++x) 
		{

			Xi[0] = pass;
			Xi[1] = x*pass*pass;
			Xi[2] = x*x*pass;

			V color(0.0, 0.0, 0.0);
			dx = (2.0*(x+erand48(Xi))/width)-1.0;
			dy = (2.0*(y+erand48(Xi))/height)-1.0;
			Hit hit;
			Ray viewRay;
			V v(X*dx+Y*dy+Z);
			viewRay.SetDir(v.unit());
			viewRay.SetOrigin(camera.GetEyePos());

			std::vector<Shape*>::iterator it=shapeList.begin();
			double t=2000.0;
			for (;it!=shapeList.end();++it)
			{
				(*it)->Intersect(viewRay,t, hit);
			}
	
			if(!hit.Obj) 
			{
				continue;
			}
			V viewdir=SCENE->camera.GetEyePos()-hit.Position;
			viewdir=viewdir.unit();
			color+=hit.Obj->GetLightModel()->GetColor(hit,viewdir);
			image[y*width + x] += color ;

		}
	}
}






