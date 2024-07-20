#include "Shader.h"
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <iostream>
#include <vector>
#include "Camera.h"
#include <GL/glew.h>
#include "Perlin.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



GLuint VAOs[4];
GLuint VBOs[4];

Shader* shaders[2];

bool perspective = true;

bool dKey = false;
bool aKey = false;
bool wKey = false;
bool sKey = false;

GLuint textureTop;
GLuint textureSide;
GLuint textureDirt;

std::vector<glm::vec3> genMap(int width, int height) {
	std::vector<glm::vec3> map;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map.push_back(glm::vec3(i, 0.0f, j));
		}			
	}
	return map;
}

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = 500;
float lastY = 500;


std::vector<glm::vec3> noiseMap;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void myDisplay() { //display function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clears the viewport

	float currentFrame = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();

	glm::mat4 projection;
	if (perspective)
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	else {
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	}
	glBindVertexArray(VAOs[0]);
	shaders[0]->use();
	shaders[0]->setMat4("model", model);
	shaders[0]->setMat4("view", view);
	shaders[0]->setMat4("projection", projection);
	
	glUniform1i(glGetUniformLocation(shaders[0]->ID, "texTop"), 0);
	glUniform1i(glGetUniformLocation(shaders[0]->ID, "texSide"), 1);
	glUniform1i(glGetUniformLocation(shaders[0]->ID, "texDirt"), 2);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTop);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureSide);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureDirt);

	
	if (dKey) {
		camera.processKeys(RIGHT, deltaTime);
	}
	if (aKey) {
		camera.processKeys(LEFT, deltaTime);
	}
	if (wKey) {
		camera.processKeys(FORWARD, deltaTime);
	}
	if (sKey) {
		camera.processKeys(BACKWARD, deltaTime);
	}

	glBindVertexArray(VAOs[0]);

	for (int i = 0; i < noiseMap.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, noiseMap[i]);
		shaders[0]->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glutSwapBuffers(); //swaps the buffers
}

void myKeyboard(unsigned char key, int x, int y) { // keyboard function for ASCII characters

	switch (key) {
	case 27: //escape key
		exit(0);
		break;
	case 112:  //press p for perspective
		perspective = true;
		break;
	case 111: //press o for orthographic
		perspective = false;
		break;
	}

	if (key == 100) {
		dKey = true;
	}
	if (key == 97) {
		aKey = true;
	}
	if (key == 119) {
		wKey = true;
	}
	if (key == 115) {
		sKey = true;
	}


	glutPostRedisplay(); //tell glut to call the display function
}

void myKeyboardUp(unsigned char key, int x, int y) { // keyboard up function for ASCII characters
	if (key == 100) {
		dKey = false;
	}
	if (key == 97) {
		aKey = false;
	}
	if (key == 119) {
		wKey = false;
	}
	if (key == 115) {
		sKey = false;
	}
}

void myKeyboard2(int key, int x, int y) { // keyboard function for special characters

}

void myMouse(int button, int state, int x, int y) { // mouse function

}

void myMouseMotion(int x, int y) { // mouse motion function only gets called when mouse is clicked

}

void myPassiveMouseMotion(int x, int y) { // mouse motion function gets called when mouse is not clicked
	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}
	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates range from bottom to top
	lastX = x;
	lastY = y;

	camera.processMouse(xoffset, yoffset);

	if (x < 300 || x > 1000 - 300) {  //you can use values other than 100 for the screen edges if you like, kind of seems to depend on your mouse sensitivity for what ends up working best
		lastX = 1000 / 2;   //centers the last known position, this way there isn't an odd jump with your cam as it resets
		lastY = 1000 / 2;
		glutWarpPointer(1000 / 2, 1000 / 2);  //centers the cursor
	}
	else if (y < 300 || y > 1000 - 300) {
		lastX = 1000 / 2;
		lastY = 1000 / 2;
		glutWarpPointer(1000 / 2, 1000 / 2);
	}
}

void myReshape(int w, int h) { // reshape function, gets called when window is resized

}

void myIdle() { // idle function, gets called when no other events are happening 

	//use this function to animate objects



	//tell glut to call the display function
	glutPostRedisplay();

}

