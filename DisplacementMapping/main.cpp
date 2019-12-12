#include <iostream>
#include <memory>
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <MouseCamera.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>

#include <stdlib.h>
#include <chrono>

const char* WINDOW_TITLE = "[CSCI-4800/5800] Shader and GPU Programming";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
GLint g_glutWindowIdentifier;

const static unsigned int TIMERMSECS = 33;

std::shared_ptr<MouseCameraf> camera = nullptr;
std::shared_ptr<Mesh> mesh = nullptr;

void g_init() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glewInit();

    camera = std::make_shared<MouseCameraf>(3.0f);
    camera->setPosition(18.0f, 1.5707f, 1.570f * 0.7f);

    mesh = std::make_shared<Mesh>();
    mesh->load("models/tessellated_plane.obj");
    mesh->loadShader("shaders/DisplacementMapping.vert", "shaders/DisplacementMapping.frag");
}

void g_glutReshapeFunc(int width, int height) {
	glViewport(0, 0, width, height);
    camera->setPerspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	glutPostRedisplay();
}

float velocity = 0.05f;
float amplitude = 0.7f;
void g_glutDisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Matrix4f transform = mesh->getTransform().toMatrix();
    Matrix4f view = camera->getViewMatrix();
    Matrix4f modelViewMatrix = transform * camera->getViewMatrix();
    Matrix3f normalMatrix = Matrix4f::NormalMatrix(modelViewMatrix);
    Matrix4f projectionMatrix = camera->getProjectionMatrix();

    mesh->beginRender();
    mesh->getShader()->uniformMatrix("projectionMatrix", projectionMatrix);
    mesh->getShader()->uniformMatrix("modelViewMatrix", modelViewMatrix);
    mesh->getShader()->uniformMatrix("normalMatrix", normalMatrix);
    mesh->getShader()->uniformVector("lightPosition", Vector3f(0.0f, 12.0f, 0.0f));
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	long time = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	time = (float)(time / 16.67); // This gives us a number that updates 60 times per second
	mesh->getShader()->uniform1f("time", time);
	mesh->getShader()->uniform1f("amplitude", amplitude);
	mesh->getShader()->uniform1f("velocity", velocity);
    mesh->endRender();

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
	case 'w': amplitude += 0.1f; break;
	case 's': amplitude -= 0.1f; break;
	case 'd': velocity += 0.02f; break;
	case 'a': velocity -= 0.02f; break;
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

