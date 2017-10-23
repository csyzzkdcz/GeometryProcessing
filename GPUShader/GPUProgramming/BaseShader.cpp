#include "BaseShader.h"
#include "Mesh3D.h"
#include "Model.h"

BaseShader::BaseShader(Mesh3D *meshFromMesh3D) :program(0)
{
}

BaseShader::~BaseShader(void)
{
}

void BaseShader::SetWeight(float weight)
{
	weight_ = weight;
}

GLuint BaseShader::GenerateAndCompiler(GLenum type, const GLchar* src)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint r;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
		if (msg[0]) 
			return 0;
	}
	
	return shader;
}

void BaseShader::LinkProgram(GLuint vertexShader, GLuint pixelShader)
{
	GLuint vShader = vertexShader;
	GLuint fShader = pixelShader;

	program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	glLinkProgram(program);
	GLint r;
	glGetProgramiv(program, GL_LINK_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetProgramInfoLog(program, sizeof(msg), 0, msg);
	}

	glDetachShader(program, vShader);
	glDetachShader(program, fShader);
}