#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <time.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"
#include "Light.h"
#include "Object.h"
#include "Texture.h"

using namespace std;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from


shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Program> prog2;
shared_ptr<Shape> shape;
shared_ptr<Shape> shape2;
shared_ptr<Shape> shape3;
shared_ptr<Shape> shape4;

// Declaration/Intialization for Materials
vector<Material> MaterialList;

// Declaration/Intialization for Lights
Light l1(0.0, 5.0, 1.0, 0.8);
vector<Light> LightList = { l1 };
 // Declaration of vector for Objects
vector<Object> ObjectList;
// Create Texture instance
shared_ptr<Texture> texture;

int MPos = 0;
int LPos = 0;
int numOfObjects = 20;
int numOfBunnies = 12;
bool Mouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
float sFactor;
glm::mat3 Tex;
bool keyToggles[256] = {false}; // only for English keyboards!

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glfwGetCursorPos(window, &xmouse, &ymouse);
	if (!(xmouse > width-1 || xmouse < 0.0f) && !(ymouse > height-1 || ymouse < 0.0f))
	{
		if (Mouse)
		{
			lastX = xmouse;
			lastY = ymouse;
			Mouse = false;
		}
		camera->mouseMoved((float)xmouse, (float)ymouse, (float)lastX, (float)lastY);
		lastX = xmouse;
		lastY = ymouse;
	}
}

