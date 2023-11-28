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
	std::vector<glm::vec3>&);



Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	//Class0310();			// 여기서 바인드 버퍼 해준다.
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
	//CreateParticleVBO(int numParticleCount = 1000);	//교수님은 여기서 해줌

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
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
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
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
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


float CenterCrossLinePos[] =		//가로세로 깊이
{
	0.0f,  1.0f, 0.0f,	0.0, 1.0, 0.0,		// 세로
	0.0f, -1.0f, 0.0f,	0.0, 1.0, 0.0,
	1.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,		// 가로
   -1.0f,  0.0f, 0.0f,	1.0, 0.0, 0.0,
	0.0f,  0.0f, 1.0f,  0.0, 0.0, 1.0,		// 깊이
	0.0f,  0.0f, -1.0f, 0.0, 0.0, 1.0,

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

// 큰 밑바닥
float Floor[] =
{
	 2.0f, 0.0f,  2.0f,  1.0f, 0.0f, 0.0f,
	 2.0f, 0.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, 0.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	-2.0f, 0.0f,  2.0f,  0.0f, 0.0f, 1.0f,
	 2.0f, 0.0f, -2.0f,  0.0f, 1.0f, 0.0f,
	-2.0f, 0.0f, -2.0f,  1.0f, 0.0f, 0.0f,

};

// 크래인 바텀.
std::vector<glm::vec3> CraneBottomVertices;
std::vector<glm::vec2> CraneBottomUVs;
std::vector<glm::vec3> CraneBottomNormals;

// 크래인 중간 사각형
std::vector<glm::vec3> CraneCenterVertices;
std::vector<glm::vec2> CraneCenterUVs;
std::vector<glm::vec3> CraneCenterNormals;

// 크래인 맨위 포신 왼쪽
std::vector<glm::vec3> CraneUpLeftGunVertices;
std::vector<glm::vec2> CraneUpLeftGunUVs;
std::vector<glm::vec3> CraneUpLeftGunNormals;

// 크래인 맨위 포신 오른쪽
std::vector<glm::vec3> CraneUpRightGunVertices;
std::vector<glm::vec2> CraneUpRightGunUVs;
std::vector<glm::vec3> CraneUpRightGunNormals;

// 크래인 앞 포신 왼쪽
std::vector<glm::vec3> CraneFrontLeftGunVertices;
std::vector<glm::vec2> CraneFrontLeftGunCraneBottomUVs;
std::vector<glm::vec3> CraneFrontLeftGunCraneBottomNormals;

// 크래인 앞 포신 오른쪽
std::vector<glm::vec3> CraneFrontRightGunVertices;
std::vector<glm::vec2> CraneFrontRightGunCraneBottomUVs;
std::vector<glm::vec3> CraneFrontRightGunCraneBottomNormals;

void Renderer::CreateVertexBufferObjects()
{

	//isCube ? Load_Object("CraneBottom3.obj") : Load_Object("CraneBottom3.obj");

	bool res = loadOBJ("CraneBottom.obj", CraneBottomVertices, CraneBottomUVs, CraneBottomNormals);
	bool res2 = loadOBJ("CraneCenter.obj", CraneCenterVertices, CraneCenterUVs, CraneCenterNormals);
	bool res3 = loadOBJ("CraneUpLeftGun.obj", CraneUpLeftGunVertices, CraneUpLeftGunUVs, CraneUpLeftGunNormals);
	bool res4 = loadOBJ("CraneUpRightGun.obj", CraneUpRightGunVertices, CraneUpRightGunUVs, CraneUpRightGunNormals);
	bool res5 = loadOBJ("CraneFrontLeftGun.obj", CraneFrontLeftGunVertices, CraneFrontLeftGunCraneBottomUVs, CraneFrontLeftGunCraneBottomNormals);
	bool res6 = loadOBJ("CraneFrontRightGun.obj", CraneFrontRightGunVertices, CraneFrontRightGunCraneBottomUVs, CraneFrontRightGunCraneBottomNormals);


	cout << endl << res << endl;
	cout << res2 << endl;
	cout << res3 << endl;
	cout << res4 << endl;
	cout << res5 << endl;
	cout << res6 << endl;


	// 가로세로높이 라인
	glGenBuffers(1, &m_CenterCrossLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CenterCrossLinePos), CenterCrossLinePos, GL_STATIC_DRAW);

	// 바닥
	glGenBuffers(1, &m_FloorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FloorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Floor), Floor, GL_STATIC_DRAW);

	// 크래인 바텀
	glGenBuffers(1, &m_CraneBottomVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneBottomVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneBottomVertices.size() * sizeof(glm::vec3), &CraneBottomVertices[0], GL_STATIC_DRAW);

	// 크래인 중간 사각형
	glGenBuffers(1, &m_CraneCenterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneCenterVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneCenterVertices.size() * sizeof(glm::vec3), &CraneCenterVertices[0], GL_STATIC_DRAW);

	// 크래인 맨위 포신 왼쪽
	glGenBuffers(1, &m_CraneUpLeftGunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneUpLeftGunVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneUpLeftGunVertices.size() * sizeof(glm::vec3), &CraneUpLeftGunVertices[0], GL_STATIC_DRAW);
	
	// 크래인 맨위 포신 오른쪽
	glGenBuffers(1, &m_CraneUpRightGunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneUpRightGunVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneUpRightGunVertices.size() * sizeof(glm::vec3), &CraneUpRightGunVertices[0], GL_STATIC_DRAW);
	
	// 크래인 앞 포신 왼쪽
	glGenBuffers(1, &m_CraneFrontLeftGunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneFrontLeftGunVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneFrontLeftGunVertices.size() * sizeof(glm::vec3), &CraneFrontLeftGunVertices[0], GL_STATIC_DRAW);

	// 크래인 앞 포신 오른쪽
	glGenBuffers(1, &m_CraneFrontRightGunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneFrontRightGunVBO);
	glBufferData(GL_ARRAY_BUFFER, CraneFrontRightGunVertices.size() * sizeof(glm::vec3), &CraneFrontRightGunVertices[0], GL_STATIC_DRAW);


	// 모든 삼각형 컬러
	glGenBuffers(1, &m_CraneColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColor), CubeColor, GL_STATIC_DRAW);

}

