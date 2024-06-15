

class Ray;
class ThreeDObject;

class DirectionalLight{
	public:
		float angle;
		glm::dvec3 position;
		glm::dvec3 pointedAt;
		glm::dvec3 direction;
		glm::dvec3 color;
		float intensity;
	
	DirectionalLight(glm::dvec3 pos, glm::dvec3 c, float i, float angle, glm::dvec3 pA):position(pos), color(c), intensity(i), pointedAt(pA), direction(pointedAt - position),angle(angle){}


};


class AreaLight{
	public:
		float side;
		glm::dvec3 position;
		glm::dvec3 color;
		float spacing;
		int widthP;
		int heightP;
		float intensity;
		bool isOn;
		
	AreaLight(glm::dvec3 pos, float s, glm::dvec3 color, float spacing, float i, bool iO): side(s), position(pos), color(color), spacing(spacing), widthP(side/spacing), heightP(side/spacing), intensity(i), isOn(iO){
	}
};


class Camera{
	public:
	glm::dvec3 eye;
	glm::dvec3 center;
	glm::dvec3 up;
	float fov;
	glm::dvec3 centerOfScene;
	
	Camera(glm::dvec3 e, glm::dvec3 c, glm::dvec3 u, int fov, glm::dvec3 cOS): eye(e), center(c), up(u), fov(M_PI/2), centerOfScene(cOS){};
	
	Ray initiateRay(int i, int j, int width, int height){
		
		glm::dvec3 a = eye - center;
		glm::dvec3 dira = glm::normalize(a);
		
		glm::dvec3 normal = up;
		glm::dvec3 x = glm::normalize(glm::cross(dira, normal));
		
		glm::dvec3 y = glm::cross(x, dira);
		
		float u = tan(fov/2.)*(i - width/2)/(width/2);
		float v = tan(fov/2.)*(j - height/2)/(height/2);
		
		glm::dvec3 direction = x*glm::dvec3(u, u, u) + y*glm::dvec3(v,v,v) -a;
		
		
		Ray ray(eye, direction);
		
		return ray;
	}
	
	
	
};


glm::dvec3 getReflectedRay(glm::dvec3& point, glm::dvec3 normal, Ray ray){
	
	float tmp = 2*glm::dot(glm::normalize(ray.getDirection()), normal);
	return ray.getDirection() - glm::dvec3(tmp, tmp, tmp) * normal;

}


glm::dvec3 getRefractedRay(glm::dvec3& point, glm::dvec3 normal, Ray ray, float etai, float etat){	
	point = glm::normalize(point);
	float cosi = glm::dot(point, normal);
	if(cosi > 1.0f) cosi = 1.0f;
	if(cosi < -1.0f) cosi = -1.0f;
	cosi = - cosi;
	
	//If the ray id going from inside the object to outside, reverse the etas
	if(cosi < 0) return getRefractedRay(point, -normal,ray, etat, etai);
	
	//calculate eta
	float eta = etai/etat;
	
	float c = sqrt(1 - (eta)*(eta)*(1 - cosi*cosi)); 
	 float tmp = (eta*cosi - c);
	 
	 //when the term c is less than zero total internal refraction happens
	 return c < 0 ? glm::dvec3(1, 0, 0) : glm::dvec3(eta, eta, eta)*point + glm::dvec3(tmp, tmp, tmp)*normal;
	

}

float min(float a, float b){
	return a < b? a:b;
}

