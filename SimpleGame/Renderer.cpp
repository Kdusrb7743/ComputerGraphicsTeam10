#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Renderer.h"
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



Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	//Class0310();			// ���⼭ ���ε� ���� ���ش�.
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
	//CreateParticleVBO(int numParticleCount = 1000);	//�������� ���⼭ ����

	/*
	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
	*/

}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
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
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
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


float CenterCrossLinePos[] =		//���μ��� ����
{
	0.0f,  5.0f, 0.0f,	0.0, 1.0, 0.0,		// ����
	0.0f, -5.0f, 0.0f,	0.0, 1.0, 0.0,
	5.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,		// ����
   -5.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,
	0.0f,  0.0f, 5.0f,  0.0, 0.0, 1.0,		// ����
	0.0f,  0.0f, -5.0f, 0.0, 0.0, 1.0,

};

float CubeColor[] = {
	   0.5f, 0.0f, 0.5f,//4
	   0.0f, 0.0f, 1.0f,//0
	   0.0f, 0.0f, 0.0f,//3

	   0.5f, 0.0f, 0.5f,//4
	   0.0f, 0.0f, 0.0f,//3
	   1.0f, 0.0f, 0.0f,//7

	   0.0f, 1.0f, 0.0f,//2
	   0.5f, 0.5f, 0.0f,//6
	   1.0f, 0.0f, 0.0f,//7

	   0.0f, 1.0f, 0.0f,//2
	   1.0f, 0.0f, 0.0f,//7
	   0.0f, 0.0f, 0.0f,//3

	   0.0f, 0.5f, 0.5f,//1
	   1.0f, 1.0f, 1.0f,//5
	   0.0f, 1.0f, 0.0f,//2

	   1.0f, 1.0f, 1.0f,//5
	   0.5f, 0.5f, 0.0f,//6
	   0.0f, 1.0f, 0.0f,//2

	   0.0f, 0.0f, 1.0f,//0
	   0.5f, 0.0f, 0.5f,//4
	   0.0f, 0.5f, 0.5f,//1

	   0.5f, 0.0f, 0.5f,//4
	   1.0f, 1.0f, 1.0f,//5
	   0.0f, 0.5f, 0.5f,//1

	   0.5f, 0.0f, 0.5f,//4
	   1.0f, 0.0f, 0.0f,//7
	   1.0f, 1.0f, 1.0f,//5

	   1.0f, 0.0f, 0.0f,//7
	   0.5f, 0.5f, 0.0f,//6
	   1.0f, 1.0f, 1.0f,//5

	   0.0f, 0.0f, 1.0f,//0
	   0.0f, 0.5f, 0.5f,//1
	   0.0f, 1.0f, 0.0f,//2

	   0.0f, 0.0f, 1.0f,//0
	   0.0f, 1.0f, 0.0f,//2
	   0.0f, 0.0f, 0.0f,//3

	   0.0f, 0.0f, 0.0f,
	   0.0f, 0.0f, 0.0f,
	   0.0f, 0.0f, 0.0f,
	   0.0f, 0.0f, 0.0f
};

// ū �عٴ�
float Floor[] =
{
	 2.0f, 0.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f, 0.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, 0.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, 0.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, 0.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, 0.0f, -2.0f,  1.0f, 0.0f, 0.0f,

};

// ü����
std::vector<glm::vec3> ChessboardVertices;
std::vector<glm::vec2> ChessboardUVs;
std::vector<glm::vec3> ChessboardUVsNormals;

// ȭ��Ʈ ��
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

// ȭ��Ʈ ���
std::vector<glm::vec3> W_Bishop1Vertices;
std::vector<glm::vec2> W_Bishop1UVs;
std::vector<glm::vec3> W_Bishop1UVsNormals;

std::vector<glm::vec3> W_Bishop2Vertices;
std::vector<glm::vec2> W_Bishop2UVs;
std::vector<glm::vec3> W_Bishop2UVsNormals;

// ȭ��Ʈ ����Ʈ
std::vector<glm::vec3> W_Knight1Vertices;
std::vector<glm::vec2> W_Knight1UVs;
std::vector<glm::vec3> W_Knight1UVsNormals;

std::vector<glm::vec3> W_Knight2Vertices;
std::vector<glm::vec2> W_Knight2UVs;
std::vector<glm::vec3> W_Knight2UVsNormals;

// ȭ��Ʈ ��
std::vector<glm::vec3> W_Rook1Vertices;
std::vector<glm::vec2> W_Rook1UVs;
std::vector<glm::vec3> W_Rook1UVsNormals;

