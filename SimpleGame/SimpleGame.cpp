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

void AllZFrontMovingAnimation(int value);
void AllZBackMovingAnimation(int value);
void UpperBodyRotatingAnimation(int value);
void UpLeftGunRotatingAnimation(int value);
void FrontLeftGunRotatingAnimation(int value);





void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Renderer Test
	//g_Renderer->DrawSolidRect(0, 0, 0, 4, 1, 0, 1, 1);
	//g_Renderer->Class0310_Rendering();
	//g_Renderer->DrawParticle();			// ���� 14
	g_Renderer->DrawParticle2();		// ���� 16
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

extern float CameraXOrbitRadians;		// ī�޶� ���� ȸ����
extern float CameraYOrbitRadians;
extern float CameraYRotatingRadians;	// ī�޶� ���� ȸ����
extern float CameraXMove;
extern float CameraZMove;

extern float AllZMove;					// ��� ��ü z�� ������
extern float UpperBodyYRotatingRadians;	// ��ü ������
extern float CraneUpLeftGunXRadians;	// ���� ���� ȸ����
extern float CraneFrontLeftGunXMove;	// ���� �� ���� x ���� ��
extern float CraneFrontLeftGunYRadians; // �� ���� ���� Y ȸ����

bool b_AllZFrontMovingAnimation = false;	// ���� ���� �ִϸ��̼�
bool b_AllZBackMovingAnimation = false;
bool b_UpperBodyRotatingAnimation = false;
bool b_UpLeftGunRotatingAnimation = false;	// ���� ������ ������ �ִϸ��̼�
bool b_FrontLeftGunRotatingAnimation = false;

void KeyInput(unsigned char key, int x, int y)
{
	switch (key) {
	case 'b':
		if (!b_AllZFrontMovingAnimation) {
			b_AllZFrontMovingAnimation = true;
			AllZFrontMovingAnimation(1);
		}
		else
			b_AllZFrontMovingAnimation = false;

		break;

	case 'B':
		if (!b_AllZBackMovingAnimation) {
			b_AllZBackMovingAnimation = true;
			AllZBackMovingAnimation(1);
		}
		else
			b_AllZBackMovingAnimation = false;

		break;

	case 'm':
		if (!b_UpperBodyRotatingAnimation) {
			b_UpperBodyRotatingAnimation = true;
			UpperBodyRotatingAnimation(1);
		}
		else
			b_UpperBodyRotatingAnimation = false;
		
		break;

	case 'M':
		break;

	case 'f':
		if (!b_FrontLeftGunRotatingAnimation) {
			b_FrontLeftGunRotatingAnimation = true;
			FrontLeftGunRotatingAnimation(1);
		}
		else
			b_FrontLeftGunRotatingAnimation = false;
		
		break;

	case 'e':
		CraneFrontLeftGunXMove += 0.02f;
		break;

	case 't':
		if (!b_UpLeftGunRotatingAnimation) {
			b_UpLeftGunRotatingAnimation = true;
			UpLeftGunRotatingAnimation(1);
		}
		else
			b_UpLeftGunRotatingAnimation = false;
		break;


	case 'z':
		CameraZMove += 0.2f;
		break;
	case 'Z':
		CameraZMove -= 0.2f;
		break;

	case 'x':
		CameraXMove += 0.2f;
		break;
	case 'X':
		CameraXMove -= 0.2f;
		break;
		
	case 'y':
		CameraYRotatingRadians += 10.f;
		break;
	case 'Y':
		CameraYRotatingRadians -= 10.f;
		break;
	

	case 'r':
		CameraYOrbitRadians += 10.f;
		break;
	case 'R':
		CameraYOrbitRadians -= 10.f;
		break;
	}

	RenderScene();
}

void AllZFrontMovingAnimation(int value) {
	if (b_AllZFrontMovingAnimation) {
		AllZMove += 0.01f;
		glutTimerFunc(16, AllZFrontMovingAnimation, 1);
	}
}

void AllZBackMovingAnimation(int value) {
	if (b_AllZBackMovingAnimation) {
		AllZMove -= 0.01f;
		glutTimerFunc(16, AllZBackMovingAnimation, 1);
	}
}

void UpperBodyRotatingAnimation(int value) {
	if (b_UpperBodyRotatingAnimation) {
		UpperBodyYRotatingRadians += 8.f;
		glutTimerFunc(16, UpperBodyRotatingAnimation, 1);
	}
}

void UpLeftGunRotatingAnimation(int value) {
	if (b_UpLeftGunRotatingAnimation) {
		CraneUpLeftGunXRadians += 1.f;
		glutTimerFunc(16, UpLeftGunRotatingAnimation, 1);
	}
}

void FrontLeftGunRotatingAnimation(int value) {
	if (b_FrontLeftGunRotatingAnimation) {
		CraneFrontLeftGunYRadians += -1.f;
		glutTimerFunc(16, FrontLeftGunRotatingAnimation, 1);
	}
}

void SpecialKeyInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:	// ���� ����Ű

		break;

	case GLUT_KEY_RIGHT: // ������ ����Ű

		break;

	case GLUT_KEY_UP:	// ���� ����Ű

		break;

	case GLUT_KEY_DOWN:	// �Ʒ��� ����Ű

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