void Renderer::DrawParticle() //문제 1번
{
}

//전역변수

float CameraXOrbitRadians = 30.f;		// 카메라 공전 회전각
float CameraYOrbitRadians = 0.f;
float CameraYRotatingRadians = 0.f;		// 카메라 자전 회전각
float CameraXMove = 0.0f;				// 카메라 위치 이동
float CameraZMove = 0.0f;

float AllZMove = 0.0f;					// 크래인 z이동

float UpperBodyYRotatingRadians = 0.f;	// 크래인 상체 회전각

float CraneUpLeftGunXRadians = 0.f;		// 크래인 위쪽 포신 x회전각

float CraneFrontLeftGunYRadians = 0.f;	// 크래인 앞 포신 회전각
float CraneFrontLeftGunXMove = 0.f;

void Renderer::DrawParticle2()					// -- 문제 18번 -- 
{

	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);


	// 카메라 변환
	glm::mat4 UnitVector(1.f);

	//모델링 변환 초기화
	unsigned int UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(UnitVector));

	glm::mat4 CameraTraslation(1.f);		// 카메라 최종 변환

	glm::mat4 CameraTranslateMatrix(1.0f);	// 카메라 이동 변환

	glm::mat4 CameraXOrbitRotateMatrix(1.0f);	// 카메라 공전
	glm::mat4 CameraYOrbitRotateMatrix(1.0f);

	glm::mat4 CameraYRotatingMatrix(1.0f);	// 카메라 자전
	glm::mat4 CameraYRotatingTranslateMatrix(1.0f);		// 카메라 자전 시에 먼저 좌표축z을 가운데로 옮김
	glm::mat4 CameraYRotatingTranslateOriMatrix(1.0f);	// 카메라 자전 시에 좌표축z 되돌림
	glm::mat4 CameraYRotatingRotateOriMatrix(1.0f);	// 카메라 자전 시에 좌표축z 되돌림

	CameraXOrbitRotateMatrix = glm::rotate(UnitVector, glm::radians(CameraXOrbitRadians), glm::vec3(1.0f, 0.0f, 0.0f));	//x축으로 
	CameraYOrbitRotateMatrix = glm::rotate(UnitVector, glm::radians(CameraYOrbitRadians), glm::vec3(0.0f, 1.0f, 0.0f));	//y축으로 

	CameraTranslateMatrix = glm::translate(UnitVector, glm::vec3(CameraXMove, 0, CameraZMove));

	CameraYRotatingTranslateMatrix = glm::translate(UnitVector, glm::vec3(0, 0, -3.f));
	CameraYRotatingRotateOriMatrix = glm::rotate(UnitVector, glm::radians(CameraYRotatingRadians), glm::vec3(0.0f, 1.0f, 0.0f));
	CameraYRotatingTranslateOriMatrix = glm::translate(UnitVector, glm::vec3(0, 0, 3.f));

	CameraYRotatingMatrix = CameraYRotatingTranslateOriMatrix * CameraYRotatingRotateOriMatrix * CameraYRotatingTranslateMatrix;

	CameraTraslation = CameraXOrbitRotateMatrix * CameraYOrbitRotateMatrix * CameraTranslateMatrix * CameraYRotatingMatrix;

	unsigned int RotateLocation = glGetUniformLocation(program, "CameraTransfrom");
	glUniformMatrix4fv(RotateLocation, 1, GL_FALSE, glm::value_ptr(CameraTraslation));



	glm::mat4 projection(1.0f);

	//원근투영 변환
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3.0));		//--- 공간을 z축 이동

	unsigned int ProjectionLocation = glGetUniformLocation(program, "projectionTransform");
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	//가로세로 라인
	int posLoc0 = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc0);
	int ColorLoc0 = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(ColorLoc0);

	glBindBuffer(GL_ARRAY_BUFFER, m_CenterCrossLineVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);  //5번째 인자: stride. 바이트 단위
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_LINES, 0, 6);

	//큰 바닥
	glBindBuffer(GL_ARRAY_BUFFER, m_FloorVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);  //5번째 인자: stride. 바이트 단위
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);


	// 크래인 그리는 부분.

	glm::mat4 AllZTranslation(1.f);		// 전체 이동

	AllZTranslation = glm::translate(UnitVector, glm::vec3(0.0, 0.0, AllZMove));

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(AllZTranslation));

	// 크래인 바텀
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneBottomVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneColorVBO);
	glVertexAttribPointer(ColorLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);		// 사각형 : 6 -> 육면체 : 6면


	// 크래인 상체 회전 부
	glm::mat4 CraneCenterTranslation(1.f);			// 먼저 자전한다.

	glm::mat4 UpperBodyYRotationMatrix(1.f);			// 먼저 자전한다.


	UpperBodyYRotationMatrix = glm::rotate(UnitVector, glm::radians(UpperBodyYRotatingRadians), glm::vec3(0.0f, 1.0f, 0.0f));

	CraneCenterTranslation = AllZTranslation * UpperBodyYRotationMatrix;

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(CraneCenterTranslation));

	// 크래인 중간 사각형
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneCenterVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);


	// 크래인 맨위 왼 포신 x축에 대해 공전 회전
	glm::mat4 CraneUpLeftGunTranslation(1.f);	// 맨위 왼 포신최종

	glm::mat4 CraneUpLeftGunYTranslateMatrix(1.f);	// 맨위 왼 포신
	glm::mat4 CraneUpLeftGunXRotateMatrix(1.f);	// 맨위 왼 포신
	glm::mat4 CraneUpLeftGunYTranslateOriginalMatrix(1.f);	// 맨위 왼 포신

	CraneUpLeftGunYTranslateMatrix = glm::translate(UnitVector, glm::vec3(0.f, -0.3f, 0.f));
	CraneUpLeftGunXRotateMatrix = glm::rotate(UnitVector, glm::radians(CraneUpLeftGunXRadians), glm::vec3(1.0f, 0.0f, 0.0f));
	CraneUpLeftGunYTranslateOriginalMatrix = glm::translate(UnitVector, glm::vec3(0.f, 0.3f, 0.f));

	CraneUpLeftGunXRotateMatrix = CraneUpLeftGunYTranslateOriginalMatrix * CraneUpLeftGunXRotateMatrix * CraneUpLeftGunYTranslateMatrix;

	CraneUpLeftGunTranslation = AllZTranslation * UpperBodyYRotationMatrix * CraneUpLeftGunXRotateMatrix;

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(CraneUpLeftGunTranslation));


	// 크래인 맨위 포신 왼쪽
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneUpLeftGunVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);


	// 크래인 맨위 왼 포신 x축에 대해 공전 회전
	glm::mat4 CraneUpRightGunTranslation(1.f);	// 맨위 왼 포신최종

	glm::mat4 CraneUpRightGunYTranslateMatrix(1.f);	// 맨위 왼 포신
	glm::mat4 CraneUpRightGunXRotateMatrix(1.f);	// 맨위 왼 포신
	glm::mat4 CraneUpRightGunYTranslateOriginalMatrix(1.f);	// 맨위 왼 포신

	CraneUpRightGunYTranslateMatrix = glm::translate(UnitVector, glm::vec3(0.f, -0.3f, 0.f));
	CraneUpRightGunXRotateMatrix = glm::rotate(UnitVector, glm::radians(CraneUpLeftGunXRadians), glm::vec3(-1.0f, 0.0f, 0.0f));
	CraneUpRightGunYTranslateOriginalMatrix = glm::translate(UnitVector, glm::vec3(0.f, 0.3f, 0.f));

	CraneUpRightGunXRotateMatrix = CraneUpRightGunYTranslateOriginalMatrix * CraneUpRightGunXRotateMatrix * CraneUpRightGunYTranslateMatrix;

	CraneUpRightGunTranslation = AllZTranslation * UpperBodyYRotationMatrix * CraneUpRightGunXRotateMatrix;

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(CraneUpRightGunTranslation));

	// 크래인 맨위 포신 오른쪽
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneUpRightGunVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);


	// 앞 왼쪽 포신 Y 축 회전
	glm::mat4 CraneFrontLeftGunTranslation(1.f);			// 최종 앞 왼쪽 포신 y축 회전
	glm::mat4 CraneFrontLeftGunTranslateMatrix(1.f);			// 앞 왼쪽 포신 x축 이동
	glm::mat4 CraneFrontLeftGunRotatingMatrix(1.f);			// 먼저 자전한다.

	CraneFrontLeftGunTranslateMatrix = glm::translate(UnitVector, glm::vec3(CraneFrontLeftGunXMove, 0.0f, 0.f));
	CraneFrontLeftGunRotatingMatrix = glm::rotate(UnitVector, glm::radians(CraneFrontLeftGunYRadians), glm::vec3(0.0f, 1.0f, 0.0f));

	CraneFrontLeftGunTranslation = AllZTranslation * CraneFrontLeftGunTranslateMatrix * CraneFrontLeftGunRotatingMatrix;

	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(CraneFrontLeftGunTranslation));

	// 크래인 앞 포신 왼쪽
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneFrontLeftGunVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 9 + 12);		// 사각형 : 6 -> 육면체 : 6면


	// 앞 오른쪽 포신 Y축 반대 회전
	glm::mat4 CraneFrontRightGunTranslation(1.f);		// 최종 앞포신 Y축 회전 애니메이션
	glm::mat4 CraneFrontRightGunTranslateMatrix(1.f);
	glm::mat4 CraneFrontRightGunRotatingMatrix(1.f);

	CraneFrontRightGunTranslateMatrix = glm::translate(UnitVector, glm::vec3(-1 * CraneFrontLeftGunXMove, 0.0f, 0.f));
	CraneFrontRightGunRotatingMatrix = glm::rotate(UnitVector, glm::radians(CraneFrontLeftGunYRadians), glm::vec3(0.0f, -1.0f, 0.0f));

	CraneFrontRightGunTranslation = AllZTranslation * CraneFrontRightGunTranslateMatrix * CraneFrontRightGunRotatingMatrix;
	
	UnitVectorLocation = glGetUniformLocation(program, "modelTransform");
	glUniformMatrix4fv(UnitVectorLocation, 1, GL_FALSE, glm::value_ptr(CraneFrontRightGunTranslation));


	// 크래인 앞 포신 오른쪽
	glBindBuffer(GL_ARRAY_BUFFER, m_CraneFrontRightGunVBO);
	glVertexAttribPointer(posLoc0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);  //5번째 인자: stride. 바이트 단위

	glDrawArrays(GL_TRIANGLES, 0, 6 * 9 + 12);		// 사각형 : 6 -> 육면체 : 6면

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
//		std::cerr << path << "파일 못찾음";
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
	printf("OBJ 파일 로딩중 %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector <glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("파일 경로를 확인하세요!\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];

		//첫번째 라인의 첫번째 단어를 읽는다
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		//else : 라인의 헤더를 parse
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; //우리가 DDS texture만을 이용할 것이므로 V의 좌표를 반대로 바꾸어준다. 만약 TGA or BMP 로더를 사용하면 이 것을 제거해라.
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
				printf("파일을 읽을수없다.");
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
			//나머지 라인을 먹는다.
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	//각 삼각형의 각 꼭지점
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		//속성의 인덱스를 가져온다
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		//인덱스에서 속성을 가져온다
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		//버퍼에 속성을 넣는다
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;

}