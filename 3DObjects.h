class Ray{
	protected:
		glm::dvec3 origin;
		glm::dvec3 direction;
		double mediumRefractiveIndex = 1;
		double intensity;
		glm::dvec3 color;
		
		public:
		Ray(glm::dvec3 &orig, glm::dvec3 &dir, float i= 1.0f, glm::dvec3 color = glm::dvec3(1.0f, 1.0f,1.0f)):origin(orig), direction(glm::normalize(dir)){};
		
		glm::dvec3 getDirection(){
			return direction;
		}			
		
		glm::dvec3 getOrigin(){
			return origin;
		}

};


class ThreeDObject{
	public:
		glm::dvec3 surfaceColor;
		float refl;     //amount of light reflected
		float refr;     //amount of light refracted
		glm::dvec3 F0;  //Fresnel reflectance
		float Kd;       //diffuse coefficient
		float Ks;       //specular coefficient
		float se; 	//specular exponent
		float eta;	//refraction coefficient
		float R;   	//Roughness value 
		
		ThreeDObject(glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, float se, float eta, float R):surfaceColor(sc), refl(refl), refr(refr), F0(F0), Kd(kd), Ks(ks), se(se), eta(eta), R(R){};
		
		virtual bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& normal) = 0;
		

};






class Glass: public ThreeDObject{
	public:
		glm::dvec3 axis;
		glm::dvec3 center;
		float radiusOuter;
		float width;
		float height;
		
	Glass(glm::dvec3 center, float radiusOuter, float height, float width, glm::dvec3 axis, glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, float se, float eta, float R): center(center),radiusOuter(radiusOuter), height(height), width(width), axis(axis), ThreeDObject(sc, refl, refr, F0, kd, ks, se, eta, R){}
	
	bool intersectBase(Ray &ray, glm::dvec3 &p, double &t){
		
			glm::dvec3 normal;
			if(p.y == center.y + height){
				normal = glm::dvec3(0,1,0);
			}else if(p.y == center.y){
				normal = glm::dvec3(0,-1,0);
			}
			glm::dvec3 deltaP = glm::dvec3(ray.getOrigin().x - center.x, ray.getOrigin().y - center.y, ray.getOrigin().z - center.z);
			
			double a = normal[0];
			double b = normal[1];
			double c = normal[2];
			
			double d = -(a*(p.x - center.x) + (p.y - center.y) + (p.z - center.z));
			
			
			if(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z == 0) return false;
			
			double dist = -(a*deltaP.x + b*deltaP.y + c*deltaP.z+d)/(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z);
			
			if(dist < 0.0000001) return false;
			
			glm::dvec3 point = deltaP + glm::dvec3(dist, dist, dist)*ray.getDirection();
			
			if(point.x*point.x + point.z*point.z -radiusOuter*radiusOuter > 0.00000001 ) return false;
			t = dist;
			return true;
		
	}
		
	bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& nor){
		
			glm::dvec3 origin = ray.getOrigin();
			glm::dvec3 direc = ray.getDirection();
			
			glm::dvec3 deltaP = origin - center;
			
			double a = direc.x*direc.x + direc.z*direc.z;
			double b = 2 * (direc.x*(deltaP.x) + direc.z*deltaP.z);
			double c = (deltaP.x)*(deltaP.x) + (deltaP.z)*(deltaP.z) - (radiusOuter*radiusOuter);
			double c2 = (deltaP.x)*(deltaP.x) + (deltaP.z)*(deltaP.z) - ((radiusOuter -width)*(radiusOuter - width));
			 
			double delta = b * b -  4 * a * c;
			double delta2 = b * b - 4 * a * c2;
			
			double epsilon = 0.00000001;
			
			if(delta < epsilon) return false;
			
			double d1 = (-b - sqrt(delta))/(2*a);
			double d2 = (-b + sqrt(delta))/(2*a);
			
			if(d1 > d2) t1 = d2;
			else t1 = d1;
			float y = origin.y + t1*direc.y;
			if(y < center.y) return false;
			
			if(y >= center.y + height){
				double d3;
				glm::dvec3 center2 =  glm::dvec3(center.x, center.y+height, center.z);
				bool b1 = intersectBase(ray, center2, d3);
				int inRadius = radiusOuter - width;
				if(b1){
					glm::dvec3 point = origin + glm::dvec3(d3, d3, d3)*direc;
					//if(center - )
					if(glm::length(point - center2) <= radiusOuter - width ){
						t1 = d3;
						
						//nor = center - point;
						nor.x = -(point.x - center.x);
						nor.y = 0;
						nor.z = -(point.z - center.z);
						//std::cout<<"aayu";
						return true;
					} 
					else{
						nor = axis;
						//t1 = d3;
						t2 = 0;
						return true;
					}
					
				}else return false;
			}
			
			glm::dvec3 point = ray.getOrigin() + glm::dvec3(t1, t1, t1)*ray.getDirection();
			glm::dvec3 pointAxis = glm::dvec3(center.x, point.y, center.z);
			nor = -(pointAxis - point);
			return true;		
	}
	

};

class HemiSphere: public ThreeDObject{
	public:
		glm::dvec3 center;
		float radius;
		
