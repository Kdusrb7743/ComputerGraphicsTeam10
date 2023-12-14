

#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Renderer.h"

using namespace std;

Renderer *g_Renderer = NULL;

void TurnOverCameraRotationAnimation(int value);
bool isCameraRotated = false;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
	g_Renderer->DrawParticle2();		
	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}


bool IsTurnOver = false;

bool IsPathClear(int startX, int startY, int endX, int endY, ChessPiece piece, PieceColor color) {
	int deltaX = endX - startX;
	int deltaY = endY - startY;

	// Rook and Queen (Horizontal and Vertical)
	if (piece == ChessPiece::Rook || (piece == ChessPiece::Queen && (deltaX == 0 || deltaY == 0))) {
		int stepX = (deltaX != 0) ? (deltaX > 0 ? 1 : -1) : 0;
		int stepY = (deltaY != 0) ? (deltaY > 0 ? 1 : -1) : 0;

		for (int x = startX + stepX, y = startY + stepY; x != endX || y != endY; x += stepX, y += stepY) {
			ChessBoardSquare square = g_Renderer->GetChessPieceAt(x, y);
			if (square.piece != ChessPiece::Empty) {
				if (square.color == color || (x != endX || y != endY)) {
					return false;
				}
			}
		}
	}

	// Bishop and Queen (Diagonal)
	if (piece == ChessPiece::Bishop || (piece == ChessPiece::Queen && deltaX != 0 && deltaY != 0)) {
		int stepX = deltaX > 0 ? 1 : -1;
		int stepY = deltaY > 0 ? 1 : -1;

		for (int x = startX + stepX, y = startY + stepY; x != endX && y != endY; x += stepX, y += stepY) {
			ChessBoardSquare square = g_Renderer->GetChessPieceAt(x, y);
			if (square.piece != ChessPiece::Empty) {
				if (square.color == color || (x != endX || y != endY)) {
					return false;
				}
			}
		}
	}

	// Check the target square for ally piece
    ChessBoardSquare targetSquare = g_Renderer->GetChessPieceAt(endX, endY);
	if (targetSquare.piece != ChessPiece::Empty && targetSquare.color == color) {
		return false;
	}
	
	// Pawn, Knight, and King do not use this function for path checking


	return true;
}

bool IsValidMovePawn(int startX, int startY, int endX, int endY, PieceColor color) {
	int direction = (color == PieceColor::White) ? -1 : 1;

	// check movement
	if (startY == endY) {
		if (endX == startX + direction) {
			return g_Renderer->GetChessPieceAt(endX, endY).piece == ChessPiece::Empty;
		}
		if (startX == (color == PieceColor::White ? 6 : 1) && endX == startX + 2 * direction) {
			// en'passant check
			return g_Renderer->GetChessPieceAt(startX + direction, startY).piece == ChessPiece::Empty &&
				g_Renderer->GetChessPieceAt(endX, endY).piece == ChessPiece::Empty;
		}
	}

	// kill check
	if (abs(endY - startY) == 1 && endX == startX + direction) {
		ChessBoardSquare targetSquare = g_Renderer->GetChessPieceAt(endX, endY);
		return targetSquare.piece != ChessPiece::Empty && targetSquare.color != color;
	}

	return false;
}

bool IsValidMoveRook(int startX, int startY, int endX, int endY, PieceColor color) {
	if (startX == endX || startY == endY) {
		return IsPathClear(startX, startY, endX, endY, ChessPiece::Rook, color);
	}
	return false;
}

bool IsValidMoveKnight(int startX, int startY, int endX, int endY, PieceColor color) {
	// check movement
	if ((abs(startX - endX) == 2 && abs(startY - endY) == 1) || (abs(startX - endX) == 1 && abs(startY - endY) == 2)) {
		ChessBoardSquare targetSquare = g_Renderer->GetChessPieceAt(endX, endY);
		// kill check
		if (targetSquare.piece != ChessPiece::Empty && targetSquare.color == color) return false;
		return true;
	}
	return false;
}

bool IsValidMoveBishop(int startX, int startY, int endX, int endY, PieceColor color) {
	if (abs(startX - endX) == abs(startY - endY)) {
		return IsPathClear(startX, startY, endX, endY, ChessPiece::Bishop, color);
	}
	return false;
}

bool IsValidMoveQueen(int startX, int startY, int endX, int endY, PieceColor color) {
	return IsValidMoveRook(startX, startY, endX, endY, color) || IsValidMoveBishop(startX, startY, endX, endY, color);
}

bool IsValidMoveKing(int startX, int startY, int endX, int endY, PieceColor color) {
	return abs(startX - endX) <= 1 && abs(startY - endY) <= 1 && IsPathClear(startX, startY, endX, endY, ChessPiece::King, color);
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

		int boardX, boardY;

		if (isCameraRotated) {
			//flip coordinates if flipped
			boardX = 7 - static_cast<int>((posX + 0.700) / 0.170);
			boardY = 7 - static_cast<int>((0.700 - posY) / 0.170);
		}
		else {
			boardX = static_cast<int>((posX + 0.700) / 0.170);
			boardY = static_cast<int>((0.700 - posY) / 0.170);
		}

		if (boardX >= 0 && boardX < 8 && boardY >= 0 && boardY < 8)
		{
			if (!isPieceSelected)
			{
				//select
				if (g_Renderer->GetChessPieceAt(boardY, boardX).piece != ChessPiece::Empty)
				{
					selectedPiece = g_Renderer->GetChessPieceAt(boardY, boardX);
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
			else
			{
				//check if it's valid move
				bool isValidMove = false;
				switch (selectedPiece.piece) {
				case ChessPiece::Pawn:
					isValidMove = IsValidMovePawn(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				case ChessPiece::Rook:
					isValidMove = IsValidMoveRook(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				case ChessPiece::Knight:
					isValidMove = IsValidMoveKnight(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				case ChessPiece::Bishop:
					isValidMove = IsValidMoveBishop(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				case ChessPiece::Queen:
					isValidMove = IsValidMoveQueen(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				case ChessPiece::King:
					isValidMove = IsValidMoveKing(selectedY, selectedX, boardY, boardX, selectedPiece.color);
					break;
				default:
					break;
				}
				//place
				if (isValidMove)
				{
					g_Renderer->SetChessPieceAt(selectedY, selectedX, { ChessPiece::Empty, PieceColor::None });
					g_Renderer->SetChessPieceAt(boardY, boardX, selectedPiece);
					//debug - print moved message
					std::cout << "moved to: " << boardX << "," << boardY << std::endl;
					IsTurnOver = false;
					TurnOverCameraRotationAnimation(1);
				}
				else
				{
					std::cout << "invalid move, try again" << endl;
				}
				isPieceSelected = false;
			}
			

		}
	}
}

// Renderer.cpp
extern float TurnChangeRadians;



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

	if (((int)TurnChangeRadians % 180) != 0) {	//180
		glutTimerFunc(4, TurnOverCameraRotationAnimation, 1);
		isCameraRotated = !isCameraRotated;
	}
	else {

	}
	

}

void SpecialKeyInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:

		break;

	case GLUT_KEY_RIGHT: 

		break;

	case GLUT_KEY_UP:

		break;

	case GLUT_KEY_DOWN:

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
	glutCreateWindow("ChessGL");

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

