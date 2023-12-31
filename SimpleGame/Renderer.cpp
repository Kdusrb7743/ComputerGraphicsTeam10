﻿#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool loadOBJ(
	const char*,
	std::vector<glm::vec3>&,
	std::vector<glm::vec2>&,
	std::vector<glm::vec3>&
);

void Renderer::InitializeChessBoard()
{
	//hard-coding pieces
	//chess board init. starting with no pieces
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = { ChessPiece::Empty, PieceColor::None };
		}
	}

	//pawn(black, white)
	for (int i = 0; i < 8; i++) {
		board[1][i] = { ChessPiece::Pawn, PieceColor::Black };
		board[6][i] = { ChessPiece::Pawn, PieceColor::White };
	}

	//other black pieces
	board[0][0] = { ChessPiece::Rook, PieceColor::Black };
	board[0][7] = { ChessPiece::Rook, PieceColor::Black };
	board[0][1] = { ChessPiece::Knight, PieceColor::Black };
	board[0][6] = { ChessPiece::Knight, PieceColor::Black };
	board[0][2] = { ChessPiece::Bishop, PieceColor::Black };
	board[0][5] = { ChessPiece::Bishop, PieceColor::Black };
	board[0][3] = { ChessPiece::Queen, PieceColor::Black };
	board[0][4] = { ChessPiece::King, PieceColor::Black };

	//other white pieces
	board[7][0] = { ChessPiece::Rook, PieceColor::White };
	board[7][7] = { ChessPiece::Rook, PieceColor::White };
	board[7][1] = { ChessPiece::Knight, PieceColor::White };
	board[7][6] = { ChessPiece::Knight, PieceColor::White };
	board[7][2] = { ChessPiece::Bishop, PieceColor::White };
	board[7][5] = { ChessPiece::Bishop, PieceColor::White };
	board[7][3] = { ChessPiece::Queen, PieceColor::White };
	board[7][4] = { ChessPiece::King, PieceColor::White };
}

ChessBoardSquare Renderer::GetChessPieceAt(int x, int y)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
	{
		return board[x][y];
	}
	else
	{
		return { ChessPiece::Empty, PieceColor::None };
	}
}

void Renderer::SetChessPieceAt(int x, int y, ChessBoardSquare piece)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
	{
		board[x][y] = piece;
	}
}

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	//Class0310();			// ¿©±â¼­ ¹ÙÀÎµå ¹öÆÛ ÇØÁØ´Ù.
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();
	//CreateParticleVBO(int numParticleCount = 1000);	//±³¼ö´ÔÀº ¿©±â¼­ ÇØÁÜ

	/*
	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
	*/

	// ÅØ½ºÃ³ ·Îµå
	m_ChessboardTexture = CreatePngTexture("./Texture/Chessboard_Base_Color.png", GL_NEAREST);
	m_WhiteTexture = CreatePngTexture("./Texture/White.png", GL_NEAREST);
	m_BlackTexture = CreatePngTexture("./Texture/Black.png", GL_NEAREST);

}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	//Load Png
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		std::cout << "PNG image loading failed:" << filePath << std::endl;
		assert(0);
	}

	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//½¦ÀÌ´õ ¿ÀºêÁ§Æ® »ý¼º
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//½¦ÀÌ´õ ÄÚµå¸¦ ½¦ÀÌ´õ ¿ÀºêÁ§Æ®¿¡ ÇÒ´ç
	glShaderSource(ShaderObj, 1, p, Lengths);

	//ÇÒ´çµÈ ½¦ÀÌ´õ ÄÚµå¸¦ ÄÄÆÄÀÏ
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj °¡ ¼º°øÀûÀ¸·Î ÄÄÆÄÀÏ µÇ¾ú´ÂÁö È®ÀÎ
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL ÀÇ shader log µ¥ÀÌÅÍ¸¦ °¡Á®¿È
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram ¿¡ attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); 

	if (ShaderProgram == 0) { 
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach
	glLinkProgram(ShaderProgram);

	//
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}


