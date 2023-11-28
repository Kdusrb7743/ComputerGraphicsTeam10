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

	// 크래인 셰이더
	GLuint m_CraneBottomVBO = -1;	
	GLuint m_CraneCenterVBO = -1;	
	GLuint m_CraneUpLeftGunVBO = -1;
	GLuint m_CraneUpRightGunVBO = -1;
	GLuint m_CraneFrontLeftGunVBO = -1;
	GLuint m_CraneFrontRightGunVBO = -1;

	GLuint m_CraneColorVBO = -1;	




};

