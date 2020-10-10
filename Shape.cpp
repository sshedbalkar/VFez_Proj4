#include "Shape.h"
#include "raytrace.h"//temp
#include <deque>


enum Axes {xAxis, yAxis, zAxis};

bool Sphere::Intersect(Ray & ray, double &tmin, Hit& hit)
{
	V sphereToRay = ray.GetOrigin() - GetPos();
	double a = ray.GetDir().dot( ray.GetDir());//always 1
	double b = 2 * sphereToRay.dot( ray.GetDir());
	double c = sphereToRay.dot( sphereToRay) - radius * radius;

	double delta = b * b - 4 * a * c;
	if( fabs(a) < DBL_EPSILON || delta < 0.0 ) return false;

	double t1 = (-b + sqrt(delta)) / (2 * a);
	double t2 = (-b - sqrt(delta)) / (2 * a);
	double t;
	if (t1 > 0.0f && t2 <= 0.0f) {
		t= t1;
	}else if (t2 > 0.0f && t1 <= 0.0f){
		t = t2;
	}else if( t1 <= 0 && t2 <= 0 ){
		return false;
	}else {
		t = (t1<t2)? t1: t2;
	}
	
	if(t<tmin)
	{
		tmin=t;
		hit.Position = ray.GetOrigin() + ray.GetDir() * t;
		hit.Normal = V( hit.Position - GetPos() ).unit();
		hit.Obj=this;
		hit.t=tmin;
	}

	
	return true;
}



//
//{
//	double t;
//	V m = r.GetOrigin () - pos;
//	V dir=r.GetDir();
//	double b = m.dot(dir);
//	double c = m.dot(m) - radius * radius;
//	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
//	if (c > 0.0f && b > 0.0f) return false;
//	double discr = b*b - c;
//	// A negative discriminant corresponds to ray missing sphere
//	if (discr < 0.0f) return false;
//	// Ray now found to intersect sphere, compute smallest t value of intersection
//	t = -b - sqrt(discr);
//	// If t is negative, ray started inside sphere so clamp t to zero
//	if (t < 0.0f) 
//	{
//		t = 0.0f;
//		return false;
//	}
//	//
//	if( t < tmin )
//	{
//		tmin = t;
//
//		//V dir_norm=viewRay.GetDir().unit();
//		h.Position = r.GetOrigin() +  r.GetDir() * tmin; 
//		
//		V N=h.Position-pos;
//
//		h.Normal=N.unit();
//		h.Obj = this;
//
//		////////////Populate the hit variable here with normal, color, etc
//	}
//	return true;
//

	//q = r.GetOrigin () + t * dir;
//}


void Shape::SetPos(V v)
{
	pos=v;
}

void Sphere::SetRadius(double r)
{
	radius=r;
}

bool Box::Intersect(Ray & r, double &t, Hit& h)
{
	double tmin =0; // set to -FLT_MAX to get first hit on line
	double tmax = FLT_MAX; // set to max distance ray can travel (for segment)
	// For all three slabs
	V p=r.GetOrigin();
	V d=r.GetDir();
	//d=d.unit();
	bool sign[3] = {false, false, false};
	Axes a=xAxis;
	for (int i = 0; i < 3; i++) {
		if (abs(d[i]) < EPSILON) {
		// Ray is parallel to slab. No hit if origin not within slab
		if (p[i] < min[i] || p[i] > max[i]) 
			return false;
		} else {
		// Compute intersection t value of ray with near and far plane of slab
		double ood = 1.0f / d[i];
		double t1 = (min[i] - p[i]) * ood;
		double t2 = (max[i] - p[i]) * ood;

		
		// Make t1 be intersection with near plane, t2 with far plane
		if (t1 > t2)
		{
			swap(t1, t2);
			if(a == zAxis)
				sign[0]=!sign[0];
			else if(a == yAxis)
				sign[1]=!sign[1];
			else if(a == zAxis)
				sign[2]=!sign[2];
			//else
			//	sign = 0;
		}
		// Compute the intersection of slab intersection intervals
		if (t1 > tmin)
		{
			tmin = t1;
			if(i==0)
				a = xAxis;
			else if(i==1)
				a = yAxis;
			else if(i==2)
				a = zAxis;
		}
		if (t2 < tmax) 
			tmax = t2;
		// Exit with no collision as soon as slab intersection becomes empty
		if (tmin > tmax) 
			return false;
		}
	}
	
	if(tmin < t)
	{
		t=tmin;
		//if(a != yAxis) return false;
		if(a == xAxis)
		{
			if(sign[0]) { 
				h.Normal = V(-1,0,0); }
			else { 
				h.Normal = V(1,0,0); }
		}
		
		
		if(a == yAxis)
		{
			if(sign[1])
			{ 
				h.Normal = V(0,1,0); 
			}
			else 
			{
				h.Normal = V(0,-1,0);
			}
		}
		

		if(a == zAxis)
		{
			if(sign[2])  { 
				h.Normal = V(0,0,-1); }
			else {
				h.Normal = V(0,0,1);  }
		}
		h.Position=p + d*tmin;
		h.Normal = h.Normal.unit();
		h.Obj = this;
		h.t=tmin;
	}
	return true;
}

