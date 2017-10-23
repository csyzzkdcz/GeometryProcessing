#pragma once
#include"BaseShader.h"

class DenoisyLightShader : public BaseShader
{
public:
	DenoisyLightShader(Mesh3D *meshFromMesh3D = NULL);
	DenoisyLightShader(Model *modelData);
	~DenoisyLightShader();

public:
	void SetWeight(float weight);
	void CreateShader();
	void DrawModel(Model *modelData, CArcBall *arcBall, double distance);

private:
	float*		lap_coord_; 
	GLuint		vbo_vert_;		//the initial information of the data struct vertex
};