glm::dvec3 trace(Ray &ray, std::vector<ThreeDObject*> objects, std::vector<DirectionalLight*>& directionalLights, std::vector<AreaLight*>&areaLights, bool isCookTorrace, int depth){
	
	if(depth < 1) return glm::dvec3(0,0,0);
	
	float t_min = INFINITY;
	ThreeDObject* object = NULL;
	
	float bias = 1e-4;
	
	glm::dvec3 normal;
	glm::dvec3 minNormal;
	
	int numOfObjects = objects.size();
	
	glm::dvec3 color = glm::dvec3(0, 0, 0);
	//std::cout<<isCookTorrace<<std::endl;
	
	for(int i = 0; i < numOfObjects; i++){
		float t1 = INFINITY, t2 = INFINITY;
		if(objects[i] -> getIntersectionPoint(ray, t1, t2, normal)){
			if(t1 < 0) t1 = t2;
			if(t1 < t_min){
				
				t_min = t1;
				object = objects[i];
				minNormal = normal;
			}
			
			//if(t2 == 0) cc = true;
		}
	}
	
	glm::dvec3 diffuseIntensity = glm::dvec3(0, 0, 0);
	glm::dvec3 point = ray.getOrigin() + glm::dvec3(t_min, t_min, t_min)*ray.getDirection();
	glm::dvec3 specularIntensity = glm::dvec3(0, 0, 0);
	
	if(object){
		//for each directional light we find if the point is in shadow or not if not then find specular and diffused shading
		for(int i = 0; i < directionalLights.size(); i++){
			
			bool isShadow = false;
			
			glm::dvec3 lightDir = directionalLights[i] -> position - point;
			lightDir = glm::normalize(lightDir);
			
			float angleLS = glm::dot(lightDir, directionalLights[i] -> direction);
			glm::dvec3 normalDir = glm::normalize(minNormal);
			
			float angleNL = glm::dot(normalDir, lightDir);
		
			glm::dvec3 pointtmp = point;
			pointtmp = (angleNL < 0)? point - minNormal*1e-4 : point + minNormal*1e-4;
			Ray ray2(pointtmp, lightDir);
			
			for(int i = 0; i < numOfObjects; i++){
				float t1 = INFINITY, t2 = INFINITY;
				if(objects[i] -> getIntersectionPoint(ray2, t1, t2, normal)) {
					isShadow = true;
					break;
				}
			
			//if(t2 == 0) cc = true;
			}
			
			if(isShadow) continue;
			if(!isCookTorrace){
				if(acos(angleLS)*180.0/M_PI > directionalLights[i] -> angle) continue;
			
			
				if(angleNL < 0) angleNL = 0;
				float in = directionalLights[i] -> intensity;
				
				diffuseIntensity += ( glm::dvec3(in, in, in)* directionalLights[i] -> color *glm::dvec3(angleNL, angleNL, angleNL)); 
			
				glm::dvec3 vplusl = (-point + lightDir);
				glm::dvec3 h = vplusl/glm::length(vplusl);
			
				float angleVN = glm::dot(h, normalDir);
				if(angleVN < 0) angleVN = 0;
			
				float nhs = directionalLights[i] -> intensity*powf(angleVN, object -> se);
			
				specularIntensity += glm::dvec3(nhs, nhs, nhs)*(directionalLights[i] -> color) ; 
			}else{
				float Ga = 1.0;
				glm::dvec3 vplusl = (-point + lightDir);
				glm::dvec3 h = vplusl/glm::length(vplusl);
				float tmp = glm::dot(minNormal, h)/glm::dot(point, h);
				
				float Gb = 2*glm::dot(minNormal, ray.getDirection())*tmp;
				float Gc = 2*tmp*glm::dot(minNormal, lightDir);
				float G = min(Ga, min(Gb, Gc));
				
				float tmp1 = (object -> R*G)/(4*glm::dot(minNormal, lightDir)*glm::dot(minNormal, point));
				  
				specularIntensity = object -> F0*glm::dvec3(tmp1, tmp1, tmp1);
				float in = directionalLights[i] -> intensity*angleNL;
				
				diffuseIntensity += (glm::dvec3(in, in, in)*directionalLights[i] -> color); 
			
			
			}
			
		}
		
		//For area light find if the point is in shadow if not then find diffuse and specular shading
		if(areaLights.size() > 0 && areaLights[0] -> isOn != false){
			
			int width = areaLights[0]->widthP;
			int height = areaLights[0]-> heightP;
			glm::dvec3 lightpos = areaLights[0] -> position;
			
			for(int i = 0; i < height; i++){
			
				lightpos.x = areaLights[0] -> position.x;
				
				for(int j = 0; j < width; j++){
					glm::dvec3 lightDir = lightpos - point;
					lightDir = glm::normalize(lightDir);
					glm::dvec3 normalDir = glm::normalize(minNormal);
					
					bool isShadow = false;
					Ray ray2(point, lightDir);
					
					for(int i = 0; i < numOfObjects; i++){
						float t1 = INFINITY, t2 = INFINITY;
							if(objects[i] -> getIntersectionPoint(ray2, t1, t2, normal)) {isShadow = true;
							break;}
					}
					
					if(isShadow) continue;
					float angleNL = glm::dot(normalDir, lightDir);
					if(angleNL < 0) angleNL = 0;
					if(!isCookTorrace){
						
					
						float temp = angleNL*(areaLights[0] -> intensity)/(height * width);
					
						diffuseIntensity += (areaLights[0]-> color*glm::dvec3(temp, temp, temp));
					
						glm::dvec3 vplusl = (-point + lightDir);
						glm::dvec3 h = vplusl/glm::length(vplusl);
			
						float angleVN = glm::dot(h, normalDir);
					
						if(angleVN < 0) angleVN = 0;
					
						float nhs = powf(angleVN, object -> se)/(height*width);
						specularIntensity += glm::dvec3(nhs, nhs, nhs)*(areaLights[0] -> color); 
					
						lightpos += glm::dvec3(areaLights[0]-> spacing, 0 ,0);
					}else{
						float Ga = 1.0;
					glm::dvec3 vplusl = (-point + lightDir);
				glm::dvec3 h = vplusl/glm::length(vplusl);
				float tmp = glm::dot(minNormal, h)/glm::dot(point, h);
				
				float Gb = 2*glm::dot(minNormal, ray.getDirection())*tmp;
				float Gc = 2*tmp*glm::dot(minNormal, lightDir);
				float G = min(Ga, min(Gb, Gc));
				
				float tmp1 = (object -> R*G)/(4*glm::dot(minNormal, lightDir)*glm::dot(minNormal, ray.getDirection()));
				  
				specularIntensity = object -> F0*glm::dvec3(tmp1, tmp1, tmp1);
				float in = areaLights[0] -> intensity*angleNL;
				
				diffuseIntensity += (glm::dvec3(in, in, in)*areaLights[0] -> color); 
					
					}
				}
				lightpos += glm::dvec3(0, 0 ,areaLights[0]-> spacing);			
			} 
		}
		
		
		
		glm::dvec3 pointtmp = point;
		glm::dvec3 reflectedDir = getReflectedRay(pointtmp, minNormal, ray);
		
		glm::dvec3 pointtmpr = point;
		glm::dvec3 refractedDir = getRefractedRay(pointtmpr, minNormal, ray, 1.0, object -> eta);
				
		//if object is reflective
		if(object -> refl > 0 && depth > 1){
			
		
			float angleVN = glm::dot(reflectedDir, minNormal);
		
			pointtmp = (angleVN < 0)? point - minNormal*1e-5 : point + minNormal*1e-5;
			Ray reflectedRay(pointtmp, reflectedDir);
			
			glm::dvec3 reflColor = trace(reflectedRay, objects, directionalLights, areaLights, isCookTorrace, depth-1);
			
			glm::dvec3 h = (reflectedDir - ray.getDirection());
			float angleNH = glm::dot(point, h);
			if(angleNH < 0) angleNH = 0;
			float refl = object -> refl;
			color+= ( glm::dvec3(refl, refl, refl)*reflColor);
			
		}
		
		
		//if object is transparent/ refracts light
		
		if(object -> refr > 0 && depth > 1){
			
			float angleTN = glm::dot(refractedDir, minNormal);
			pointtmpr = (angleTN < 0)? point - minNormal*1e-5 : point + minNormal*1e-5;	
			
			Ray refractedRay(pointtmpr, refractedDir);
		
			glm::dvec3 refractColor = trace(refractedRay, objects, directionalLights, areaLights, isCookTorrace, depth-1);
			float refr = object ->refr; 
			color+= (glm::dvec3(refr, refr, refr) * refractColor);

		}		
		
	
	}
	
	//float r = 0, g = 0, b = 0;
	
	float kd, ks;
	if(object){kd = object -> Kd; ks = object -> Ks;}
	if(object != NULL){
	 	return color+object -> surfaceColor+glm::dvec3(kd, kd, kd) * diffuseIntensity + glm::dvec3(ks, ks, ks)* specularIntensity;
	 }else return glm::dvec3(0.0f, 0.0f, 0.0f);

}