	HemiSphere(glm::dvec3 c, float r, glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, float se, float eta, float R ): center(c), radius(r), ThreeDObject(sc, refl, refr, F0, kd, ks, se, eta, R){}
	
	bool intersectTop(Ray &ray, glm::dvec3 &p){
		
			glm::dvec3 normal = glm::dvec3(0,1,0);
			
			glm::dvec3 deltaP = glm::dvec3(ray.getOrigin().x - center.x, ray.getOrigin().y - center.y, ray.getOrigin().z - center.z);
			
			double a = normal[0];
			double b = normal[1];
			double c = normal[2];
			
			double d = -(a*(p.x - center.x) + (p.y - center.y) + (p.z - center.z));
			
			
			if(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z == 0) return false;
			
			double dist = -(a*deltaP.x + b*deltaP.y + c*deltaP.z+d)/(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z);
			
			if(dist < 0.0000001) return false;
			
			glm::dvec3 point = deltaP + glm::dvec3(dist, dist, dist)*ray.getDirection();
			
			if(point.x*point.x + point.z*point.z -radius*radius > 0.00000001 ) return false;
			else
			return true;
		
	}
	
	bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& normal){
		glm::dvec3 L = center - ray.getOrigin();
		float tca = glm::dot(L, ray.getDirection());
		float d2 = glm::dot(L,L) - tca*tca;
		
		if(d2 > radius * radius) return false;
		
		float thc = sqrt(radius*radius - d2);
		
		t1 = tca - thc;
		t2 = tca + thc;
		
		if(t1 < 0) t1 = t2;
		if(t1 < 0) return false;
		float y = ray.getOrigin().y + t1 * ray.getDirection().y;
		
		if(y > center.y){
			float d;
			bool a = intersectTop(ray, center);
			
			if(a){
				t2 = 0;
				normal = (center - (ray.getOrigin() + glm::dvec3(t1, t1, t1)*ray.getDirection())); 
			 	return true;
			 }else 
			 return false;
			 
		}
		normal = ((ray.getOrigin() + glm::dvec3(t1, t1, t1)*ray.getDirection()) - center);
		return true;
	
	}
	

};
class Plane:public ThreeDObject{
	public:
		glm::dvec3 normal;
		glm::dvec3 cornerPoint;
		float side;
		
		Plane(glm::dvec3 normal, float side, glm::dvec3 cornerPoint, glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, int se, float eta, float R): normal(glm::normalize(normal)), side(side), cornerPoint(cornerPoint), ThreeDObject(sc, refl, refr, F0, kd, ks, se, eta, R){}
		
		
		bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& nor){
			
			if(glm::dot(normal, ray.getDirection()) >= 0) return false;
			
			//glm::dvec3 p = glm::dvec3(cornerPoint.x + (side/2), cornerPoint.y, cornerPoint.z+(side/2));
			//float distance = glm::length(p);
			float od = glm::dot(ray.getOrigin()-cornerPoint, normal);
			if(od < 0) od = -od;
			
			float cosa = -glm::dot(ray.getDirection(), normal);
			
			t1 = od/cosa;
			t2 = t1;
			
			glm::dvec3 intersection = ray.getOrigin() + glm::dvec3(t1, t1, t1)*ray.getDirection();
			
			if(intersection.x < cornerPoint.x || intersection.x > cornerPoint.x + side || intersection.z < cornerPoint.z || intersection.z > cornerPoint.z + side ) return false;
			nor = normal; 
			return true;
		
		}

};


class Cylinder: public ThreeDObject{
	public:
		glm::dvec3 center;
		glm::dvec3 axis;
		float radius;
		float height;
		
		
		Cylinder(glm::dvec3 center, glm::dvec3 axis, float radius, float height, glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, float se, float eta, float R ): center(center), axis(axis), radius(radius), height(height), ThreeDObject(sc, refl, refr, F0, kd, ks, se, eta, R){}
		
