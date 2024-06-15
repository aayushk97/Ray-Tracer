 #include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include <vector>
#include "3DObjects.h"
#include "trace.h"





using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


std::vector<ThreeDObject*> objects;
std::vector<DirectionalLight*> dls;
std::vector<AreaLight*> ars;
Camera* camera;
bool isCookTorrace = false;

int main(int argc, char* argv[]){
	
	//int SCR_WIDTH = 600*2;
	//int SCR_HEIGHT =600*2;
	
	int SCR_WIDTH = 800;
	int SCR_HEIGHT =800;
	
	if(argc >= 3){
		SCR_WIDTH = stoi(argv[1]);
		SCR_HEIGHT = stoi(argv[2]);
	}
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Recursive Ray Tracer", NULL, NULL);
	if(window == NULL){
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//To be notified when a physical key is pressed
	glfwSetKeyCallback(window, key_callback);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		cout<< "Failed to initialize GLAD" << endl;
		return -1;
	}
	
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	//create floor
	//parameters: normal, distance, surface color 
	//==================FLOOR============================
	objects.push_back(new Plane(glm::dvec3(0.0f, 1.0f, 0.0f), 100.0f, glm::dvec3(-35-12.5,-110, 130-12.5),glm::dvec3(0.5f, 0.5f, 0.75f), 0.8, 0, glm::dvec3(0.02, 0.02, 0.02), 0.3, 0.5, 1., 0.135, 0.1));
	
	//==================TABLE============================
	//create 5 cubes
	Cube* tableTop = new Cube(glm::dvec3(-35, -15-20, 50+50+30), 75, 1, 75, glm::dvec3(0.37f, 0.0f,0.0f), 0.1, 0.50, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Cube* tableLeg1 = new Cube(glm::dvec3(-35, -15-75-20, 50+50+30), 2, 75, 2,glm::dvec3(0.37f, 0.0f,0.0f), 0.1, 0.50, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Cube* tableLeg2 = new Cube(glm::dvec3(-35+75-2, -15-75-20, 50+50+30), 2, 75, 2,glm::dvec3(0.37f, 0.0f,0.0f), 0.1, 0.50, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Cube* tableLeg3 = new Cube(glm::dvec3(-35+75-2, -15-75-20, 50+50+30+75-2), 2, 75, 2,glm::dvec3(0.37f, 0.0f,0.0f), 0.1, 0.50, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Cube* tableLeg4 = new Cube(glm::dvec3(-35, -15-75-20, 50+50+30+75-2), 2, 75, 2,glm::dvec3(0.37f, 0.0f,0.0f), 0.1, 0.50, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	objects.push_back(tableTop);
	objects.push_back(tableLeg1);
	objects.push_back(tableLeg2);
	objects.push_back(tableLeg3);
	objects.push_back(tableLeg4);
	
	//==================PLATE=============================
	Cylinder* plate = new Cylinder(glm::dvec3(-35+35.5, -15-20+1, 50+50+30+35.5), glm::dvec3(0, 1, 0), 10,1, glm::dvec3(0.75f, 0.75f, 0.75f), 0.9, 0.0, glm::dvec3(0.95, 0.93, 0.88), 0.3, 0.5, 50., 0.135, 0.3);
	objects.push_back(plate);
	
	//==================BOWL==============================
	//HemiSphere* bowl = new HemiSphere(glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5), 25, glm::dvec3(1.0f, 0.84f, 0.75f), 1, 0);
	HemiSphere* bowl = new HemiSphere(glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5), 5, glm::dvec3(1.0f, 0.84f, 0.75f), 0.9, 0.0, glm::dvec3(1.00, 0.71, 0.29),  0.3, 0.5, 50., 0.135, 0.2);
	objects.push_back(bowl);
	
	//==================GLASSES===========================
	Glass* gl = new Glass(glm::dvec3(-35+35.5+10, -15-20+1, 50+50+30+35.5+10), 3, 10, 1,glm::dvec3(0,1,0), glm::dvec3(0.7f, 0.7f,0.75f),  0.1, 0.9, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Glass* gl2 = new Glass(glm::dvec3(-35+35.5-10, -15-20+1, 50+50+30+35.5+10), 3, 10, 1, glm::dvec3(0,1,0), glm::dvec3(0.7f, 0.7f,0.75f), 0.1, 0.9, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Glass* gl3 = new Glass(glm::dvec3(-35+35.5-15, -15-20+1, 50+50+30+35.5-10), 3, 10, 1, glm::dvec3(0,1,0), glm::dvec3(0.7f, 0.7f,0.75f), 0.1, 0.9, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	Glass* gl4 = new Glass(glm::dvec3(-35+35.5+15, -15-20+1, 50+50+30+35.5-10), 3, 10, 1, glm::dvec3(0,1,0), glm::dvec3(0.7f, 0.7f,0.75f),  0.1, 0.9, glm::dvec3(0.08, 0.08, 0.08), 0.35, 0.0, 123.f, 1.5, 0.1);
	
	
	objects.push_back(gl);
	objects.push_back(gl2);
	objects.push_back(gl3);
	objects.push_back(gl4);
	//=========================LIQUIDS=========================
	
	Cylinder* water1 = new Cylinder(glm::dvec3(-35+35.5+10, -15-20+2, 50+50+30+35.5+10), glm::dvec3(0.0, 1.0, 0), 1.9,8, glm::dvec3(1.0, 0.65, 0.0), 0.0, 1.0, glm::dvec3(0.95, 0.93, 0.88), 0.3, 0.5, 50., 0.135, 0.1);
	
	Cylinder* water2 = new Cylinder(glm::dvec3(-35+35.5-10, -15-20+1, 50+50+30+35.5+10), glm::dvec3(0.0, 1.0, 0), 1.9,8, glm::dvec3(0.69, 0.07, 0.15), 0.9, 0.0, glm::dvec3(0.95, 0.93, 0.88), 0.3, 0.5, 50., 0.135, 0.1);
	
	Cylinder* water3 = new Cylinder(glm::dvec3(-35+35.5-15, -15-20+1, 50+50+30+35.5-10), glm::dvec3(0.0, 1.0, 0), 1.9,8, glm::dvec3(0.6, 0.6, 0.6), 1.0, 0.0, glm::dvec3(0.95, 0.93, 0.88), 0.3, 0.5, 50., 0.135, 0.1);
	
	Cylinder* water4 = new Cylinder(glm::dvec3(-35+35.5+15, -15-20+1, 50+50+30+35.5-10), glm::dvec3(0.0, 1.0, 0), 1.9,8, glm::dvec3(0.5, 0.5, 0.0), 0.9, 0.0, glm::dvec3(0.95, 0.93, 0.88), 0.3, 0.5, 50., 0.135, 0.1);
	
	objects.push_back(water1);
	objects.push_back(water2);
	objects.push_back(water3);
	objects.push_back(water4);
	
	
	
	//=====================DIRECTIONAL LIGHTS==================
	DirectionalLight* dl1 = new DirectionalLight(glm::dvec3(-35+75, -15-20+75, 50+50+30), glm::dvec3(0.0f, 1.0f, 1.0f), 0.3f, 5, glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5));
	DirectionalLight* dl2 = new DirectionalLight(glm::dvec3(-35, -15-20+75, 50+50+30), glm::dvec3(1.0f, 1.0f, 0.0f), 0.3f,5, glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5));
	DirectionalLight* dl3 = new DirectionalLight(glm::dvec3(-35+75, -15-20+75, 50+50+30+75), glm::dvec3(0.0f, 1.0f, 1.0f), 0.3f,5, glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5));
	DirectionalLight* dl4 = new DirectionalLight(glm::dvec3(-35, -15-20+75, 50+50+30+75), glm::dvec3(1.0f, 1.0f, 0.0f), 0.3f,5, glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5));
	dls.push_back(dl1);
	dls.push_back(dl2);
	dls.push_back(dl3);
	dls.push_back(dl4);
	
	
	//=====================AREA LIGHTS==========================
	AreaLight* ar = new AreaLight(glm::dvec3(-35+35.5-1,-15-20+10, 50+50+30+35.5-1), 2, glm::dvec3(1.0f, 1.0f, 1.0f), 1.0, 1.0f, false);
	ars.push_back(ar);
	
	//=======================CAMERA=============================
	camera = new Camera(glm::dvec3(0, 0, -1), glm::dvec3(0,0,0), glm::dvec3(0, 1, 0), 90, glm::dvec3(-35+35.5, -15-20+6, 50+50+30+35.5));
	int depth = 4;
	
	GLubyte image[SCR_WIDTH*SCR_HEIGHT*3];
	
	
	
	
	while(!glfwWindowShouldClose(window)){
	
		
		for(int iter = 0; iter < SCR_HEIGHT*SCR_WIDTH; iter++){
			int i, j;
			i = iter / SCR_WIDTH;
			j = iter % SCR_WIDTH;
			
			glm::dvec3 colorObtained = glm::dvec3(0.0f, 0.0f, 0.0f);
			
			Ray ray = camera ->initiateRay(j, i, SCR_WIDTH, SCR_HEIGHT);
			
			colorObtained += trace(ray, objects, dls, ars, isCookTorrace, depth);
			
			
		
			for( int i = 0; i < 3; i++) if(colorObtained[i] > 1.0f){
			 	colorObtained[i]= 0.9999;
			 }
			 
			//std::cout<<colorObtained.x*255.f;
			
			image[iter * 3] = (colorObtained.x)*255.f;
			image[(iter * 3) + 1] = (colorObtained.y)*255.f;
			image[(iter * 3) + 2] = (colorObtained.z)*255.f;
			
			
		}
		
		
		glClearColor(0.5f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0.0, 0.0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, image);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	
	
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)	
		glfwSetWindowShouldClose(window, true);
	else if(key == GLFW_KEY_T && action == GLFW_PRESS){
		cout<< "T is pressed" << endl;
		isCookTorrace = !isCookTorrace;
		
	} 
	else if(key == GLFW_KEY_L && action == GLFW_PRESS){
		cout<< "l is pressed" << endl;
		ars[0] -> isOn = !ars[0] -> isOn;
	} 
	else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		cout<< "left is pressed" << endl;
		
		
	} 
	else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) cout<< "right is pressed" << endl;
	else if(key == GLFW_KEY_UP && action == GLFW_PRESS) cout<< "up is pressed" << endl;
	else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) cout<< "down is pressed" << endl;
	else if(key == GLFW_KEY_A && action == GLFW_PRESS){
		cout<< "a is pressed" << endl;
		ars[0] -> position = glm::dvec3(-35, -15-20+10, 50+50+30);
	} 
	else if(key == GLFW_KEY_B && action == GLFW_PRESS){
		cout<< "b is pressed" << endl;
		ars[0] -> position = glm::dvec3(-35+75-2, -15-20+10, 50+50+30);	
	} 
	else if(key == GLFW_KEY_C && action == GLFW_PRESS){
		cout<< "c is pressed" << endl;
		ars[0] -> position = glm::dvec3(-35+35.5-1,-15-20+10, 50+50+30+35.5-1);
	} 
	else if(key == GLFW_KEY_D && action == GLFW_PRESS){
		cout<< "d is pressed" << endl;
		ars[0] -> position = glm::dvec3(-35+75-4, -15-20+10, 50+50+30+75-4);
	} 
	else if(key == GLFW_KEY_E && action == GLFW_PRESS){
		cout<< "e is pressed" << endl;
		ars[0] -> position = glm::dvec3(-35+ 1, -15-20+10, 50+50+30+75-4);
	} 
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);

}