float CenterCrossLinePos[] =	
{
	0.0f,  5.0f, 0.0f,	0.0, 1.0, 0.0,	
	0.0f, -5.0f, 0.0f,	0.0, 1.0, 0.0,
	5.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,	
   -5.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,
	0.0f,  0.0f, 5.0f,  0.0, 0.0, 1.0,	
	0.0f,  0.0f, -5.0f, 0.0, 0.0, 1.0,

};


// 체스 보드판
std::vector<glm::vec3> ChessboardVertices;
std::vector<glm::vec2> ChessboardUVs;
std::vector<glm::vec3> ChessboardUVsNormals;

//---------------------------------------체스말-----------------------------
std::vector<glm::vec3> W_Pawn1Vertices;
std::vector<glm::vec2> W_Pawn1UVs;
std::vector<glm::vec3> W_Pawn1UVsNormals;

std::vector<glm::vec3> W_Pawn2Vertices;
std::vector<glm::vec2> W_Pawn2UVs;
std::vector<glm::vec3> W_Pawn2UVsNormals;

std::vector<glm::vec3> W_Pawn3Vertices;
std::vector<glm::vec2> W_Pawn3UVs;
std::vector<glm::vec3> W_Pawn3UVsNormals;

std::vector<glm::vec3> W_Pawn4Vertices;
std::vector<glm::vec2> W_Pawn4UVs;
std::vector<glm::vec3> W_Pawn4UVsNormals;

std::vector<glm::vec3> W_Pawn5Vertices;
std::vector<glm::vec2> W_Pawn5UVs;
std::vector<glm::vec3> W_Pawn5UVsNormals;

std::vector<glm::vec3> W_Pawn6Vertices;
std::vector<glm::vec2> W_Pawn6UVs;
std::vector<glm::vec3> W_Pawn6UVsNormals;

std::vector<glm::vec3> W_Pawn7Vertices;
std::vector<glm::vec2> W_Pawn7UVs;
std::vector<glm::vec3> W_Pawn7UVsNormals;

std::vector<glm::vec3> W_Pawn8Vertices;
std::vector<glm::vec2> W_Pawn8UVs;
std::vector<glm::vec3> W_Pawn8UVsNormals;

std::vector<glm::vec3> W_Bishop1Vertices;
std::vector<glm::vec2> W_Bishop1UVs;
std::vector<glm::vec3> W_Bishop1UVsNormals;

std::vector<glm::vec3> W_Bishop2Vertices;
std::vector<glm::vec2> W_Bishop2UVs;
std::vector<glm::vec3> W_Bishop2UVsNormals;

std::vector<glm::vec3> W_Knight1Vertices;
std::vector<glm::vec2> W_Knight1UVs;
std::vector<glm::vec3> W_Knight1UVsNormals;

std::vector<glm::vec3> W_Knight2Vertices;
std::vector<glm::vec2> W_Knight2UVs;
std::vector<glm::vec3> W_Knight2UVsNormals;

std::vector<glm::vec3> W_Rook1Vertices;
std::vector<glm::vec2> W_Rook1UVs;
std::vector<glm::vec3> W_Rook1UVsNormals;

std::vector<glm::vec3> W_Rook2Vertices;
std::vector<glm::vec2> W_Rook2UVs;
std::vector<glm::vec3> W_Rook2UVsNormals;

std::vector<glm::vec3> W_QueenVertices;
std::vector<glm::vec2> W_QueenUVs;
std::vector<glm::vec3> W_QueenUVsNormals;

std::vector<glm::vec3> W_KingVertices;
std::vector<glm::vec2> W_KingUVs;
std::vector<glm::vec3> W_KingUVsNormals;

//---------------------------------------흑색 체스말»-----------------------------
std::vector<glm::vec3> B_Pawn1Vertices;
std::vector<glm::vec2> B_Pawn1UVs;
std::vector<glm::vec3> B_Pawn1UVsNormals;

std::vector<glm::vec3> B_Pawn2Vertices;
std::vector<glm::vec2> B_Pawn2UVs;
std::vector<glm::vec3> B_Pawn2UVsNormals;

