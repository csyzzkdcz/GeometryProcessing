#include"DenoisyLightShader.h"
#include "Model.h"
#include "vmath.h"
#include "ArcBall.h"
#include <iostream>
using namespace std;
DenoisyLightShader::DenoisyLightShader(Mesh3D *meshFromMesh3D) : BaseShader(meshFromMesh3D)
{

}

DenoisyLightShader::DenoisyLightShader(Model *modelData)
{
	Model::Vertex *pVertexesOfTriangles = &((modelData->vertexesOfTriangles)[0]);
	glGenBuffers(1, &vbo_vert_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);
	glBufferData(GL_ARRAY_BUFFER, (modelData->vertexesOfTriangles.size())*sizeof(Model::Vertex), pVertexesOfTriangles, GL_STATIC_DRAW);
	weight_ = 0.0;
}

DenoisyLightShader::~DenoisyLightShader()
{
}

void DenoisyLightShader::CreateShader()
{
	const GLchar *vertexShaderSrc =
		"#version 430\n"
		"uniform mat4 PMV;\n"
		"uniform float weight;\n"
		"layout(location=0) in vec3 position;\n"
		"layout(location=1) in vec3 delta;\n"
		"layout(location=2) in vec3 normal;\n"
		"layout(location=3) in vec4 color;\n"
		"out vec4 ocolor;\n"
		"out vec3 onormal;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = PMV * vec4(position-weight*delta,1.0);\n"
		"	ocolor = color;\n"
		"	onormal = vec3(inverse(transpose(PMV))*vec4(normal,1.0));\n"
		"}\n";

	const GLchar *fragmentShaderSrc =
		"#version 430\n"
		"uniform vec3 LightDirection;\n"
		"uniform vec3 HalfVector;\n"
		"uniform vec3 Ambient;\n"
		"uniform vec3 LightColor;\n"
		"uniform float Shininess;\n"
		"uniform float Strength;\n"
		"in vec4 ocolor;\n"
		"in vec3 onormal;\n"
		"out vec4 Fragment;\n"
		"void main()\n"
		"{\n"
		"	float diffuse;\n"
		"	diffuse = max(0.0,dot(normalize(onormal),LightDirection));\n"
		"	float specular;\n"
		"   specular = max(0.0,dot(normalize(onormal),normalize(HalfVector)));\n"
		"	if (diffuse == 0)\n"
		"	specular = 0;\n"
		"	else\n"
		"	specular = pow(specular, Shininess);\n"
		"	Fragment = vec4(min(vec3(ocolor)*(Ambient + LightColor * diffuse)+LightColor * specular * Strength,vec3(1.0)) ,1.0);\n"
		"}\n";

	GLuint vertexShader = GenerateAndCompiler(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint fragmentShader = GenerateAndCompiler(GL_FRAGMENT_SHADER, fragmentShaderSrc);

	LinkProgram(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void DenoisyLightShader::DrawModel(Model *modelData, CArcBall *arcBall, double distance)
{

	Matrix4f modelView = Matrix4f::createLookAt(Vector3f(0, 0, distance), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
	Matrix4f proj = Matrix4f::createFrustum(-1, 1, -1, 1, 1, 50);
	float modelTranArray[16];

	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			modelTranArray[i * 4 + j] = arcBall->GetBallMatrix()[i * 4 + j];
		}
	}

	Matrix4f modelTranMatrix(modelTranArray);
	Matrix4f combined = proj*modelView*modelTranMatrix;
	// Set the uniform variable
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "PMV"), 1, GL_FALSE, (float*)&combined);
	glUniform3f(glGetUniformLocation(program, "LightDirection"), 1.0, 0.0, 0.0);
	glUniform3f(glGetUniformLocation(program, "HalfVector"), 1.0, 0.0, -1.0); //The eye position is (0,0,-1);
	glUniform3f(glGetUniformLocation(program, "Ambient"), 0.3, 0.3, 0.3);
	glUniform3f(glGetUniformLocation(program, "LightColor"), 0.9, 0.9, 0.9);
	glUniform1f(glGetUniformLocation(program, "Shininess"), 30.0);
	glUniform1f(glGetUniformLocation(program, "Strength"), 2.0);
	glUniform1f(glGetUniformLocation(program, "weight"), (float)weight_);

	// Set local variable also named as "in" variable

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 8));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 11));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawArrays(GL_TRIANGLES, 0, modelData->vertexesOfTriangles.size());
}