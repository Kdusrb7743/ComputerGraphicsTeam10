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

	void CreateVertexBufferObjects();					// VBO ����

	void DrawParticle();								//�׸���
	void DrawParticle2();								//�׸���

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0310();


	// ���� ����
	bool m_Initialized = false;

	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;
	//
	
	//�Ⱦ�
	GLuint m_testVBO = 0;
	GLuint m_testVBO1 = -1;


	// ���̴� ����
	GLuint m_ParticleShader = -1;

	GLuint m_VertexVBO = -1;		// vertex + color
	GLuint m_PointVBO = -1;			// Point + color
	GLuint m_LineVBO = -1;			// Line + color
	GLuint m_SquareVBO = -1;		// Square + color

	GLuint m_CenterCrossLineVBO = -1;
	GLuint m_FloorVBO = -1;

	// ü�� ��
	GLuint m_ChessboardVBO = -1;
	GLuint m_ChessboardTexture = -1;
	GLuint m_ChessboardUVVBO = -1;
	GLuint m_ChessboardNormalVBO = -1;

	// ü�� ��� ������ �ؽ�ó
	GLuint m_WhiteTexture = -1;
	GLuint m_BlackTexture = -1;

	//�� ü���� �븻
	GLuint m_PawnNormalVBO = -1;
	GLuint m_BishopNormalVBO = -1;
	GLuint m_KnightNormalVBO = -1;
	GLuint m_RookNormalVBO = -1;
	GLuint m_QueenNormalVBO = -1;
	GLuint m_KingNormalVBO = -1;

	// ü�� ȭ��Ʈ ����	+ �븻
	// ��
	GLuint m_W_Pawn1VBO = -1;
	GLuint m_W_Pawn2VBO = -1;
	GLuint m_W_Pawn3VBO = -1;
	GLuint m_W_Pawn4VBO = -1;
	GLuint m_W_Pawn5VBO = -1;
	GLuint m_W_Pawn6VBO = -1;
	GLuint m_W_Pawn7VBO = -1;
	GLuint m_W_Pawn8VBO = -1;
	GLuint m_PawnUVVBO = -1;

	// ���
	GLuint m_W_Bishop1VBO = -1;
	GLuint m_W_Bishop2VBO = -1;

	// ����Ʈ
	GLuint m_W_Knight1VBO = -1;
	GLuint m_W_Knight2VBO = -1;

	// ��
	GLuint m_W_Rook1VBO = -1;
	GLuint m_W_Rook2VBO = -1;

	// ��
	GLuint m_W_QueenVBO = -1;

	// ŷ
	GLuint m_W_KingVBO = -1;


	// ü�� �� ����
	// ��
	GLuint m_B_Pawn1VBO = -1;
	GLuint m_B_Pawn2VBO = -1;
	GLuint m_B_Pawn3VBO = -1;
	GLuint m_B_Pawn4VBO = -1;
	GLuint m_B_Pawn5VBO = -1;
	GLuint m_B_Pawn6VBO = -1;
	GLuint m_B_Pawn7VBO = -1;
	GLuint m_B_Pawn8VBO = -1;

	// ���
	GLuint m_B_Bishop1VBO = -1;
	GLuint m_B_Bishop2VBO = -1;

	// ����Ʈ
	GLuint m_B_Knight1VBO = -1;
	GLuint m_B_Knight2VBO = -1;

	// ��
	GLuint m_B_Rook1VBO = -1;
	GLuint m_B_Rook2VBO = -1;

	// ��
	GLuint m_B_QueenVBO = -1;

	// ŷ
	GLuint m_B_KingVBO = -1;



};