std::vector<glm::vec3> B_Pawn3Vertices;
std::vector<glm::vec2> B_Pawn3UVs;
std::vector<glm::vec3> B_Pawn3UVsNormals;

std::vector<glm::vec3> B_Pawn4Vertices;
std::vector<glm::vec2> B_Pawn4UVs;
std::vector<glm::vec3> B_Pawn4UVsNormals;

std::vector<glm::vec3> B_Pawn5Vertices;
std::vector<glm::vec2> B_Pawn5UVs;
std::vector<glm::vec3> B_Pawn5UVsNormals;

std::vector<glm::vec3> B_Pawn6Vertices;
std::vector<glm::vec2> B_Pawn6UVs;
std::vector<glm::vec3> B_Pawn6UVsNormals;

std::vector<glm::vec3> B_Pawn7Vertices;
std::vector<glm::vec2> B_Pawn7UVs;
std::vector<glm::vec3> B_Pawn7UVsNormals;

std::vector<glm::vec3> B_Pawn8Vertices;
std::vector<glm::vec2> B_Pawn8UVs;
std::vector<glm::vec3> B_Pawn8UVsNormals;

std::vector<glm::vec3> B_Bishop1Vertices;
std::vector<glm::vec2> B_Bishop1UVs;
std::vector<glm::vec3> B_Bishop1UVsNormals;

std::vector<glm::vec3> B_Bishop2Vertices;
std::vector<glm::vec2> B_Bishop2UVs;
std::vector<glm::vec3> B_Bishop2UVsNormals;

std::vector<glm::vec3> B_Knight1Vertices;
std::vector<glm::vec2> B_Knight1UVs;
std::vector<glm::vec3> B_Knight1UVsNormals;

std::vector<glm::vec3> B_Knight2Vertices;
std::vector<glm::vec2> B_Knight2UVs;
std::vector<glm::vec3> B_Knight2UVsNormals;

std::vector<glm::vec3> B_Rook1Vertices;
std::vector<glm::vec2> B_Rook1UVs;
std::vector<glm::vec3> B_Rook1UVsNormals;

std::vector<glm::vec3> B_Rook2Vertices;
std::vector<glm::vec2> B_Rook2UVs;
std::vector<glm::vec3> B_Rook2UVsNormals;

std::vector<glm::vec3> B_QueenVertices;
std::vector<glm::vec2> B_QueenUVs;
std::vector<glm::vec3> B_QueenUVsNormals;

std::vector<glm::vec3> B_KingVertices;
std::vector<glm::vec2> B_KingUVs;
std::vector<glm::vec3> B_KingUVsNormals;


