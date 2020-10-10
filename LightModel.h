#ifndef LIGHTMODEL_H
#define LIGHTMODEL_H

#include "vector.h"
#include "raytrace.h"
#include "hit.h"

class LightModel 
{
	public:
		virtual const V & GetDiffuse(){return diffuse ;}
		std::vector<Light*> GetVisibleLights(const V& pos);
		virtual ~LightModel(){};
		virtual V  GetColor(const Hit & hit,const V& startPos);
	protected:
		V diffuse;
};


class PhongBRDF : public LightModel
{
	enum choice{DIFFUSE,SPECULAR, TRANSMISSION};
	public:	
		
		void SetDiffuse(const V & v){diffuse=v;}
		void SetSpecular(const V & v){specular=v;}
		void SetShininess(double v){shininess=v;}
		void SetKt(const V & v){Kt=v;}
		void SetIor(double v){ior=v;}
		V CalcDiffuse(Light* light,const Hit &hit);
		V CalcSpecular(Light* light,const Hit &hit,const V& viewDir);
		V GetColor(const Hit & hit,const V& viewdirection);
		V GetDirectColor(const Hit & hit,const V& viewDir);
		V GetInDirectColor(const Hit & hit,const V& viewDir);
		choice ChoseRandomProperty();
		double RGBtoYUV(V &v);
		void SetKd(double v){Kd=v;}
		void SetKs(double v){Ks=v;}
		void SetKt_Y(double v){Kt_Y=v;}
		void SetKsum(){K_Sum=Kd+Ks+Kt_Y;}
		bool Snell( double n, const V& I, const V& normal, V& T ); 
		
	private:
		double shininess;
		V specular;
		V Kt;
		double K_Sum;
		double Kd;
		double Ks;
		double Kt_Y;
		double ior;
};



#endif