class t_normalV{
public:
	double t;
	V normal;
	t_normalV( double _t, const V& _normal ): t(_t), normal(_normal){}
};

//
void cylinderHelperF(double &tmin,double t, Hit& h, Ray & r,Shape & p,int side, V endPoint)
{
	if(t<DBL_EPSILON) return;
	V w=r.GetOrigin() + endPoint * t; 
	V v= w - r.GetOrigin();
	t = v.length();

	if( t < tmin && tmin>0)
	{
		V N;
		tmin = t;
		h.Position=w;
	
		if(side==0)//between
		{
			V q;
			q=V(p.GetPos().x,p.GetPos().y,h.Position.z); 

			N=h.Position-q;
		}
		else if (side==1)//top
		{
			V q;
			q=V(p.GetPos().x,p.GetPos().y,h.Position.z); 
			N=q-p.GetPos();
		}
		else if (side==2)//botton
		{
			V q;
			q=V(p.GetPos().x,p.GetPos().y,h.Position.z); 
			N=q-p.GetPos();
		}
		h.Normal=N.unit();
		h.Obj = &p;
		h.t=tmin;
	}
}

bool Cylinder::Intersect(Ray &ray, double &tmin, Hit& h)
{
	double t;
	V p = GetBase();
	V q = GetTop();
	double r = GetRadius();

	V d = q - p;
	V m = ray.GetOrigin() - p;
	V tempEnd=ray.GetDir();
	tempEnd = ray.GetOrigin() + tempEnd*100.0;
	V n = tempEnd - ray.GetOrigin();

	double md = m.dot(d);
	double nd = n.dot(d);
	double dd = d.dot(d);
	
	// Test if segment fully outside either endcap of cylinder
	if (md < 0.0f && md + nd < 0.0f) 
		return 0; // Segment outside ’p’ side of cylinder
	if (md > dd && md + nd > dd) 
		return 0; // Segment outside ’q’ side of cylinder
	
	double nn = n.dot(n);
	double mn = m.dot(n);
	double a = (dd * nn) - (nd * nd);
	double k = m.dot(m) - (r * r);
	double c = (dd * k) - (md * md);

	if (abs(a) < EPSILON) 
	{
		// Segment runs parallel to cylinder axis
		if (c > 0.0f) return 0; // ’a’ and thus the segment lie outside cylinder
		// Now known that segment intersects cylinder; figure out how it intersects
		if (md < 0.0f) t = -mn / nn; // Intersect segment against ’p’ endcap --bottom
		else if (md > dd) t = (nd - mn) / nn; // Intersect segment against ’q’ endcap --top
		else t = 0.0f; // ’a’ lies inside cylinder
		cylinderHelperF(tmin,t,h,ray,*this,0,n);
		return true;
	}
	
	double b = (dd * mn) - (nd * md);
	double discr = (b * b) - (a * c);
	
	if (discr < 0.0f) 
		return 0; // No real roots; no intersection
	
	t = (-b - sqrt(discr)) / a;
	
		
	if (md + t * nd < 0.0f) 
	{
		// Intersection outside cylinder on ’p’ side
		if (nd <= 0.0f) 
			return 0; // Segment pointing away from endcap
	
		t = -md / nd;
		// Keep intersection if Dot(S(t) - p, S(t) - p) <= r∧2
		if(  k + 2 * t * (mn + t * nn) <= 0.0f)
		{
			cylinderHelperF(tmin,t,h,ray,*this,2,n);
			return true;
		}
		else
		{
			return false;
		}
		
	} 
	else 
	if (md + t * nd > dd) 
	{
		// Intersection outside cylinder on ’q’ side
		if (nd >= 0.0f) 
			return 0; // Segment pointing away from endcap
		t = (dd - md) / nd;
		// Keep intersection if Dot(S(t) - q, S(t) - q) <= r∧2
		if( k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f)
		{
			cylinderHelperF(tmin,t,h,ray,*this,1,n);
			return true;
		}
		else
			return false;
	}

	// Segment intersects cylinder between the endcaps; t is correct
	//
	cylinderHelperF(tmin,t,h,ray,*this,0,n);
	return true;
}