		bool intersectBase(Ray &ray, glm::dvec3 &p, double &t){
		
			glm::dvec3 normal;
			if(p.y == center.y + height){
				normal = glm::dvec3(0,1,0);
			}else if(p.y == center.y){
				normal = glm::dvec3(0,-1,0);
			}
			glm::dvec3 deltaP = glm::dvec3(ray.getOrigin().x - center.x, ray.getOrigin().y - center.y, ray.getOrigin().z - center.z);
			
			double a = normal[0];
			double b = normal[1];
			double c = normal[2];
			
			double d = -(a*(p.x - center.x) + (p.y - center.y) + (p.z - center.z));
			
			
			if(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z == 0) return false;
			
			double dist = -(a*deltaP.x + b*deltaP.y + c*deltaP.z+d)/(a*ray.getDirection().x + b*ray.getDirection().y + c*ray.getDirection().z);
			
			if(dist < 0.0000001) return false;
			
			glm::dvec3 point = deltaP + glm::dvec3(dist, dist, dist)*ray.getDirection();
			
			if(point.x*point.x + point.z*point.z -radius*radius > 0.00000001 ) return false;
			t = dist;
			return true;
		
		}
		bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& normal){
			
			
			glm::dvec3 origin = ray.getOrigin();
			glm::dvec3 direc = ray.getDirection();
			
			glm::dvec3 deltaP = origin - center;
			
			double a = direc.x*direc.x + direc.z*direc.z;
			double b = 2 * (direc.x*(deltaP.x) + direc.z*deltaP.z);
			double c = (deltaP.x)*(deltaP.x) + (deltaP.z)*(deltaP.z) - (radius*radius);
			
			
			double delta = b * b -  4 * a * c;
			
			double epsilon = 0.00000001;
			//if(delta < 0) return false;
			if(delta < epsilon) return false;
			
			double d1 = (-b - sqrt(delta))/(2*a);
			double d2 = (-b + sqrt(delta))/(2*a);
			
			
			if(d1 > d2) t1 = d2;
			else t1 = d1;
			
			if(t1 <= epsilon) return false;
			
			double y = origin.y + t1 * direc.y;
			
			
			if(y >= height + center.y|| y <= center.y){
				double dist;
				glm::dvec3 center2 = glm::dvec3(center.x,center.y+height, center.z);
				bool b1 = intersectBase(ray,center2 , dist);
				if(b1){
				 	t1 = dist;
				 	normal = axis;
				}
				
				bool b2 = intersectBase(ray, center, dist);
				if(b2 && dist > epsilon && t1 >= dist){
				 	t1 = dist;
				 	normal = -axis;
				 }
				
				return b1 || b2;
			}
			normal = (ray.getOrigin() + glm::dvec3(t1, t1, t1)*ray.getDirection() - center);
			normal.y = 0;
			//normal.y = center.y;
			return true;
			  
		
		}

};

class Cube: public ThreeDObject{
	public:
		glm::dvec3 minBound;
		glm::dvec3 maxBound;
		
		Cube(glm::dvec3 v, float xl, float yl, float zl, glm::dvec3 sc, float refl, float refr, glm::dvec3 F0, float kd, float ks, float se, float eta, float R): minBound(v), maxBound(glm::dvec3(v.x+xl, v.y+yl, v.z+zl)), ThreeDObject(sc, refl, refr, F0, kd, ks, se, eta, R){
		};
		
		bool getIntersectionPoint(Ray &ray, float &t1, float &t2, glm::dvec3& nor){
			
			glm::dvec3 orig = ray.getOrigin();
			glm::dvec3 dir = ray.getDirection();
		
			std::vector<float> normals = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
			
			
			for(int i = 0; i < 6; i++){
				glm::dvec3 normal = glm::dvec3(normals[3*i], normals[3*i+1], normals[3*i+2]);
				if(glm::dot(normal, dir) >= 0) continue;
					
				float cosa = -glm::dot(normal, dir);
				
				
				float t;
				bool flag = 0;
				glm::dvec3 tmp_p;
				
				switch(i){
                case 0:
                	//std::cout<<orig.z <<maxBound.z<<std::endl;
                    
                    t = (orig.z-(maxBound.z))/cosa;
                    //std::cout<<" b"<<(orig.z -maxBound.z)<<" "<<cosa<<std::endl;
                    if(t<0) break;
                    //std::cout<<"case";
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.x >= minBound.x && tmp_p.x<=maxBound.x && tmp_p.y>=minBound.y && tmp_p.y<=maxBound.y;
                    
                    break;
                case 1:
                    t = (minBound.z-orig.z)/cosa;
                    if(t<0) break;
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.x>minBound.x && tmp_p.x<maxBound.x && tmp_p.y>minBound.y && tmp_p.y<maxBound.y;
                    break;
                case 2:
                    t = (minBound.y-orig.y)/cosa;
                    if(t<0) break;
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.z>=minBound.z && tmp_p.z<=maxBound.z && tmp_p.x>= minBound.x&& tmp_p.x<=maxBound.x;
                    break;
                case 3:
                    t = (orig.y-maxBound.y)/cosa;
                    if(t<0) break;
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.z>minBound.z && tmp_p.z<maxBound.z && tmp_p.x>minBound.x && tmp_p.x<=maxBound.x;
                    break;
                case 4:
                    t = (orig.x-maxBound.x)/cosa;
                    if(t<0) break;
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.z>= minBound.z && tmp_p.z<=maxBound.z && tmp_p.y>=minBound.y && tmp_p.y<=maxBound.y;
                    break;
                default:
                    t = (minBound.x-orig.x)/cosa;
                    if(t<0) break;
                    tmp_p = orig + glm::dvec3(t,t,t)*dir;
                    flag = tmp_p.z>=minBound.z && tmp_p.z<=maxBound.z && tmp_p.y>=minBound.y && tmp_p.y<=maxBound.y;
                    break;
            }
            //std::cout<<flag;
            if(!flag) continue;
            if(t<t1){
                t2 = t1;
                t1 = t;
                nor = normal;
                //std::cout<<"here";Zz
            }
            else t2 = t;
        }
      
        //std::cout<<t1<<std::endl;
        if(t1<INFINITY){
        	//nor = glm::dvec3(0, 0, 1);
         return true;
	
	}
	else return false;
								
			
	}
			
};
		
