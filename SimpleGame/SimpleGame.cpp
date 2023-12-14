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

void TurnOverCameraRotationAnimation(int value);

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Renderer Test
	//g_Renderer->DrawSolidRect(0, 0, 0, 4, 1, 0, 1, 1);
	//g_Renderer->Class0310_Rendering();
	//g_Renderer->DrawParticle();			// ¹®Á¦ 14
	g_Renderer->DrawParticle2();		// ¹®Á¦ 16
	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

ChessBoardSquare selectedPiece;
bool isPieceSelected = false;
int selectedX, selectedY;

void MouseInput(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) 
	{
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		//debug - print clicked spot
		std::cout << "Clicked position in world coordinates: (" << posX << ", " << posY << ", " << posZ << ")" << std::endl;

		int boardX = static_cast<int>((posX + 0.700) / 0.170);
		int boardY = static_cast<int>((0.700 - posY) / 0.170);

		if (boardX >= 0 && boardX < 8 && boardY >= 0 && boardY < 8)
		{
			if (g_Renderer->GetChessPieceAt(boardY, boardX).piece != ChessPiece::Empty)
			{
				//select piece
				selectedPiece = g_Renderer->GetChessPieceAt(boardX, boardY);
				isPieceSelected = true;
				selectedX = boardX;
				selectedY = boardY;

				//debug - print selected piece
				std::cout << "selected: " << boardX << "," << boardY << "," << endl;
			}
			else
			{
				//debug - print if selected piece is empty
				std::cout << "selected: " << boardX << "," << boardY << ", is empty" << endl;
			}

		}
	}
}

// Renderer.cpp Àü¿ªº¯¼ö ºÒ·¯¿À±â
extern float TurnChangeRadians;

// Àü¿ªº¯¼ö
bool IsTurnOver = false;

void KeyInput(unsigned char key, int x, int y)
{
	switch (key) {
	case 't':
		if (IsTurnOver == false) {
			IsTurnOver = true;
			TurnOverCameraRotationAnimation(1);
			//¾Ö´Ï¸ÞÀÌ¼Ç
		}
		else if (IsTurnOver == true) {
			IsTurnOver = false;
			TurnOverCameraRotationAnimation(1);
		}
		break;
	}
	RenderScene();
}

void TurnOverCameraRotationAnimation(int value) {
	TurnChangeRadians += 2.0f;

	if (((int)TurnChangeRadians % 180) != 0) {	//180±îÁö º¯°æ
		glutTimerFunc(4, TurnOverCameraRotationAnimation, 1);
	}
	else {

	}

}

void SpecialKeyInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:	// ¿ÞÂÊ ¹æÇâÅ°

		break;

	case GLUT_KEY_RIGHT: // ¿À¸¥ÂÊ ¹æÇâÅ°

		break;

	case GLUT_KEY_UP:	// À§ÂÊ ¹æÇâÅ°

		break;

	case GLUT_KEY_DOWN:	// ¾Æ·¡ÂÊ ¹æÇâÅ°

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

	g_Renderer->InitializeChessBoard();
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyInput);

	glutMainLoop();

	delete g_Renderer;

    return 0;
}

