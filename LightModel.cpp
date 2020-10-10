#include "LightModel.h"
#include "raytrace.h"//temp
#include "Shape.h"
#include <iostream>

const bool NO_SHADOWS=false;
const double SURVIVAL_PROB = 0.8;


 V  LightModel::GetColor(const Hit & hit,const V & startPos)
{
	return V(1,1,1);
}



std::vector<Light*> LightModel::GetVisibleLights(const V & startPos)
{
	std::vector<Light*>  templightList;
	if(NO_SHADOWS)
		return SCENE->lightList;
	templightList.clear();

	unsigned int i = ((unsigned int)(erand48(Scene::Xi)))*SCENE->lightList.size();


	double t1=2000.0;
	Hit shadowHit;
	Ray shadowRay;
	V hitToLightDir=SCENE->lightList[i]->GetPos()- startPos ;

	double ldist=hitToLightDir.length();
	double d= static_cast<Sphere*>(SCENE->lightList[i]->GetShape())->GetRadius()*erand48(Scene::Xi)-FLT_EPSILON;
	if(d<0.0) d=0.0;
	double asjacent = sqrt(ldist*ldist+d*d);
	double costheta=ldist/asjacent;
	hitToLightDir=SCENE->sample(hitToLightDir,costheta,2*PI*erand48(Scene::Xi));
	
	hitToLightDir=hitToLightDir.unit();

	shadowRay.SetDir(hitToLightDir);
	V hackedPoint;
		
	hackedPoint=startPos + hitToLightDir*FLT_EPSILON;
	shadowRay.SetOrigin(hackedPoint);



	std::vector<Shape*>::iterator it=SCENE->shapeList.begin();
	for (;it!=SCENE->shapeList.end();++it)
	{
		(*it)->Intersect(shadowRay,t1, shadowHit);
	}
	if(shadowHit.Obj && shadowHit.Obj->GetIsLight())
	{
		templightList.push_back(SCENE->lightList[i]);
	}
	return templightList;

}

PhongBRDF::choice PhongBRDF::ChoseRandomProperty()
{
	double j=erand48(Scene::Xi);
	double prob1,prob2;
	prob1=Kd/K_Sum;
	prob2=(Kd+Ks)/K_Sum;

	if (j<(prob1))
	{
		return PhongBRDF::DIFFUSE;
	}
	else if (j<(prob2))
	{

		return PhongBRDF::SPECULAR;
	}
	else
	{
		return PhongBRDF::TRANSMISSION;
	}
	
	
}

double PhongBRDF::RGBtoYUV(V& v)
{
	double d;
	d=v[0]*0.299+v[1]*0.587+v[2]*0.114;
	return d;
}

 V PhongBRDF::CalcDiffuse(Light* light,const Hit &hit)
{
	
	V lightVec=light->GetPos() - hit.Position;
	lightVec=lightVec.unit();
	double nDotL=max(0.0,hit.Normal.dot(lightVec));
	V reflection = (hit.Normal)*2.0*nDotL-lightVec;
	V color = diffuse * (nDotL*nDotL) * light->GetDiffuse() * ONE_PI;
	return color;
}
V PhongBRDF::CalcSpecular(Light* light,const Hit &hit,const V& viewDir)
{
	V lightVec=light->GetPos() - hit.Position;
	lightVec=lightVec.unit();
	double nDotL=max(0.0,hit.Normal.dot(lightVec));
	V H = (lightVec+viewDir).unit();
	double hDotN=H.dot(hit.Normal);
	V color = specular * pow( hDotN, shininess) * light->GetDiffuse() * 0.5 * ONE_PI * (shininess + 2.0) *nDotL ;
	return color;
}
 
V PhongBRDF::GetDirectColor(const Hit & hit,const V& viewDir)
 {
	V color;
	std::vector<Light*>  lightList = GetVisibleLights(hit.Position);
	std::vector<Light*>::iterator it1=lightList.begin();

	for (;it1!=lightList.end();++it1)
	{

		color+=CalcDiffuse((*it1),hit);
		color+=CalcSpecular((*it1),hit,viewDir);
		color=color*SCENE->lightList.size();

	}
	return color;
 }

 bool PhongBRDF::Snell( double n, const V& I, const V& normal, V& T ) 
 {
	double NdotI = normal.dot(I);
	double sqrtBase = 1-n*n*( 1-NdotI*NdotI );
	if( sqrtBase<0 ) return false;
	else T = ( ( normal*NdotI - I )*n - normal*sqrt( sqrtBase ) ).unit();
	return true;
}

V PhongBRDF::GetInDirectColor(const Hit & hit,const V& viewDir)
{
	choice mychoice=ChoseRandomProperty();
	V color;
	V R;
	if( erand48(Scene::Xi) < SURVIVAL_PROB )
	{
		Ray reflectionRay;
		if(mychoice==DIFFUSE)
		{
			R=SCENE->sample(hit.Normal,sqrt(erand48(Scene::Xi)),2.0*PI*erand48(Scene::Xi));
		}
		else
		{
			if(mychoice==TRANSMISSION)
			{
				V I=-viewDir;

				V Normal=hit.Normal;
				double etar;
				R=I;
				if(I.dot(Normal)>0)//inside sphere
				{
					etar=ior;
					Normal=V(-1,-1,-1)*Normal;
				}
				else
				{
					etar=1.0/ior;
				}
				if( !Snell( etar, viewDir, Normal, I) )
				{
					return color;
				}
				else
				{
					R=I;
				}
				double power = pow(erand48(Scene::Xi),(1.0/(shininess+1.0)));
				R=SCENE->sample(R,power,2.0*PI*erand48(Scene::Xi));
				R=R.unit();

			}
			else if (mychoice==SPECULAR)
			{
				R = hit.Normal*2.0*viewDir.dot(hit.Normal) - viewDir;
				R=R.unit();
				double power = pow(erand48(Scene::Xi),(1.0/(shininess+1.0)));
				R=SCENE->sample(R,power,2.0*PI*erand48(Scene::Xi));
				R=R.unit();
			}
		}

		reflectionRay.SetDir(R);
		reflectionRay.SetOrigin(hit.Position + R * FLT_EPSILON);
		Hit reflectionHit;
		if( SCENE->RayTrace( reflectionRay, reflectionHit ) )
		{	
			if(!reflectionHit.Obj->GetIsLight())
			{
				color += reflectionHit.Obj->GetLightModel()->GetColor(reflectionHit,-R);
			}
		}
	}
	if(mychoice==DIFFUSE)
	{
		color = diffuse * color /(Kd/K_Sum);
	}
	else if (mychoice==SPECULAR)
	{
		color = specular * color /(Ks/K_Sum);
	}
	else if (mychoice==TRANSMISSION)
	{
		color = Kt * color /(Kt_Y/K_Sum);
	}
	return color;
}



 V PhongBRDF::GetColor(const Hit & hit,const V& viewDir)
{
	V color;
	color+=GetDirectColor(hit,viewDir);
	color+=GetInDirectColor(hit,viewDir);
	return color/SURVIVAL_PROB;
}



