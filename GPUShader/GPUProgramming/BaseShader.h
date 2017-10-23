#pragma once
#include <gl/glew.h>
#include <iostream>
class Mesh3D;
class Model;
class CArcBall;

class BaseShader
{
public:
	BaseShader(Mesh3D *meshFromMesh3D = NULL);
	virtual ~BaseShader(void);

	virtual void CreateShader() = 0;
	virtual void DrawModel(Model *model,CArcBall *arcBall, double distance) = 0;

	GLuint  GenerateAndCompiler(GLenum type, const GLchar *src);

	GLuint  GetShaderProgram() const{return program;}

	void	LinkProgram(GLuint vertex_shader, GLuint pixel_shader);

	void SetWeight(float weight);

protected:
	GLuint		program;
	float		weight_;
};