std::vector<glm::vec3> W_Rook2Vertices;
std::vector<glm::vec2> W_Rook2UVs;
std::vector<glm::vec3> W_Rook2UVsNormals;

// ȭ��Ʈ ��
std::vector<glm::vec3> W_QueenVertices;
std::vector<glm::vec2> W_QueenUVs;
std::vector<glm::vec3> W_QueenUVsNormals;

// ȭ��Ʈ ŷ
std::vector<glm::vec3> W_KingVertices;
std::vector<glm::vec2> W_KingUVs;
std::vector<glm::vec3> W_KingUVsNormals;

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



	// ���μ��γ��� ����
	glGenBuffers(1, &m_CenterCrossLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CenterCrossLinePos), CenterCrossLinePos, GL_STATIC_DRAW);

	// �ٴ�
	glGenBuffers(1, &m_FloorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FloorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Floor), Floor, GL_STATIC_DRAW);

	// ü����
	glGenBuffers(1, &m_ChessboardVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardVBO);
	glBufferData(GL_ARRAY_BUFFER, ChessboardVertices.size() * sizeof(glm::vec3), &ChessboardVertices[0], GL_STATIC_DRAW);

	// ȭ��Ʈ ����
	// ȭ��Ʈ ��
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

	// ȭ��Ʈ ���
	glGenBuffers(1, &m_W_Bishop1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Bishop1Vertices.size() * sizeof(glm::vec3), &W_Bishop1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Bishop2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Bishop2Vertices.size() * sizeof(glm::vec3), &W_Bishop2Vertices[0], GL_STATIC_DRAW);

	// ȭ��Ʈ ����Ʈ
	glGenBuffers(1, &m_W_Knight1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Knight1Vertices.size() * sizeof(glm::vec3), &W_Knight1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Knight2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Knight2Vertices.size() * sizeof(glm::vec3), &W_Knight2Vertices[0], GL_STATIC_DRAW);

	// ȭ��Ʈ ��
	glGenBuffers(1, &m_W_Rook1VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook1VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Rook1Vertices.size() * sizeof(glm::vec3), &W_Rook1Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_W_Rook2VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook2VBO);
	glBufferData(GL_ARRAY_BUFFER, W_Rook2Vertices.size() * sizeof(glm::vec3), &W_Rook2Vertices[0], GL_STATIC_DRAW);

	// ȭ��Ʈ ��
	glGenBuffers(1, &m_W_QueenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_QueenVBO);
	glBufferData(GL_ARRAY_BUFFER, W_QueenVertices.size() * sizeof(glm::vec3), &W_QueenVertices[0], GL_STATIC_DRAW);

	// ȭ��Ʈ ŷ
	glGenBuffers(1, &m_W_KingVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_W_KingVBO);
	glBufferData(GL_ARRAY_BUFFER, W_KingVertices.size() * sizeof(glm::vec3), &W_KingVertices[0], GL_STATIC_DRAW);


	// ��� �ﰢ�� �÷�
	glGenBuffers(1, &m_CraneColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColor), CubeColor, GL_STATIC_DRAW);

}

void Renderer::DrawParticle() //���� 1��
{
}

//��������

float CameraXOrbitRadians = 30.f;		// ī�޶� ���� ȸ����
float CameraYOrbitRadians = 0.f;
float CameraYRotatingRadians = 0.f;		// ī�޶� ���� ȸ����
float CameraXMove = 0.0f;				// ī�޶� ��ġ �̵�
float CameraZMove = 0.0f;