void Renderer::CreateVertexBufferObjects()
{

	//isCube ? Load_Object("CraneBottom3.obj") : Load_Object("CraneBottom3.obj");

	//bool res = loadOBJ("CraneBottom.obj", CraneBottomVertices, CraneBottomUVs, CraneBottomNormals);
	bool W_Chessboard = loadOBJ("Chess_Object\\Chessboard.obj", ChessboardVertices, ChessboardUVs, ChessboardUVsNormals);
	bool W_Pawn1 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn1Vertices, W_Pawn1UVs, W_Pawn1UVsNormals);
	bool W_Pawn2 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn2Vertices, W_Pawn2UVs, W_Pawn2UVsNormals);
	bool W_Pawn3 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn3Vertices, W_Pawn3UVs, W_Pawn3UVsNormals);
	bool W_Pawn4 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn4Vertices, W_Pawn4UVs, W_Pawn4UVsNormals);
	bool W_Pawn5 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn5Vertices, W_Pawn5UVs, W_Pawn5UVsNormals);
	bool W_Pawn6 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn6Vertices, W_Pawn6UVs, W_Pawn6UVsNormals);
	bool W_Pawn7 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn7Vertices, W_Pawn7UVs, W_Pawn7UVsNormals);
	bool W_Pawn8 = loadOBJ("Chess_Object\\Pawn.obj", W_Pawn8Vertices, W_Pawn8UVs, W_Pawn8UVsNormals);
	bool W_Bishop1 = loadOBJ("Chess_Object\\Bishop.obj", W_Bishop1Vertices, W_Bishop1UVs, W_Bishop1UVsNormals);
	bool W_Bishop2 = loadOBJ("Chess_Object\\Bishop.obj", W_Bishop2Vertices, W_Bishop2UVs, W_Bishop2UVsNormals);
	bool W_Knight1 = loadOBJ("Chess_Object\\Knight.obj", W_Knight1Vertices, W_Knight1UVs, W_Knight1UVsNormals);
	bool W_Knight2 = loadOBJ("Chess_Object\\Knight.obj", W_Knight2Vertices, W_Knight2UVs, W_Knight2UVsNormals);
	bool W_Rook1 = loadOBJ("Chess_Object\\Rook.obj", W_Rook1Vertices, W_Rook1UVs, W_Rook1UVsNormals);
	bool W_Rook2 = loadOBJ("Chess_Object\\Rook.obj", W_Rook2Vertices, W_Rook2UVs, W_Rook2UVsNormals);
	bool W_Queen = loadOBJ("Chess_Object\\Queen.obj", W_QueenVertices, W_QueenUVs, W_QueenUVsNormals);
	bool W_King = loadOBJ("Chess_Object\\King.obj", W_KingVertices, W_KingUVs, W_KingUVsNormals);

	bool B_Pawn1 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn1Vertices, B_Pawn1UVs, B_Pawn1UVsNormals);
	bool B_Pawn2 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn2Vertices, B_Pawn2UVs, B_Pawn2UVsNormals);
	bool B_Pawn3 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn3Vertices, B_Pawn3UVs, B_Pawn3UVsNormals);
	bool B_Pawn4 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn4Vertices, B_Pawn4UVs, B_Pawn4UVsNormals);
	bool B_Pawn5 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn5Vertices, B_Pawn5UVs, B_Pawn5UVsNormals);
	bool B_Pawn6 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn6Vertices, B_Pawn6UVs, B_Pawn6UVsNormals);
	bool B_Pawn7 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn7Vertices, B_Pawn7UVs, B_Pawn7UVsNormals);
	bool B_Pawn8 = loadOBJ("Chess_Object\\Pawn.obj", B_Pawn8Vertices, B_Pawn8UVs, B_Pawn8UVsNormals);
	bool B_Bishop1 = loadOBJ("Chess_Object\\Bishop.obj", B_Bishop1Vertices, B_Bishop1UVs, B_Bishop1UVsNormals);
	bool B_Bishop2 = loadOBJ("Chess_Object\\Bishop.obj", B_Bishop2Vertices, B_Bishop2UVs, B_Bishop2UVsNormals);
	bool B_Knight1 = loadOBJ("Chess_Object\\Knight.obj", B_Knight1Vertices, B_Knight1UVs, B_Knight1UVsNormals);
	bool B_Knight2 = loadOBJ("Chess_Object\\Knight.obj", B_Knight2Vertices, B_Knight2UVs, B_Knight2UVsNormals);
	bool B_Rook1 = loadOBJ("Chess_Object\\Rook.obj", B_Rook1Vertices, B_Rook1UVs, B_Rook1UVsNormals);
	bool B_Rook2 = loadOBJ("Chess_Object\\Rook.obj", B_Rook2Vertices, B_Rook2UVs, B_Rook2UVsNormals);
	bool B_Queen = loadOBJ("Chess_Object\\Queen.obj", B_QueenVertices, B_QueenUVs, B_QueenUVsNormals);
	bool B_King = loadOBJ("Chess_Object\\King.obj", B_KingVertices, B_KingUVs, B_KingUVsNormals);


	glGenBuffers(1, &m_CenterCrossLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CenterCrossLinePos), CenterCrossLinePos, GL_STATIC_DRAW);


	glGenBuffers(1, &m_ChessboardVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardVBO);
	glBufferData(GL_ARRAY_BUFFER, ChessboardVertices.size() * sizeof(glm::vec3), &ChessboardVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_ChessboardUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardUVVBO);
	glBufferData(GL_ARRAY_BUFFER, ChessboardUVs.size() * sizeof(glm::vec2), &ChessboardUVs[0], GL_STATIC_DRAW);


	glGenBuffers(1, &m_ChessboardNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, ChessboardUVsNormals.size() * sizeof(glm::vec3), &ChessboardUVsNormals[0], GL_STATIC_DRAW);


	//µ
	//
	glGenBuffers(1, &m_W_Pawn1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn1Vertices.size() * sizeof(glm::vec3), &W_Pawn1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn2Vertices.size() * sizeof(glm::vec3), &W_Pawn2Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn3VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn3VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn3Vertices.size() * sizeof(glm::vec3), &W_Pawn3Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn4VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn4VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn4Vertices.size() * sizeof(glm::vec3), &W_Pawn4Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn5VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn5VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn5Vertices.size() * sizeof(glm::vec3), &W_Pawn5Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn6VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn6VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn6Vertices.size() * sizeof(glm::vec3), &W_Pawn6Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn7VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn7VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn7Vertices.size() * sizeof(glm::vec3), &W_Pawn7Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Pawn8VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn8VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn8Vertices.size() * sizeof(glm::vec3), &W_Pawn8Vertices[0], GL_STATIC_DRAW);

	
	glGenBuffers(1, &m_PawnUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PawnUVVBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn1UVs.size() * sizeof(glm::vec2), &W_Pawn1UVs[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &m_PawnNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_PawnNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_Pawn1UVsNormals.size() * sizeof(glm::vec3), &W_Pawn1UVsNormals[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &m_W_Bishop1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Bishop1Vertices.size() * sizeof(glm::vec3), &W_Bishop1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Bishop2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Bishop2Vertices.size() * sizeof(glm::vec3), &W_Bishop2Vertices[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &m_BishopNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_BishopNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_Bishop1UVsNormals.size() * sizeof(glm::vec3), &W_Bishop1UVsNormals[0], GL_STATIC_DRAW);


	//
	glGenBuffers(1, &m_W_Knight1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Knight1Vertices.size() * sizeof(glm::vec3), &W_Knight1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Knight2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Knight2Vertices.size() * sizeof(glm::vec3), &W_Knight2Vertices[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &m_KnightNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_KnightNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_Knight1UVsNormals.size() * sizeof(glm::vec3), &W_Knight1UVsNormals[0], GL_STATIC_DRAW);


	//
	glGenBuffers(1, &m_W_Rook1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Rook1Vertices.size() * sizeof(glm::vec3), &W_Rook1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Rook2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Rook2Vertices.size() * sizeof(glm::vec3), &W_Rook2Vertices[0], GL_STATIC_DRAW);

	//
	glGenBuffers(1, &m_RookNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_RookNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_Rook1UVsNormals.size() * sizeof(glm::vec3), &W_Rook1UVsNormals[0], GL_STATIC_DRAW);


	// 
	glGenBuffers(1, &m_W_QueenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_QueenVBO);
	glBufferData(GL_ARRAY_BUFFER, W_QueenVertices.size() * sizeof(glm::vec3), &W_QueenVertices[0], GL_STATIC_DRAW);

	// 
	glGenBuffers(1, &m_QueenNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QueenNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_QueenUVsNormals.size() * sizeof(glm::vec3), &W_QueenUVsNormals[0], GL_STATIC_DRAW);


	// 
	glGenBuffers(1, &m_W_KingVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_KingVBO);
	glBufferData(GL_ARRAY_BUFFER, W_KingVertices.size() * sizeof(glm::vec3), &W_KingVertices[0], GL_STATIC_DRAW);

	// Å· ³ë¸»
	glGenBuffers(1, &m_KingNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_KingNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, W_KingUVsNormals.size() * sizeof(glm::vec3), &W_KingUVsNormals[0], GL_STATIC_DRAW);


	// ---------------------------------------------------------
	// 
	glGenBuffers(1, &m_B_Pawn1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn1VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn1Vertices.size() * sizeof(glm::vec3), &B_Pawn1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn2VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn2Vertices.size() * sizeof(glm::vec3), &B_Pawn2Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn3VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn3VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn3Vertices.size() * sizeof(glm::vec3), &B_Pawn3Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn4VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn4VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn4Vertices.size() * sizeof(glm::vec3), &B_Pawn4Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn5VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn5VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn5Vertices.size() * sizeof(glm::vec3), &B_Pawn5Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn6VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn6VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn6Vertices.size() * sizeof(glm::vec3), &B_Pawn6Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn7VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn7VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn7Vertices.size() * sizeof(glm::vec3), &B_Pawn7Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Pawn8VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Pawn8VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Pawn8Vertices.size() * sizeof(glm::vec3), &B_Pawn8Vertices[0], GL_STATIC_DRAW);


	// 
	glGenBuffers(1, &m_B_Bishop1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Bishop1VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Bishop1Vertices.size() * sizeof(glm::vec3), &B_Bishop1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Bishop2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Bishop2VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Bishop2Vertices.size() * sizeof(glm::vec3), &B_Bishop2Vertices[0], GL_STATIC_DRAW);


	// 
	glGenBuffers(1, &m_B_Knight1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Knight1VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Knight1Vertices.size() * sizeof(glm::vec3), &B_Knight1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Knight2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Knight2VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Knight2Vertices.size() * sizeof(glm::vec3), &B_Knight2Vertices[0], GL_STATIC_DRAW);

	// 
	glGenBuffers(1, &m_B_Rook1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Rook1VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Rook1Vertices.size() * sizeof(glm::vec3), &B_Rook1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_B_Rook2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_Rook2VBO);
	glBufferData(GL_ARRAY_BUFFER, B_Rook2Vertices.size() * sizeof(glm::vec3), &B_Rook2Vertices[0], GL_STATIC_DRAW);

	// 
	glGenBuffers(1, &m_B_QueenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_QueenVBO);
	glBufferData(GL_ARRAY_BUFFER, B_QueenVertices.size() * sizeof(glm::vec3), &B_QueenVertices[0], GL_STATIC_DRAW);


	//
	glGenBuffers(1, &m_B_KingVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_B_KingVBO);
	glBufferData(GL_ARRAY_BUFFER, B_KingVertices.size() * sizeof(glm::vec3), &B_KingVertices[0], GL_STATIC_DRAW);

}

void Renderer::DrawParticle() 
{
}


float CameraXRadians = 90.f;		
float CameraYRadians = 0.f;
float TurnChangeRadians = 0.f;


void Renderer::DrawParticle2()					
{

	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);

	//
	glm::mat4 UnitVector(1.f);

	unsigned int UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(UnitVector));

	// ----------------------------------------------------------------------------------------------
	glm::mat4 CameraTraslation(1.f);	

	glm::mat4 CameraTranslateMatrix(1.0f);
	glm::mat4 CameraRotationMatrix(1.0f);
	glm::mat4 CameraTurnChangeMatrix(1.0f);

	CameraRotationMatrix = glm::rotate(UnitVector, glm::radians(CameraXRadians), glm::vec3(1.0f, 0.0f, 0.0f));	
	CameraTranslateMatrix = glm::translate(UnitVector, glm::vec3(-1.62, 0.0, -1.62));

	CameraTurnChangeMatrix = glm::rotate(UnitVector, glm::radians(TurnChangeRadians), glm::vec3(0.0f, 1.0f, 0.0f));

	CameraTraslation = CameraRotationMatrix * CameraTurnChangeMatrix * CameraTranslateMatrix;

	unsigned int RotateLocation = glGetUniformLocation(program, "CameraTransfrom");
	glUniformMatrix4fv(RotateLocation, 1, GL_FALSE, glm::value_ptr(CameraTraslation));



	//--------------------------------------------변환-------------------------------------------------

	glm::mat4 projection(1.0f);

	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); 
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));		

	unsigned int ProjectionLocation = glGetUniformLocation(program, "projectionTransform");
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));


	//
	int posLoc0 = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc0);
	int ColorLoc0 = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(ColorLoc0);

	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);  //5¹øÂ° ÀÎÀÚ: stride. ¹ÙÀÌÆ® ´ÜÀ§
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_LINES, 0, 6);

	
	//----------------------------------------조명----------------------------------------
	//
	unsigned int lightPosLocation = glGetUniformLocation(program, "lightPos");	  
	glUniform3f(lightPosLocation, 5.0, 4.0, 5.0);		
	unsigned int lightColorLocation = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);	
	unsigned int viewPosLocation = glGetUniformLocation(program, "viewPos"); //---
	glUniform3f(viewPosLocation, 0.0, 0.0, 0.0);		



	// -------------------------------------------렌더--------------------------------------------


	glm::mat4 W_ChessboardTranslation(1.f);	

	W_ChessboardTranslation = glm::translate(UnitVector, glm::vec3(0.0, 0.0, 0.0));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_ChessboardTranslation));

	int uvLoc = glGetAttribLocation(program, "a_UV");
	glEnableVertexAttribArray(uvLoc);
	int NormalLoc = glGetAttribLocation(program, "a_Normal");
	glEnableVertexAttribArray(uvLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardUVVBO);
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);  
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardNormalVBO);
	glVertexAttribPointer(NormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); 

	int texUloc = glGetUniformLocation(program, "u_Texture");
	glUniform1f(texUloc, 0);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_ChessboardTexture);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 576);


	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			ChessBoardSquare square = board[x][y];

			if (square.piece != ChessPiece::Empty) {
				// select VBO and texture
				GLuint vbo = GetVBO(square.piece, square.color);
				GLuint texture = (square.color == PieceColor::White) ? m_WhiteTexture : m_BlackTexture;
				int vertexCount = GetVertexCount(square.piece);

				// calc piece location
				float posX = 0.36 * (x+1);
				float posY = 0.36 * (y+1); // Z축이라고 가정

				// transform matrix
				glm::mat4 pieceTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(posY, 0.0f, posX));

				// render
				glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(pieceTranslation));
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			}
		}
	}

}

