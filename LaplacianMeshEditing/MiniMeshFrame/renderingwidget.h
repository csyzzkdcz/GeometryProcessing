#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QGLWidget>
#include <QEvent>
#include <vector>
#include "HE_mesh/Vec.h"
#include "Laplacian_Editor.h"

using std::vector;
using trimesh::vec;
using trimesh::point;

class MainWindow;
class CArcBall;
class Mesh3D;

struct color
{
	double r;
	double g;
	double b;
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
	void SelectFixedPoint();
	void SelectHandle();
	void Predecomposition();			// predecomposite the matrix for Laplacian mesh editor
	void RestoreMesh();				// restore the mesh
	int	 PickOnePoint(QPoint p,double &err);
	point screenPt_2_GLPt(QPoint screen_pos_); // transfer screen coordinate into the openGL coordinate

	float Distance(int i,int j);
	vector<int> DijkstraPath(int i,int j);

	void CheckDrawPoint(bool bv);
	void CheckDrawEdge(bool bv);
	void CheckDrawFace(bool bv);
	void CheckLight(bool bv);
	void CheckDrawTexture(bool bv);
	void CheckDrawAxes(bool bv);

	// Interavtion
	void CheckLaplacianEditor(bool bv);
	void CheckTranslation(bool bv);
	void CheckRotation(bool bv);
	void CheckOperation(bool bv);

	// Interactive operations
	void Translation(QPoint cur_pos_, QPoint pre_pos_);
	void Rotation(QPoint cur_pos_);

private:
	void DrawAxes(bool bv);
	void DrawPoints(bool);
	void DrawEdge(bool);
	void DrawFace(bool);
	void DrawTexture(bool);
	void DrawXORRect();

	void PickVert();
	void CalculateCenter();			// get the center of the handle points

public:
	MainWindow					*ptr_mainwindow_;
	CArcBall					*ptr_arcball_;
	CArcBall					*ptr_arcball_local_;
	Mesh3D						*ptr_mesh_;

	// Texture
	GLuint						texture_[1];
	bool						is_load_texture_;

	// eye
	GLfloat						eye_distance_;
	point						eye_goal_;
	vec							eye_direction_;
	QPoint						current_position_;
	QPoint						start_position_;

	// Render information
	bool						is_draw_point_;
	bool						is_draw_edge_;
	bool						is_draw_face_;
	bool						is_draw_texture_;
	bool						has_lighting_;
	bool						is_draw_axes_;

	// Interactive
	bool						is_selecting_fixed_;
	bool						is_selecting_handle_;
	bool						is_dragging_;
	bool						is_laplacian_;
	bool						is_translation_;
	bool						is_rotation_;
	bool						is_no_operation_;

	// Local Axis
	Vec3f					center_;
	Vec3f					axis_x_;
	Vec3f					axis_y_;
	Vec3f					axis_z_;

	// Interactive
	vector<int>					current_index_;
	vector<int>					fixed_index_;
	vector<int>					handle_index_;
	vector<vector<int>>			handle_route_;		
	vector<color>				vertex_colors_;
	Laplacian_Editor				*mesh_editor_;

	// vertices recorded
	vector<point>				pre_vertices_;
private:

};

#endif // RENDERINGWIDGET_H
