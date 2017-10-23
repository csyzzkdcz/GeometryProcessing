#pragma once

#include <gl/glew.h>
#include <QGLWidget>
#include <gl/glut.h>
#include <qimage.h>

class BaseShader;
class CArcBall;
class Model;
class QMouseEvent;

enum ShaderType
{
	kSimple,
	kDeNoise,
	kNormalMap,
};

class RenderingWidget : public QGLWidget
{
public:
	RenderingWidget(QWidget *parent = 0);
	~RenderingWidget(void);

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

	void Open();
	void Save();
	void SetDeNoisyshader();
	void setNormalMapshader();

	void SetWeight(float weight);
	void LoadTexture();

private:
	void DoShadering();

private:
	CArcBall		*arcBall;
	Model		*modelData;
	BaseShader	*shader;
	ShaderType	shader_type_;
	QImage		tex_image_;

	double distance;
};