void Renderer::DrawParticle2()					
{

	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);


	// ī�޶� ��ȯ
	glm::mat4 UnitVector(1.f);

	//�𵨸� ��ȯ �ʱ�ȭ
	unsigned int UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(UnitVector));

	glm::mat4 CameraTraslation(1.f);		// ī�޶� ���� ��ȯ

	glm::mat4 CameraTranslateMatrix(1.0f);	// ī�޶� �̵� ��ȯ

	glm::mat4 CameraXOrbitRotateMatrix(1.0f);	// ī�޶� ����
	glm::mat4 CameraYOrbitRotateMatrix(1.0f);

	glm::mat4 CameraYRotatingMatrix(1.0f);	// ī�޶� ����
	glm::mat4 CameraYRotatingTranslateMatrix(1.0f);		// ī�޶� ���� �ÿ� ���� ��ǥ��z�� ����� �ű�
	glm::mat4 CameraYRotatingTranslateOriMatrix(1.0f);	// ī�޶� ���� �ÿ� ��ǥ��z �ǵ���
	glm::mat4 CameraYRotatingRotateOriMatrix(1.0f);	// ī�޶� ���� �ÿ� ��ǥ��z �ǵ���

	CameraXOrbitRotateMatrix = glm::rotate(UnitVector, glm::radians(CameraXOrbitRadians), glm::vec3(1.0f, 0.0f, 0.0f));	//x������ 
	CameraYOrbitRotateMatrix = glm::rotate(UnitVector, glm::radians(CameraYOrbitRadians), glm::vec3(0.0f, 1.0f, 0.0f));	//y������ 

	CameraTranslateMatrix = glm::translate(UnitVector, glm::vec3(CameraXMove, 0, CameraZMove));

	CameraYRotatingTranslateMatrix = glm::translate(UnitVector, glm::vec3(0, 0, -3.f));
	CameraYRotatingRotateOriMatrix = glm::rotate(UnitVector, glm::radians(CameraYRotatingRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	CameraYRotatingTranslateOriMatrix = glm::translate(UnitVector, glm::vec3(0, 0, 3.f));

	CameraYRotatingMatrix = CameraYRotatingTranslateOriMatrix * CameraYRotatingRotateOriMatrix * CameraYRotatingTranslateMatrix;

	CameraTraslation = CameraXOrbitRotateMatrix * CameraYOrbitRotateMatrix * CameraTranslateMatrix * CameraYRotatingMatrix;

	unsigned int RotateLocation = glGetUniformLocation(program, "CameraTransfrom");
	glUniformMatrix4fv(RotateLocation, 1, GL_FALSE, glm::value_ptr(CameraTraslation));



	glm::mat4 projection(1.0f);

	//�������� ��ȯ
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- ���� ���� ����: fovy, aspect, near, far
	projection = glm::translate(projection, glm::vec3(-1.5, 0.0, -8.0));		//--- ������ z�� �̵�

	unsigned int ProjectionLocation = glGetUniformLocation(program, "projectionTransform");
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	//���μ��� ����
	int posLoc0 = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc0);
	int ColorLoc0 = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(ColorLoc0);

	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);  //5��° ����: stride. ����Ʈ ����
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_LINES, 0, 6);


	// ----------------------------------------------������Ʈ ��ġ��ȯ-----------------------------------------

	// ü����
	glm::mat4 W_ChessboardTranslation(1.f);		// ü���� �𵨸� ��ȯ

	W_ChessboardTranslation = glm::translate(UnitVector, glm::vec3(0.0, 0.0, 0.0));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_ChessboardTranslation));

	glBindBuffer(GL_ARRAY_BUFFER, m_ChessboardVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 576);


	// ȭ��Ʈ ��1	�ﰢ�� 6070��
	glm::mat4 W_Rook1Translation(1.f);		// �𵨸� ��ȯ

	W_Rook1Translation = glm::translate(UnitVector, glm::vec3(0.36 * 1, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Rook1Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook1VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6070);


	// ȭ��Ʈ ����Ʈ1
	glm::mat4 W_Knight1Translation(1.f);		// �𵨸� ��ȯ

	W_Knight1Translation = glm::translate(UnitVector, glm::vec3(0.36 * 2, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Knight1Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight1VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 2438);


	// ȭ��Ʈ ���1
	glm::mat4 W_Bishop1Translation(1.f);		// �𵨸� ��ȯ

	W_Bishop1Translation = glm::translate(UnitVector, glm::vec3(0.36 * 3, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Bishop1Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop1VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6776);


	// ȭ��Ʈ ��	�ﰢ�� 6838
	glm::mat4 W_QueenTranslation(1.f);		// �𵨸� ��ȯ

	W_QueenTranslation = glm::translate(UnitVector, glm::vec3(0.36 * 4, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_QueenTranslation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_QueenVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6838);


	// ȭ��Ʈ ŷ	�ﰢ�� 5084
	glm::mat4 W_KingTranslation(1.f);		// �𵨸� ��ȯ

	W_KingTranslation = glm::translate(UnitVector, glm::vec3(0.36 * 5, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_KingTranslation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_KingVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5084);


	// ȭ��Ʈ ���2
	glm::mat4 W_Bishop2Translation(1.f);		// �𵨸� ��ȯ

	W_Bishop2Translation = glm::translate(UnitVector, glm::vec3(0.36 * 6, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Bishop2Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Bishop2VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6776);


	// ȭ��Ʈ ����Ʈ2
	glm::mat4 W_Knight2Translation(1.f);		// �𵨸� ��ȯ

	W_Knight2Translation = glm::translate(UnitVector, glm::vec3(0.36 * 7, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Knight2Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Knight2VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 2438);


	// ȭ��Ʈ ��2	�ﰢ�� 6070��
	glm::mat4 W_Rook2Translation(1.f);		// �𵨸� ��ȯ

	W_Rook2Translation = glm::translate(UnitVector, glm::vec3(0.36 * 8, 0.0, 0.36 * 1));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Rook2Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Rook2VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6070);


	// ȭ��Ʈ ��1
	glm::mat4 W_Pawn1Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn1Translation = glm::translate(UnitVector, glm::vec3(0.36 * 1, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn1Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn1VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��2
	glm::mat4 W_Pawn2Translation(1.f);		// ȭ��Ʈ ����Ʈ1 �𵨸� ��ȯ

	W_Pawn2Translation = glm::translate(UnitVector, glm::vec3(0.36 * 2, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn2Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn2VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��3
	glm::mat4 W_Pawn3Translation(1.f);		// ȭ��Ʈ ����Ʈ1 �𵨸� ��ȯ

	W_Pawn3Translation = glm::translate(UnitVector, glm::vec3(0.36 * 3, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn3Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn3VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����



	// ȭ��Ʈ ��4
	glm::mat4 W_Pawn4Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn4Translation = glm::translate(UnitVector, glm::vec3(0.36 * 4, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn4Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn4VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��5
	glm::mat4 W_Pawn5Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn5Translation = glm::translate(UnitVector, glm::vec3(0.36 * 5, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn5Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn5VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��6
	glm::mat4 W_Pawn6Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn6Translation = glm::translate(UnitVector, glm::vec3(0.36 * 6, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn6Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn6VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��7
	glm::mat4 W_Pawn7Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn7Translation = glm::translate(UnitVector, glm::vec3(0.36 * 7, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn7Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn7VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����


	// ȭ��Ʈ ��8
	glm::mat4 W_Pawn8Translation(1.f);		// ȭ��Ʈ ��1 �𵨸� ��ȯ

	W_Pawn8Translation = glm::translate(UnitVector, glm::vec3(0.36 * 8, 0.0, 0.36 * 2));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(W_Pawn8Translation));

	glBindBuffer(GL_ARRAY_BUFFER, m_W_Pawn8VBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5��° ����: stride. ����Ʈ ����

	glDrawArrays(GL_TRIANGLES, 0, 3 * 5816);		// �簢��-6���� ��. ���� ������ ����






}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::Class0310()
{

}

//bool Load_Object(const char* path) {
//	vertexIndices.clear();
//	uvIndices.clear();
//	normalIndices.clear();
//	vertices.clear();
//	uvs.clear();
//	normals.clear();
//
//	std::ifstream in(path);
//	if (!in) {
//		std::cerr << path << "���� ��ã��";
//		exit(1);
//	}
//
//	//vector<char> lineHeader(istream_iterator<char>{in}, {});
//
//	while (in) {
//		string lineHeader;
//		in >> lineHeader;
//		if (lineHeader == "v") {
//			glm::vec3 vertex;
//			in >> vertex.x >> vertex.y >> vertex.z;
//			vertices.push_back(vertex);
//		}
//		else if (lineHeader == "vt") {
//			glm::vec2 uv;
//			in >> uv.x >> uv.y;
//			uvs.push_back(uv);
//		}
//		else if (lineHeader == "vn") {
//			glm::vec3 normal;
//			in >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		}
//		else if (lineHeader == "f") {
//			char a;
//			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//
//			for (int i = 0; i < 3; i++)
//			{
//				in >> vertexIndex[i] >> a >> uvIndex[i] >> a >> normalIndex[i];
//				vertexIndices.push_back(vertexIndex[i] - 1);
//				uvIndices.push_back(uvIndex[i] - 1);
//				normalIndices.push_back(normalIndex[i] - 1);
//			}
//		}
//	}
//
//	return true;
//}

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	printf("OBJ ���� �ε��� %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector <glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("���� ��θ� Ȯ���ϼ���!\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];

		//ù��° ������ ù��° �ܾ �д´�
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		//else : ������ ����� parse
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; //�츮�� DDS texture���� �̿��� ���̹Ƿ� V�� ��ǥ�� �ݴ�� �ٲپ��ش�. ���� TGA or BMP �δ��� ����ϸ� �� ���� �����ض�.
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
				printf("������ ����������.");
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
			//������ ������ �Դ´�.
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	//�� �ﰢ���� �� ������
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		//�Ӽ��� �ε����� �����´�
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		//�ε������� �Ӽ��� �����´�
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		//���ۿ� �Ӽ��� �ִ´�
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;

}