#include <iostream>
#include <memory>
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <MouseCamera.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>
#include <EnvironmentMap.h>

#include <stdlib.h>
#include <chrono>

const char* WINDOW_TITLE = "[CSCI-4800/5800] Shader and GPU Programming";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
GLint g_glutWindowIdentifier;

const static unsigned int TIMERMSECS = 33;

std::shared_ptr<MouseCameraf> camera = nullptr;
std::shared_ptr<Mesh> surface = nullptr;
std::shared_ptr<Mesh> ground = nullptr;
std::shared_ptr<EnvironmentMap> map = nullptr;

float velocity = 0.05f;
float amplitude = 0.7f;

void g_init() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glewInit();

    camera = std::make_shared<MouseCameraf>(3.0f);
    camera->setPosition(18.0f, 1.5707f, 1.570f * 0.7f);

    surface = std::make_shared<Mesh>();
    surface->load("models/tessellated_plane.obj");
    surface->loadShader("shaders/Ripple.vert", "shaders/Refraction.frag");

    ground = std::make_shared<Mesh>();
    ground->load("models/tessellated_plane.obj");
    ground->loadShader("shaders/SpecularMapping.vert", "shaders/SpecularMapping.frag");
    ground->setDiffuseTexture("textures/marble_diffuse.png");
    ground->setNormalTexture("textures/marble_normal.png");
    ground->setSpecularTexture("textures/marble_specular.png");
	ground->setPosition(0.0f, -amplitude - 1.0f, 0.0f);

	map = std::make_shared<EnvironmentMap>();
    map->load("images/clouds");
}

void g_glutReshapeFunc(int width, int height) {
	glViewport(0, 0, width, height);
    camera->setPerspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	glutPostRedisplay();
}

void g_glutDisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4f view = camera->getViewMatrix();
    Matrix4f projectionMatrix = camera->getProjectionMatrix();
    
    Matrix4f surfaceTransform = surface->getTransform().toMatrix();
    Matrix4f surfaceModelViewMatrix = surfaceTransform * camera->getViewMatrix();
    Matrix3f surfaceNormalMatrix = Matrix4f::NormalMatrix(surfaceModelViewMatrix);

	Vector3f lightPosition = Vector3f(0.0f, 12.0f, 0.0f);

    surface->beginRender();
    surface->getShader()->uniformMatrix("projectionMatrix", projectionMatrix);
    surface->getShader()->uniformMatrix("modelViewMatrix", surfaceModelViewMatrix);
    surface->getShader()->uniformMatrix("normalMatrix", surfaceNormalMatrix);
    surface->getShader()->uniformVector("lightPosition", lightPosition);
	surface->getShader()->uniformVector("cameraPosition", camera->getEye());
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	long time = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	time = (float)(time / 16.67); // This gives us a number that updates 60 times per second
	surface->getShader()->uniform1f("time", time);
	surface->getShader()->uniform1f("amplitude", amplitude);
	surface->getShader()->uniform1f("velocity", velocity);
    surface->endRender();

    Matrix4f groundTransform = ground->getTransform().toMatrix();
    Matrix4f groundModelViewMatrix = groundTransform * camera->getViewMatrix();
    Matrix3f groundNormalMatrix = Matrix4f::NormalMatrix(groundModelViewMatrix);

	ground->beginRender();
    ground->getShader()->uniformMatrix("projectionMatrix", projectionMatrix);
    ground->getShader()->uniformMatrix("modelViewMatrix", groundModelViewMatrix);
    ground->getShader()->uniformMatrix("normalMatrix", groundNormalMatrix);
    ground->getShader()->uniformVector("lightPosition", lightPosition);
    ground->endRender();

    glFlush();
}

void g_glutMotionFunc(int x, int y) {
	camera->onMouseMove(x, y);
	glutPostRedisplay();
}

void g_glutMouseFunc(int button, int state, int x, int y) {
	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) camera->onMouseButton(LB_DOWN, x, y);
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) camera->onMouseButton(RB_DOWN, x, y);
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP ) camera->onMouseButton(LB_UP, x, y);
    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_UP ) camera->onMouseButton(RB_UP, x, y);
}

void g_glutKeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': 
		amplitude += 0.1f; 
		ground->setPosition(0.0f, -amplitude - 1.0f, 0.0f);
		break;
	case 's': 
		amplitude -= 0.1f;
		ground->setPosition(0.0f, -amplitude - 1.0f, 0.0f);
		break;
	case 'd': velocity += 0.02f; break;
	case 'a': velocity -= 0.02f; break;
	case '1': surface->loadShader("shaders/Wave.vert", "shaders/Wireframe.frag"); break;
	case '2': surface->loadShader("shaders/Wave.vert", "shaders/Refraction.frag"); break;
	case '3': surface->loadShader("shaders/Ripple.vert", "shaders/Wireframe.frag"); break;
	case '4': surface->loadShader("shaders/Ripple.vert", "shaders/Refraction.frag"); break;
	}
}

void update(int value) {
    glutTimerFunc(TIMERMSECS, update, 0);
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	g_glutWindowIdentifier = glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(g_glutDisplayFunc);
	glutReshapeFunc(g_glutReshapeFunc);
    glutMotionFunc(g_glutMotionFunc);
    glutMouseFunc(g_glutMouseFunc);
	glutKeyboardFunc(g_glutKeyboardFunc);

    glutTimerFunc(TIMERMSECS, update, 0);

	g_init();

	glutMainLoop();
	return 0;
}

