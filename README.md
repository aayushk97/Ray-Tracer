
COL781 ASSIGNMENT 1: SUBMITTED BY AAYUSH KUMAR SINGH (2020MCS2444)

Implementation of recursive ray tracer in C++ and glfw for displaying the image on a window.

The scene contains:
1. A white reflective wet floor of size 100 units
2. A Brown Table centered on the floor.
3. On the table we have a silver disk and top of which we have a golden bowl. 
4. Surrowing the golden bowl we have 4 glasses filled with water, red-wine, orange and lemon-yellow color liquids.
The glasses are implemented as 2 concentric cylinders with top filled in between these 2 and liquid is implmented as almost transparent cylinders.

Illumination:
1. The scene contains 4 directional lights with cone angle of 15 degrees. Two on the left are orange and 3 on right are cyan. 
2. On top of the scene we have area light of size 2x2 with spacing 0.5.
3. The diffuse and specular reflection produced is based on phong model and Cook-Torrace which can be toggled.

Reflection and refraction:
1. The reflection and refractions are calculated for a depth of 4.




(I) TO COMPILE:

make

(II)TO RUN:

./test				//TO RUN WITH DEFAULT WINDOW SIZE OF 800 * 800

./test 1200 1200 		//USER CAN SPECIFY INPUT WINDOW SIZE    




(III) INTERFACE INPUTS:
1. Turn On/Off area light: l (Off by default to increase load time)
2. Keys to move area light: 
	Front-left: a ;
	Front-right: b;
	Bottom-left: d;
	Bottom-Right: e;
	center(default): c;
	
3. Toggle between Blinn-Phong and Cook-Torrace: t


 
