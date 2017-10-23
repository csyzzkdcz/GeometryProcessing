#pragma once
#include "baseshader.h"

class SimpleDerivedShader : public BaseShader
{
public:
	SimpleDerivedShader(Mesh3D *meshFromMesh3D = NULL);
	SimpleDerivedShader(Model *modelData);
	~SimpleDerivedShader(void);

	void CreateShader();
	

	void DrawModel(Model *model, CArcBall *arcBall, double distance);
private:
	GLuint vbbo;

};

