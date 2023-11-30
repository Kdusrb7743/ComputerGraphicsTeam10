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

	// ü�� ȭ��Ʈ ����
	// ��
	GLuint m_W_Pawn1VBO = -1;
	GLuint m_W_Pawn2VBO = -1;
	GLuint m_W_Pawn3VBO = -1;
	GLuint m_W_Pawn4VBO = -1;
	GLuint m_W_Pawn5VBO = -1;
	GLuint m_W_Pawn6VBO = -1;
	GLuint m_W_Pawn7VBO = -1;
	GLuint m_W_Pawn8VBO = -1;

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

  

	GLuint m_CraneColorVBO = -1;	




};
