#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();

	void CreateVertexBufferObjects();					// VBO 생성

	void DrawParticle();								//그리기
	void DrawParticle2();								//그리기

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0310();


	// 내부 원소
	bool m_Initialized = false;

	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;
	//
	
	//안씀
	GLuint m_testVBO = 0;
	GLuint m_testVBO1 = -1;


	// 셰이더 선언
	GLuint m_ParticleShader = -1;

	GLuint m_VertexVBO = -1;		// vertex + color
	GLuint m_PointVBO = -1;			// Point + color
	GLuint m_LineVBO = -1;			// Line + color
	GLuint m_SquareVBO = -1;		// Square + color

	GLuint m_CenterCrossLineVBO = -1;
	GLuint m_FloorVBO = -1;

	// 체스 판
	GLuint m_ChessboardVBO = -1;
	GLuint m_ChessboardTexture = -1;
	GLuint m_ChessboardUVVBO = -1;
	GLuint m_ChessboardNormalVBO = -1;

	// 체스 흰색 검은색 텍스처
	GLuint m_WhiteTexture = -1;
	GLuint m_BlackTexture = -1;

	//각 체스말 노말
	GLuint m_PawnNormalVBO = -1;
	GLuint m_BishopNormalVBO = -1;
	GLuint m_KnightNormalVBO = -1;
	GLuint m_RookNormalVBO = -1;
	GLuint m_QueenNormalVBO = -1;
	GLuint m_KingNormalVBO = -1;

	// 체스 화이트 진영	+ 노말
	// 폰
	GLuint m_W_Pawn1VBO = -1;
	GLuint m_W_Pawn2VBO = -1;
	GLuint m_W_Pawn3VBO = -1;
	GLuint m_W_Pawn4VBO = -1;
	GLuint m_W_Pawn5VBO = -1;
	GLuint m_W_Pawn6VBO = -1;
	GLuint m_W_Pawn7VBO = -1;
	GLuint m_W_Pawn8VBO = -1;
	GLuint m_PawnUVVBO = -1;

	// 비숍
	GLuint m_W_Bishop1VBO = -1;
	GLuint m_W_Bishop2VBO = -1;

	// 나이트
	GLuint m_W_Knight1VBO = -1;
	GLuint m_W_Knight2VBO = -1;

	// 룩
	GLuint m_W_Rook1VBO = -1;
	GLuint m_W_Rook2VBO = -1;

	// 퀸
	GLuint m_W_QueenVBO = -1;

	// 킹
	GLuint m_W_KingVBO = -1;


	// 체스 블랙 진영
	// 폰
	GLuint m_B_Pawn1VBO = -1;
	GLuint m_B_Pawn2VBO = -1;
	GLuint m_B_Pawn3VBO = -1;
	GLuint m_B_Pawn4VBO = -1;
	GLuint m_B_Pawn5VBO = -1;
	GLuint m_B_Pawn6VBO = -1;
	GLuint m_B_Pawn7VBO = -1;
	GLuint m_B_Pawn8VBO = -1;

	// 비숍
	GLuint m_B_Bishop1VBO = -1;
	GLuint m_B_Bishop2VBO = -1;

	// 나이트
	GLuint m_B_Knight1VBO = -1;
	GLuint m_B_Knight2VBO = -1;

	// 룩
	GLuint m_B_Rook1VBO = -1;
	GLuint m_B_Rook2VBO = -1;

	// 퀸
	GLuint m_B_QueenVBO = -1;

	// 킹
	GLuint m_B_KingVBO = -1;



};

