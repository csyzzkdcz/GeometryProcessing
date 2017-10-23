#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QGLWidget>
#include <QEvent>
#include "HE_mesh/Vec.h"
#include "Parameterization.h"
#include "Parameterization_FixedBoundary.h"
#include "Parameterization_FixedCircle.h"
#include "Parameterization_FixedSquare.h"
#include "Parameterization_FC_Uniform.h"
#include "Parameterization_FC_WLS.h"
#include "Parameterization_FC_SP.h"
#include "Parameterization_FS_Uniform.h"
#include "Parameterization_FS_WLS.h"
#include "Parameterization_FS_SP.h"
#include "Parameterization_UnFixedBoundary.h"
#include "Parameterization_UF_ARAP.h"


using trimesh::vec;
using trimesh::point;

class MainWindow;
class CArcBall;
class Mesh3D;

enum BoundaryType
{
	kCircle,
	kRect,
	kNoneType
};

enum BoundaryConstraint
{
	kFixed,
	kUnfixed,
	kNoneConstraint
};

enum FBParametericMethod
{
	kUniform,
	kWLS,
	kSP,
	kNoneFBMethod
};

enum UFBParametericMethod
{
	kARAP,
	kNoneUFBMethod
};

class RenderingWidget : public QGLWidget
{
	Q_OBJECT

public:
	RenderingWidget(QWidget *parent, MainWindow* mainwindow=0);
	~RenderingWidget();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *e);

	// mouse events
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

public:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

signals:
	void meshInfo(int, int, int);
	void operatorInfo(QString);

private:
	void Render();
	void SetLight();

	public slots:
	void SetBackground();
	void ReadMesh();
	void WriteMesh();
	void LoadTexture();
	void RestoreMesh();
	void DoParameterization();

	void CheckDrawPoint(bool bv);
	void CheckDrawEdge(bool bv);
	void CheckDrawFace(bool bv);
	void CheckLight(bool bv);
	void CheckDrawTexture(bool bv);
	void CheckDrawAxes(bool bv);
	void CheckFB(bool bv);
	void CheckUFB(bool bv);

	// Boundary
	void CheckCircle(bool bv);
	void CheckRect(bool bv);

	// Fixed Boundary methods
	void CheckUniform(bool bv);
	void CheckWLS(bool bv);
	void CheckSP(bool bv);

	// Unfixed Boundary methods
	void CheckARAP(bool bv);

private:
	void DrawAxes(bool bv);
	void DrawPoints(bool);
	void DrawEdge(bool);
	void DrawFace(bool);
	void DrawTexture(bool);

public:
	MainWindow					*ptr_mainwindow_;
	CArcBall					*ptr_arcball_;
	Mesh3D						*ptr_mesh_;
	vector<point>				prev_vertices_;

	// Texture
	GLuint						texture_[1];
	bool						is_load_texture_;

	// eye
	GLfloat						eye_distance_;
	point						eye_goal_;
	vec							eye_direction_;
	QPoint						current_position_;

	// Render information
	bool						is_draw_point_;
	bool						is_draw_edge_;
	bool						is_draw_face_;
	bool						is_draw_texture_;
	bool						has_lighting_;
	bool						is_draw_axes_;

private:
	BoundaryType				B_Type_;
	BoundaryConstraint		B_Constraint_;
	FBParametericMethod		P_FB_Method_;
	UFBParametericMethod     P_UFB_Method_;
	Parameterization         *P_Editor_;
};

#endif // RENDERINGWIDGET_H
