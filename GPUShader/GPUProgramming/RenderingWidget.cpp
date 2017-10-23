#include "RenderingWidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>
#include "Model.h"
#include "ArcBall.h"
#include "vmath.h"
#include "SimpleDerivedShader.h"
#include "DenoisyLightShader.h"
#include "NormalMapShader.h"
#include <QMouseEvent>
#define DELETE_PTR(ptr) \
	if (ptr != NULL)\
	{\
		delete ptr;\
		ptr = NULL;\
	}\


RenderingWidget::RenderingWidget(QWidget *parent) : QGLWidget(parent),distance(2.0)
{
	arcBall = new CArcBall(width(),height());
	modelData = new Model();
	shader = NULL;
}


RenderingWidget::~RenderingWidget(void)
{
	DELETE_PTR(arcBall);
	DELETE_PTR(modelData);
	DELETE_PTR(shader);
	shader = NULL;
}

void RenderingWidget::initializeGL()
{
	glewInit();
	glClearColor(0.3,0.3,0.3,0.0);
	glEnable(GL_DEPTH_TEST);

	glLineWidth(2.3);

// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_BACK);

}

void RenderingWidget::resizeGL(int w, int h)
{
	int m = w;
	if (h < w)
		m = h;
	glViewport(0,0,m,m);
	arcBall->reSetBound(w, h);
}

void RenderingWidget::paintGL()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (modelData->vertexesOfTriangles.empty())
	{
		return;
	}
	if (shader)
		shader->DrawModel(modelData,arcBall,distance);

}

void RenderingWidget::Open()
{
	QString fileName = QFileDialog::getOpenFileName(this,"Open",".","Obj Files(*.obj)",NULL);

	if (fileName.isEmpty())
	{
		return;
	}

	QTextCodec *code = QTextCodec::codecForName("gd18030");
	QTextCodec::setCodecForLocale(code);

	QByteArray byfilename = fileName.toLocal8Bit();
	modelData->sourceData->LoadFromOBJFileWithNormal(byfilename.data());

	modelData->GenerateVertexesFromSource();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DELETE_PTR(shader);
	shader_type_ = kSimple;
	DoShadering();
}

void RenderingWidget::Save()
{
	if (modelData->sourceData->num_of_vertex_list() == 0)
	{
		emit(QString("The Mesh is Empty !"));
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Write Mesh"), "..", tr("Meshes (*.obj)"));

	if (filename.isEmpty())
		return;

	modelData->sourceData->WriteToOBJFileWithNormal(filename.toLatin1().data());
}

void RenderingWidget::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		arcBall->MouseDown(e->pos());
		break;
	default:
		break;
	}

	updateGL();
}

void RenderingWidget::mouseMoveEvent(QMouseEvent *e)
{
	switch (e->buttons())
	{
		setCursor(Qt::ClosedHandCursor);
	case Qt::LeftButton:
		arcBall->MouseMove(e->pos());
		break;
	default:
		break;
	}

	updateGL();
	//shader->DrawModel(modelData, arcBall, distance);
}

void RenderingWidget::mouseReleaseEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		arcBall->MouseUp(e->pos());
		setCursor(Qt::ArrowCursor);
		break;
	default:
		break;
	}

}

void RenderingWidget::wheelEvent(QWheelEvent *e)
{
	distance += e->delta()*0.001;
	distance = distance < 0 ? 0 : distance;

	updateGL();

}

void RenderingWidget::SetWeight(float weight)
{
	if (!shader)
		return;
	shader->SetWeight(weight);
	updateGL();
}

void RenderingWidget::SetDeNoisyshader()
{
	shader_type_ = kDeNoise;
	DoShadering();
}

void RenderingWidget::setNormalMapshader()
{
	shader_type_ = kNormalMap;
	DoShadering();
}

void RenderingWidget::DoShadering()
{
	if (!modelData->vertexesOfTriangles.size())
		return;
	switch (shader_type_)
	{
	case kSimple:
		shader = new SimpleDerivedShader(modelData);
		shader->CreateShader();
		break;
	case kDeNoise:
		shader = new DenoisyLightShader(modelData);
		shader->CreateShader();
		break;
	case kNormalMap:
		LoadTexture();
		shader = new NormalMapShader(modelData,tex_image_);
		shader->CreateShader();
		break;
	default:
		break;
	}
	updateGL();
}

void RenderingWidget::LoadTexture()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	QImage buf;										///////////////////////////////////////
	if (!buf.load(fileName))								//读取一张路径为filename的图片，
	{													//并转换为OpenGL可以识别的格式，
		return;											//保存在tex1中。
	}													//
	tex_image_ = QGLWidget::convertToGLFormat(buf);	////////////////////////////////////
}