bool Triangle::Intersect(Ray & ray,  double &tmin, Hit& h)
{
	V p = ray.GetDir().cross( E2 );
	double d = p.dot( E1 );
	if( fabs(d) < DBL_EPSILON ) return false;
	V S = ray.GetOrigin() - p1;
	double u = p.dot( S )/d;
	if( u<0 || u>1 ) return false;
	V q = S.cross( E1 );
	double v = ray.GetDir().dot( q )/d;
	if( v<0 || u+v>1 ) return false;
	double t = E2.dot( q )/d;
	if( t < 0 ) return false;
	if(t<tmin)
	{
		tmin = t;
		h.Normal = Normal;
		h.Obj=this;
		h.Position=ray.GetOrigin() + ray.GetDir()*tmin;
		h.t=tmin;
	}
	
	return true;
}


//int IntersectSegmentTriangle(Point p, Point q, Point a, Point b, Point c,
//float &u, float &v, float &w, float &t)
//bool Triangle::Intersect(Ray & r,  double &tmin, Hit& h)
//{
//	double v,u,w;
//	double t;
//	V p= r.GetOrigin();
//
//	V ab = p2-p1;
//	V ac = p3-p1;
//	
//	V q = r.GetOrigin() + r.GetDir() *FLT_MAX;
//	V a,b,c;
//	a=p1;
//	b=p2;
//	c=p3;
//	//V n = tempEnd - ray.GetOrigin();
//	V ab = b - a;
//V ac = c - a;
//V qp = p - q;
//// Compute triangle normal. Can be precalculated or cached if
//// intersecting multiple segments against the same triangle
//V n =ab.cross(ac);
//// Compute denominator d. If d <= 0, segment is parallel to or points
//// away from triangle, so exit early
//float d =qp.dot(n);
//if (d <= 0.0f) return 0;
//// Compute intersection t value of pq with plane of triangle. A ray
//// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
//// dividing by d until intersection has been found to pierce triangle
//V ap = p - a;
//t = ap.dot( n);
//if (t < 0.0f) return 0;
//if (t > d) return 0; // For segment; exclude this code line for a ray test
//// Compute barycentric coordinate components and test if within bounds
//V e = qp.cross( ap);
//v = ac.dot( e);
//if (v < 0.0f || v > d) return 0;
//w = -ab.dot( e);
//if (w < 0.0f || v + w > d) return 0;
//// Segment/ray intersects triangle. Perform delayed division and
//// compute the last barycentric coordinate component
//float ood = 1.0f / d;
//t *= ood;
//v *= ood;
//w *= ood;
//u = 1.0f - v - w;
//
//
//
//
//
//
//return 1;
//}