/*
Copyright 2022 Lee Taek Hee (Tech University of Korea)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Renderer.h"

using namespace std;

Renderer *g_Renderer = NULL;



void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Renderer Test
	//g_Renderer->DrawSolidRect(0, 0, 0, 4, 1, 0, 1, 1);
	//g_Renderer->Class0310_Rendering();
	//g_Renderer->DrawParticle();			// 문제 14
	g_Renderer->DrawParticle2();		// 문제 16
	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	RenderScene();
}

extern float CameraXOrbitRadians;		// 카메라 공전 회전각
extern float CameraYOrbitRadians;
extern float CameraYRotatingRadians;	// 카메라 자전 회전각
extern float CameraXMove;
extern float CameraZMove;


void KeyInput(unsigned char key, int x, int y)
{
	switch (key) {
	case 'b':

		break;
	}
	RenderScene();
}


void SpecialKeyInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:	// 왼쪽 방향키

		break;

	case GLUT_KEY_RIGHT: // 오른쪽 방향키

		break;

	case GLUT_KEY_UP:	// 위쪽 방향키

		break;

	case GLUT_KEY_DOWN:	// 아래쪽 방향키

		break;
	}

	RenderScene();
}

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(400, 0);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Game Software Engineering KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
	{
		std::cout << " GLEW Version is 3.0\n ";
	}
	else
	{
		std::cout << "GLEW 3.0 not supported\n ";
	}

	// Initialize Renderer
	g_Renderer = new Renderer(1000, 1000);
	if (!g_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyInput);

	glutMainLoop();

	delete g_Renderer;

    return 0;
}

