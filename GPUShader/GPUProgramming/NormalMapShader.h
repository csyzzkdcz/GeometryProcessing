#pragma once
#include"BaseShader.h"
#include <qimage.h>
class NormalMapShader : public BaseShader
{
public:
	NormalMapShader(Mesh3D *meshFromMesh3D = NULL);
	NormalMapShader(Model *modelData,QImage texture_);
	~NormalMapShader();

public:
	void SetWeight(float weight);
	void CreateShader();
	void DrawModel(Model *modelData, CArcBall *arcBall, double distance);
	void GenerateTexture(QImage texture_);

private:
	GLuint		vbo_vert_;		//the initial information of the data struct vertex
	GLuint		vbo_textId_;		//the texture
	QImage		text_image_;		//texture image
};