GLuint Renderer::GetVBO(ChessPiece piece, PieceColor color) {
	switch (piece) {
	case ChessPiece::Pawn:
		return (color == PieceColor::White) ? m_W_Pawn1VBO : m_B_Pawn1VBO;
	case ChessPiece::Rook:
		return (color == PieceColor::White) ? m_W_Rook1VBO : m_B_Rook1VBO;
	case ChessPiece::Bishop:
		return (color == PieceColor::White) ? m_W_Bishop1VBO : m_B_Bishop1VBO;
	case ChessPiece::Knight:
		return (color == PieceColor::White) ? m_W_Knight1VBO : m_B_Knight1VBO;
	case ChessPiece::Queen:
		return (color == PieceColor::White) ? m_W_QueenVBO : m_B_QueenVBO;
	case ChessPiece::King:
		return (color == PieceColor::White) ? m_W_KingVBO : m_B_KingVBO;
	default:
		return -1;
	}
}

int Renderer::GetVertexCount(ChessPiece piece) {
	switch (piece) {
	case ChessPiece::Pawn:
		return 5816;
	case ChessPiece::Rook:
		return 6070;
	case ChessPiece::Bishop:
		return 6776;
	case ChessPiece::Knight:
		return 2438;
	case ChessPiece::Queen:
		return 6838;
	case ChessPiece::King:
		return 5084;
	default:
		return 0;
	}
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	printf("파일 로드 중\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector <glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("파일이 로드되지 않습니다.\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];

		//
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		//
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; 
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("Load Fail");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			//³ª¸ÓÁö ¶óÀÎÀ» ¸Ô´Â´Ù.
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	//°¢ »ï°¢ÇüÀÇ °¢ ²ÀÁöÁ¡
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		//¼Ó¼ºÀÇ ÀÎµ¦½º¸¦ °¡Á®¿Â´Ù
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		//ÀÎµ¦½º¿¡¼­ ¼Ó¼ºÀ» °¡Á®¿Â´Ù
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		//¹öÆÛ¿¡ ¼Ó¼ºÀ» ³Ö´Â´Ù
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;

}