// This function is called when a key is pushed
static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	// Move Foward
	if (key == 'w' || key == 'W')
	{
		camera->AdjustPosition('w');
	}
	// Move Backward
	else if (key == 's' || key == 'S')
	{
		camera->AdjustPosition('s');
	}
	// Move to the Left
	else if (key == 'a' || key == 'A')
	{
		camera->AdjustPosition('a');
	}
	// Move to the Right
	else if (key == 'd' || key == 'D')
	{
		camera->AdjustPosition('d');
	}
	else
	{
		//do nothing
	}
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Draw(std::shared_ptr<MatrixStack> MV, std::shared_ptr<MatrixStack> P, std::shared_ptr<MatrixStack> MV2)
{
	glUniformMatrix4fv(prog->getUniform("MV2"), 1, GL_FALSE, glm::value_ptr(MV2->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	// ambient component
	glUniform3f(prog->getUniform("ka"), MaterialList.at(MPos).getAmbient().x,
		MaterialList.at(MPos).getAmbient().y, MaterialList.at(MPos).getAmbient().z);
	// diffuse component
	glUniform3f(prog->getUniform("kd"), MaterialList.at(MPos).getDiffuse().x,
		MaterialList.at(MPos).getDiffuse().y, MaterialList.at(MPos).getDiffuse().z);
	// specular component
	glUniform3f(prog->getUniform("ks"), MaterialList.at(MPos).getSpecular().x,
		MaterialList.at(MPos).getSpecular().y, MaterialList.at(MPos).getSpecular().z);
	// shininess factor
	glUniform1f(prog->getUniform("s"), MaterialList.at(MPos).getShiny());
	// light intensity factor
	glUniform1f(prog->getUniform("i1"), LightList.at(LPos).getIntensity());
	MV->translate(0, -2, 0);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(true);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addAttribute("aTex");
	prog->addUniform("MV");
	prog->addUniform("MV2");
	prog->addUniform("P");
	prog->addUniform("Tex");
	prog->addUniform("texture");
	prog->addUniform("lightPos");
	prog->addUniform("CamPos");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");
	prog->addUniform("i1");
	prog->setVerbose(false);
	// For Adding on Texture to Ground
	prog2 = make_shared<Program>();
	prog2->setShaderNames(RESOURCE_DIR + "vert2.glsl", RESOURCE_DIR + "frag2.glsl");
	prog2->setVerbose(true);
	prog2->init();
	prog2->addAttribute("aPos");
	prog2->addAttribute("aNor");
	prog2->addAttribute("aTex");
	prog2->addUniform("MV");
	prog2->addUniform("MV2");
	prog2->addUniform("P");
	prog2->addUniform("Tex");
	prog2->addUniform("texture");
	prog2->addUniform("lightPos");
	prog2->addUniform("CamPos");
	prog2->addUniform("ka");
	prog2->addUniform("ks");
	prog2->addUniform("s");
	prog2->addUniform("i1");
	prog2->setVerbose(false);

	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f);
	
	Tex[0][0] = 1.0f;
	Tex[1][1] = 1.0f;
	Tex[2][2] = 1.0f;
	texture = make_shared<Texture>();
	texture->setFilename(RESOURCE_DIR + "Grass.jpg");
	texture->init();
	texture->setUnit(0);
	texture->setWrapModes(GL_REPEAT, GL_REPEAT);

	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "bunny.obj");
	shape->fitToUnitBox();
	shape->init();

	shape2 = make_shared<Shape>();
	shape2->loadMesh(RESOURCE_DIR + "teapot.obj");
	shape2->fitToUnitBox();
	shape2->init();

	shape3 = make_shared<Shape>();
	shape3->loadMesh(RESOURCE_DIR + "Sphere.obj");
	shape3->fitToUnitBox();
	shape3->init();

	shape4 = make_shared<Shape>();
	shape4->loadMesh(RESOURCE_DIR + "G.obj");
	shape4->fitToUnitBox();
	shape4->init();

	// Creating Materials and adding them to the Material List
	srand((unsigned)time(NULL));
	float r, g, b, turn;
	int xMove, zMove;
	Material Hud(0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 200.0f);
	MaterialList.push_back(Hud);
	for (int i = 0; i < numOfObjects; i++)
	{
		r = (float)rand() / RAND_MAX;
		g = (float)rand() / RAND_MAX;
		b = (float)rand() / RAND_MAX;
		xMove = rand() % 24 + 1;
		xMove = xMove - 12;
		zMove = rand() % 24 + 1;
		zMove = zMove - 12;
		turn = 0.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6.28319 - 0.0)));
		Object O(xMove, 0.0f, zMove, 0.0f, 1.0f, 0.0f, turn, 1.0f, 1.0f, 1.0f);
		Material M(0.2f, 0.2f, 0.2f, r, g, b, 1.0f, 1.0f, 0.0f, 200.0f);
		MaterialList.push_back(M);
		ObjectList.push_back(O);
	}
	Material Sun(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 50.0f);
	MaterialList.push_back(Sun);
	
	
	GLSL::checkError(GET_FILE_LINE);

}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'l']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	auto MV2 = make_shared<MatrixStack>();

	float angle = glfwGetTime();
	angle = cos(angle);
	if (angle > 0.0f && angle < 1.0f)
	{
		sFactor = angle;
	}
	else if(angle < 0.0f && angle > -1.0f)
	{
		sFactor = -angle;
	}
	else
	{
		//do nothing
	}
	
	float HUDRotate = glfwGetTime();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	// Draws HUD Bunny
	MV->pushMatrix();
		MV->translate(1.2f, 0.9f, -3.0f);
		MV->rotate(0.3, 1.0f, 0.0f, 0.0f);
		MV->rotate(HUDRotate, 0.0f, 1.0f, 0.0f);
		MV->scale(0.5);
		prog->bind();
		// light position
		glUniform3f(prog->getUniform("lightPos"), LightList.at(LPos).getLightPosition().x, 
			LightList.at(LPos).getLightPosition().y, LightList.at(LPos).getLightPosition().z);
		Draw(MV, P, MV2);
		shape->draw(prog);
		prog->unbind();
	MV->popMatrix();
	// Draws HUD Teapot
	MV->pushMatrix();
		MV->translate(-1.3f, 0.9f, -3.0f);
		MV->rotate(0.3, 1.0f, 0.0f, 0.0f);
		MV->rotate(HUDRotate, 0.0f, 1.0f, 0.0f);
		MV->scale(0.5);
		prog->bind();
		// light position
		glUniform3f(prog->getUniform("lightPos"), LightList.at(LPos).getLightPosition().x,
			LightList.at(LPos).getLightPosition().y, LightList.at(LPos).getLightPosition().z);
		Draw(MV, P, MV2);
		shape2->draw(prog);
		prog->unbind();
	MV->popMatrix();
	// Multiply our View Matrix that will be transformed by our objects by our LookAt Matrix
	camera->applyViewMatrix(MV);
	MV->translate(0, -1, 0);
	MV2->pushMatrix();
	// Multiply our View Matrix that will be not be transformed by our objects by our LookAt Matrix
	camera->applyViewMatrix(MV2);
	// Transforms our Camera from World space to Camera Space so its constant
	glm::vec3 CamPos = camera->getCamPosition();
	CamPos = glm::vec3(MV2->topMatrix() * glm::vec4(CamPos, 0.0));
	glm::vec4 temp = glm::vec4(LightList.at(LPos).getLightPosition(), 1.0);
	temp = MV2->topMatrix() * temp;
	// Drawing the Bunnies
	MPos++;
	for (int i = 0; i < numOfBunnies; i++)
	{
		MV->pushMatrix();
		MV->translate(ObjectList.at(i).getTFactor());
		MV->rotate(ObjectList.at(i).getAngle(), ObjectList.at(i).getRFactor());
		MV->scale(sFactor);
		prog->bind();
		// light position
		glUniform3f(prog->getUniform("lightPos"), temp.x, temp.y, temp.z);
		// Camera Position
		glUniform3f(prog->getUniform("CamPos"), CamPos.x, CamPos.y, CamPos.z);
		Draw(MV, P, MV2);
		shape->draw(prog);
		prog->unbind();
		MV->popMatrix();
		MPos++;
	}
	// Drawing the teapots
	for (int i = numOfBunnies; i < numOfObjects; i++)
	{
		MV->pushMatrix();
		MV->translate(0.0f, -0.27f, 0.0f);
		MV->translate(ObjectList.at(i).getTFactor());
		MV->rotate(ObjectList.at(i).getAngle(), ObjectList.at(i).getRFactor());
		MV->scale(sFactor);
		prog->bind();
		// light position
		glUniform3f(prog->getUniform("lightPos"), temp.x, temp.y, temp.z);
		// Camera Position
		glUniform3f(prog->getUniform("CamPos"), CamPos.x, CamPos.y, CamPos.z);
		Draw(MV, P, MV2);
		shape2->draw(prog);
		prog->unbind();
		MV->popMatrix();
		MPos++;
	}
	// Drawing the sun
	MV->pushMatrix();
		MV->translate(0.0f, 5.0f, 1.0f);
		prog->bind();
		// light position
		glUniform3f(prog->getUniform("lightPos"), temp.x, temp.y, temp.z);
		// Camera position
		glUniform3f(prog->getUniform("CamPos"), CamPos.x, CamPos.y, CamPos.z);
		Draw(MV, P, MV2);
		shape3->draw(prog);
		MV->translate(0, -2, 0);
		prog->unbind();
	MV->popMatrix();
	// Drawing the ground
	MV->pushMatrix();
		MV->translate(1.0f, -0.6f, 6.0f);
		MV->rotate(-M_PI/2, 1.0f, 0.0f, 0.0f);
		MV->scale(40.0f);
		prog2->bind();
		texture->bind(prog2->getUniform("texture"));
		glUniformMatrix4fv(prog2->getUniform("MV2"), 1, GL_FALSE, glm::value_ptr(MV2->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix3fv(prog2->getUniform("Tex"), 1, GL_FALSE, glm::value_ptr(Tex));
		// light position
		glUniform3f(prog2->getUniform("lightPos"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(prog->getUniform("lightPos"), temp.x, temp.y, temp.z);
		// Camera position
		glUniform3f(prog2->getUniform("CamPos"), CamPos.x, CamPos.y, CamPos.z);
		// ambient component
		glUniform3f(prog2->getUniform("ka"), 0.3f, 0.3f, 0.3f);
		// specular component
		glUniform3f(prog2->getUniform("ks"), 1.0f, 1.0f, 0.3f);
		// shininess factor
		glUniform1f(prog2->getUniform("s"), MaterialList.at(MPos).getShiny());
		// light intensity factor
		glUniform1f(prog2->getUniform("i1"), LightList.at(LPos).getIntensity());
		shape4->draw(prog2);
		texture->unbind();
		MV->translate(0, -2, 0);
		prog2->unbind();
	MV->popMatrix();
	P->popMatrix();
	MPos = 0;
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Hides the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
