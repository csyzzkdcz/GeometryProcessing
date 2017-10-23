#include"NormalMapShader.h"
#include "Model.h"
#include "vmath.h"
#include "ArcBall.h"
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
using namespace std;
NormalMapShader::NormalMapShader(Mesh3D *meshFromMesh3D) : BaseShader(meshFromMesh3D)
{

}

NormalMapShader::NormalMapShader(Model *modelData, QImage texture_)
{
	Model::Vertex *pVertexesOfTriangles = &((modelData->vertexesOfTriangles)[0]);
	glGenBuffers(1, &vbo_vert_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);
	glBufferData(GL_ARRAY_BUFFER, (modelData->vertexesOfTriangles.size())*sizeof(Model::Vertex), pVertexesOfTriangles, GL_STATIC_DRAW);
	weight_ = 0.0;
	GenerateTexture(texture_);
}

NormalMapShader::~NormalMapShader()
{
}

void NormalMapShader::CreateShader()
{
	const GLchar *vertexShaderSrc =
		"#version 430\n"
		"uniform mat4 PMV;\n"
		"uniform float weight;\n"
		"layout(location=0) in vec3 position;\n"
		"layout(location=1) in vec3 delta;\n"
		"layout(location=2) in vec2 texCoord;\n"
		"layout(location=3) in vec4 color;\n"
		"out vec4 ocolor;\n"
		"out vec4 onormal;\n"
		"out vec2 otexCoord;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = PMV * vec4(position-weight*delta,1.0);\n"
		"	ocolor = color;\n"
		"   otexCoord = texCoord;\n"
		"}\n";

	const GLchar *fragmentShaderSrc =
		"#version 430\n"
		"uniform sampler2D Texture;\n"	//声明一个名为Texture类型为sampler2D的变量。同vec3、mat4类似，sampler2D是OpenGL保存纹理数据的内置类型，必须声明为uniform。我们需要把tex1的数据传入Texture中。
		"uniform vec3 LightDirection;\n"
		"uniform vec3 Ambient;\n"
		"uniform vec3 LightColor;\n"
		"in vec4 ocolor;\n"
		"in vec2 otexCoord;\n"
		"out vec4 Fragment;\n"
		"void main()\n"
		"{\n"
		"   vec4 normal = texture2D(Texture, otexCoord);\n"
		"	float diffuse;\n"
		"	diffuse = max(0.0,dot(normalize(vec3(normal)),LightDirection));\n"
		"	Fragment = vec4(min(vec3(ocolor)*(Ambient + LightColor * diffuse),vec3(1.0)) ,1.0);\n"
		"}\n";

	GLuint vertexShader = GenerateAndCompiler(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint fragmentShader = GenerateAndCompiler(GL_FRAGMENT_SHADER, fragmentShaderSrc);

	LinkProgram(vertexShader, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void NormalMapShader::DrawModel(Model *modelData, CArcBall *arcBall, double distance)
{

	Matrix4f modelView = Matrix4f::createLookAt(Vector3f(0, 0, distance), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
	//cout << modelView << endl;
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

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "PMV"), 1, GL_FALSE, (float*)&combined);
	glUniform3f(glGetUniformLocation(program, "LightDirection"), 1.0, 0.0, 0.0);
	glUniform3f(glGetUniformLocation(program, "Ambient"), 0.3, 0.3, 0.3);
	glUniform3f(glGetUniformLocation(program, "LightColor"), 0.9, 0.9, 0.9);
	glUniform1f(glGetUniformLocation(program, "weight"), (float)weight_);

	//将着色器中Texture变量的编号设为0。
	glUniform1i(glGetUniformLocation(program, "Texture"), 0);
	//设置0号sampler2D变量（也就是Texture）为活跃，允许传入数据。
	glActiveTexture(GL_TEXTURE0);
	//将vbo_textId_绑定的数据传入编号为0的sampler2D变量，也就是将text_image_传入Texture中。
	glBindTexture(GL_TEXTURE_2D, vbo_textId_);
	//glBindTexture(GL_TEXTURE_2D, vbo_textId_);


	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 6));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (void*)(sizeof(float)* 11));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawArrays(GL_TRIANGLES, 0, modelData->vertexesOfTriangles.size());
}

void NormalMapShader::GenerateTexture(QImage texture_)
{
	text_image_ = texture_;
	glGenTextures(1, &vbo_textId_);	//申请1个编号用于与保存纹理的内存绑定，该编号保存在vbo_textId_中。
	glBindTexture(GL_TEXTURE_2D, vbo_textId_);		//将vbo_textId_与GL_TEXTURE_2D绑定。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//将设置texture_的解析方式，并将该纹理数据同vbo_textId_绑定。至此，我们可以通过texId找到纹理数据texture_。
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_image_.width(), text_image_.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text_image_.bits());

}