int main(int argc, char** argv) {
	std::vector<glm::vec3> map = genMap(50, 50);
	std::vector<float> heights;

	for (int i = 0; i < map.size(); i++) {
		float n = 0.0f;
		float a = 1.0f;
		float f = 0.005f;
		for (int o = 0; o < 8; o++) {
			double xi = (double)map[i].x * f;
			double zi = (double)map[i].z * f;
			double noise = a * ((Noise2D(xi, zi)));

			n += noise;

			a *= 0.5;
			f *= 2.0;
		}
		n *= 25;
		n = n>0.0f ? n : n*-1;
		n = (float)floor(n);
		
		heights.push_back(n);
	}

	for (int i = 0; i < heights.size(); i++) {
		for (int j = 0; j <= heights[i]; j++) {
			noiseMap.push_back(glm::vec3(map[i].x, j, map[i].z));
		}
	}

	float vertices[] = {
	
  -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 1.0f,
  0.25f, -0.25f, -0.25f, -1.0f, 0.0f, 1.0f,
  0.25f,  0.25f, -0.25f, -1.0f, -1.0f, 1.0f,
  0.25f,  0.25f, -0.25f, -1.0f, -1.0f, 1.0f,
  -0.25f, 0.25f, -0.25f, 0.0f,-1.0f, 1.0f,
  -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 1.0f,

 -0.25f, -0.25f,  0.25f,  0.0f, 0.0f, 1.0f,
  0.25f, -0.25f,  0.25f,  -1.0f, 0.0f, 1.0f,
  0.25f,  0.25f,  0.25f,  -1.0f, -1.0f, 1.0f,
  0.25f,  0.25f,  0.25f,  -1.0f, -1.0f, 1.0f,
 -0.25f,  0.25f,  0.25f,  0.0f, -1.0f, 1.0f,
 -0.25f, -0.25f,  0.25f,  0.0f, 0.0f, 1.0f,

 -0.25f,  0.25f,  0.25f,  0.0f, 0.0f, 1.0f,
 -0.25f,  0.25f, -0.25f,  1.0f, 0.0f, 1.0f,
 -0.25f, -0.25f, -0.25f,  1.0f, 1.0f, 1.0f,
 -0.25f, -0.25f, -0.25f,  1.0f, 1.0f, 1.0f,
 -0.25f, -0.25f,  0.25f,  0.0f, 1.0f, 1.0f,
 -0.25f,  0.25f,  0.25f,  0.0f, 0.0f, 1.0f,

  0.25f,  0.25f,  0.25f,  0.0f, 0.0f, 1.0f,
  0.25f,  0.25f, -0.25f,  1.0f, 0.0f, 1.0f,
  0.25f, -0.25f, -0.25f,  1.0f, 1.0f, 1.0f,
  0.25f, -0.25f, -0.25f,  1.0f, 1.0f, 1.0f,
  0.25f, -0.25f,  0.25f,  0.0f, 1.0f, 1.0f,
  0.25f,  0.25f,  0.25f,  0.0f, 0.0f, 1.0f,


  -0.25f, -0.25f, -0.25f,  0.0f, 1.0f, 2.0f,
  0.25f, -0.25f, -0.25f,  1.0f, 1.0f, 2.0f,
  0.25f, -0.25f,  0.25f,  1.0f, 0.0f, 2.0f,
  0.25f, -0.25f,  0.25f,  1.0f, 0.0f, 2.0f,
  -0.25f, -0.25f,  0.25f,  0.0f, 0.0f, 2.0f,
  -0.25f, -0.25f, -0.25f,  0.0f, 1.0f, 2.0f,


   -0.25f,  0.25f, -0.25f,  0.0f, 1.0f, 0.0f,
  0.25f,  0.25f, -0.25f,  1.0f, 1.0f, 0.0f,
  0.25f,  0.25f,  0.25f,  1.0f, 0.0f, 0.0f,
  0.25f,  0.25f,  0.25f,  1.0f, 0.0f, 0.0f,
  -0.25f,  0.25f,  0.25f,  0.0f, 0.0f, 0.0f,
  -0.25f,  0.25f, -0.25f,  0.0f, 1.0f, 0.0f,
};


	
	//glut initializarions
	glutInit(&argc, argv);
	glutInitWindowSize(1600, 1200);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Window 1");
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutKeyboardUpFunc(myKeyboardUp);
	glutSpecialFunc(myKeyboard2);
	glutMouseFunc(myMouse);
	glutMotionFunc(myMouseMotion);
	glutPassiveMotionFunc(myPassiveMouseMotion);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myIdle);

	glutSetCursor(GLUT_CURSOR_NONE); //hide cursor

	//glew initializations
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}

	glEnable(GL_DEPTH_TEST);//enable depth testing

	Shader sh1("ver1.vs", "frag1.fs");
	shaders[0] = &sh1;
	glClearColor(0.0747, 0.17451, 0, 0); //set background color

	//loading texture side green and dirt
	glGenTextures(1, &textureSide);
	glBindTexture(GL_TEXTURE_2D, textureSide);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("grassSide.jpeg", &width, &height, &nrChannels, 0);

	if (nullptr == data) {
		std::cerr << "Failed to load % s resource!" << std::endl;
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	// loading texture dirt
	glGenTextures(1, &textureDirt);
	glBindTexture(GL_TEXTURE_2D, textureDirt);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("dirt.png", &width, &height, &nrChannels, 0);

	if (nullptr == data) {
		std::cerr << "Failed to load % s resource!" << std::endl;
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// loading texture top green
	glGenTextures(1, &textureTop);
	glBindTexture(GL_TEXTURE_2D, textureTop);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("grassTop.png", &width, &height, &nrChannels, 0);

	if (nullptr == data) {
		std::cerr << "Failed to load % s resource!" << std::endl;
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenVertexArrays(4, VAOs);
	glGenBuffers(4, VBOs); //generate buffer 

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); //bind buffer (Type of Vertex Buffer Object is GL_ARRAY_BUFFER)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //put data in buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// to draw in wireframe mode

	glutMainLoop();

	return 0;
}