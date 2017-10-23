#include "renderingwidget.h"
#include <QKeyEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <iostream>
#include <QtWidgets/QMenu>
#include <qlineedit.h>
#include <QTextCodec>
#include <gl/GLU.h>
#include <glut/glut.h>
#include <algorithm>
#include <utility>
#include "mainwindow.h"
#include "ArcBall.h"
#include "globalFunctions.h"
#include "HE_mesh/Mesh3D.h"

using std::pair;

#define MAX_DISTANCE 10e10

bool inline igtPointInTriangle2D(Vec3f p, Vec3f pt[3])
{
	//can have a reference in BuildParent program
	Vec3f v1 = pt[0] - p;
	Vec3f v2 = pt[1] - p;
	Vec3f v3 = pt[2] - p;

	//Clear the value on z
	v1[2]=0;
	v2[2]=0;
	v3[2]=0;

	//the direction of the triangle is anti-clockwise
	float fS1 = v1.cross(v2)[2];
	float fS2 = v2.cross(v3)[2];
	float fS3 = v3.cross(v1)[2];

	if (( fS1*fS2 > 0 )&& ( fS3*fS2> 0 ))
		return true;
	else
		return false;
};


RenderingWidget::RenderingWidget(QWidget *parent, MainWindow* mainwindow)
: QGLWidget(parent), ptr_mainwindow_(mainwindow), eye_distance_(5.0),
has_lighting_(false), is_draw_point_(true), is_draw_edge_(false), is_draw_face_(false), is_draw_texture_(false), is_selecting_fixed_(false), is_selecting_handle_(false), is_dragging_(false)
{
	ptr_arcball_ = new CArcBall(width(), height());
	ptr_arcball_local_ = new CArcBall(width(), height());
	ptr_mesh_ = new Mesh3D();

	is_load_texture_ = false;
	is_draw_axes_ = false;

	is_laplacian_ = false;
	is_translation_ = false;
	is_rotation_ = false;

	// initialize the axis
	center_[0] = center_[1] = center_[2] = 0.0;
	axis_x_[0] = 0.7;
	axis_x_[1] = axis_x_[2] = 0.0;
	axis_y_[1] = 0.7;
	axis_y_[0] = axis_y_[2] = 0.0;
	axis_z_[2] = 0.7;
	axis_z_[0] = axis_z_[1] = 0.0;

	eye_goal_[0] = eye_goal_[1] = eye_goal_[2] = 0.0;
	eye_direction_[0] = eye_direction_[1] = 0.0;
	eye_direction_[2] = 1.0;

	current_index_.clear();
	fixed_index_.clear();
	handle_index_.clear();

	mesh_editor_ = new Laplacian_Editor();
}

RenderingWidget::~RenderingWidget()
{
	SafeDelete(ptr_arcball_);
	SafeDelete(ptr_mesh_);
	SafeDelete(ptr_arcball_local_);
	if (mesh_editor_)
		delete(mesh_editor_);
	mesh_editor_ = NULL;
}

void RenderingWidget::initializeGL()
{
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DOUBLEBUFFER);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1);

	SetLight();

}

void RenderingWidget::resizeGL(int w, int h)
{
	h = (h == 0) ? 1 : h;

	ptr_arcball_->reSetBound(w, h);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, GLdouble(w) / GLdouble(h), 0.001, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderingWidget::paintGL()
{
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (has_lighting_)
	{
		SetLight();
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	register vec eyepos = eye_distance_*eye_direction_;
	gluLookAt(eyepos[0], eyepos[1], eyepos[2],
		eye_goal_[0], eye_goal_[1], eye_goal_[2],
		0.0, 1.0, 0.0);
	glPushMatrix();

	glMultMatrixf(ptr_arcball_->GetBallMatrix());

	Render();
	glPopMatrix();
}

void RenderingWidget::timerEvent(QTimerEvent * e)
{
	updateGL();
}

void RenderingWidget::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		if ( is_selecting_fixed_ )
		{
			is_dragging_ = true;
			if ( !current_index_.empty() )
			{
				for (int i = 0; i != current_index_.size(); i++)
				{
					vertex_colors_[current_index_[i]].r = 0.28;
					vertex_colors_[current_index_[i]].g = 0.28;
					vertex_colors_[current_index_[i]].b = 0.28;
				}
			}
		}
		else
		{
			ptr_arcball_->MouseDown(e->pos());
		}
		break;

	case Qt::MidButton:
		current_position_ = e->pos();
		break;
	case Qt::RightButton:
		if ( is_selecting_handle_ )
		{
			QPoint vPosGL = e->pos();
			vPosGL.ry() = height()- vPosGL.ry();		// coordinate transformation
			double err = 0;

			int v_index = PickOnePoint(vPosGL,err);	// select a vertex in mesh pMesh

			if ( v_index>=0 && err<6 )	//the vertex is valid	
			{
				int curr_size = current_index_.size();
				if ( current_index_.size() != 0 )
				{
					vector<int> part_route = DijkstraPath(current_index_[curr_size-1], v_index);
					handle_route_.push_back(part_route);
				}

				current_index_.push_back(v_index);
				handle_index_ = current_index_;
			}
		}
		if (is_laplacian_ && is_rotation_ && handle_index_.size())
		{
			ptr_arcball_local_->MouseDown(e->pos());
		}
		break;
		if (is_translation_)
			current_position_ = e->pos();
	default:
		break;
	}

	start_position_ = e->pos();
	current_position_ = start_position_;

	updateGL();
}

void RenderingWidget::mouseMoveEvent(QMouseEvent *e)
{
	switch (e->buttons())
	{
		setCursor(Qt::ClosedHandCursor);
	case Qt::LeftButton:
		if ( is_selecting_fixed_ )
		{
			current_position_ = e->pos();
			if (!current_index_.empty())
			{
				for (int i = 0; i != current_index_.size(); i++)
				{
					vertex_colors_[current_index_[i]].r = 1.0;
					vertex_colors_[current_index_[i]].g = 1.0;
					vertex_colors_[current_index_[i]].b = 1.0;
				}
			}
			glPushMatrix();
			glMultMatrixf(ptr_arcball_->GetBallMatrix());
			PickVert();
			glPopMatrix();
		}

		else
		{
			ptr_arcball_->MouseMove(e->pos());
		}
		
		break;
	case Qt::MidButton:
		eye_goal_[0] -= 4.0*GLfloat(e->x() - current_position_.x()) / GLfloat(width());
		eye_goal_[1] += 4.0*GLfloat(e->y() - current_position_.y()) / GLfloat(height());
		current_position_ = e->pos();
		break;
	case Qt::RightButton:
		if (is_laplacian_ && handle_index_.size())
		{
			if (!mesh_editor_)
				mesh_editor_ = new Laplacian_Editor();
			if (is_translation_)
			{
				Translation(e->pos(), current_position_);
				current_position_ = e->pos();
				mesh_editor_->Do_Laplacian_Editor(*(ptr_mesh_->get_vertex_list()), fixed_index_, handle_index_);
				ptr_mesh_->UpdateNormal();
			}

			else if (is_rotation_)
			{
				Rotation(e->pos());
				mesh_editor_->Do_Laplacian_Editor(*(ptr_mesh_->get_vertex_list()), fixed_index_, handle_index_);
				ptr_mesh_->UpdateNormal();
			}
		}
		break;
	default:
		break;
	}

	updateGL();
}
void RenderingWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		break;
	default:
		break;
	}
	updateGL();
}

void RenderingWidget::mouseReleaseEvent(QMouseEvent *e)
{
	current_position_ = e->pos();
	is_dragging_ = false;

	switch (e->button())
	{
	case Qt::LeftButton:
		if ( !is_selecting_fixed_ )
		{
			ptr_arcball_->MouseUp(e->pos());
			setCursor(Qt::ArrowCursor);
		}
		
		break;

	case Qt::RightButton:
		//if (is_laplacian_ && is_rotation_ && handle_index_.size())
		//	ptr_arcball_local_->MouseUp(e->pos());
		break;
	default:
		break;
	}
}

void RenderingWidget::wheelEvent(QWheelEvent *e)
{
	eye_distance_ += e->delta()*0.001;
	eye_distance_ = eye_distance_ < 0 ? 0 : eye_distance_;

	updateGL();
}

void RenderingWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_A:
		break;
	default:
		break;
	}
}

void RenderingWidget::keyReleaseEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_A:
		break;
	default:
		break;
	}
}

void RenderingWidget::Render()
{
	DrawAxes(is_draw_axes_);

	DrawPoints(is_draw_point_);
	DrawEdge(is_draw_edge_);
	DrawFace(is_draw_face_);
	DrawTexture(is_draw_texture_);

	if( is_selecting_fixed_ && is_dragging_ )
	{
		DrawXORRect();
	}

	int handle_size = handle_index_.size();
	if ( handle_size )
	{
		glPointSize(5.0);
		glColor3f(0.0,1.0,0.0);
		glBegin(GL_POINTS);
		for ( int i=0; i<handle_size; i++ )
		{
			glVertex3fv(ptr_mesh_->get_vertex(handle_index_[i])->position_);
		}
		glEnd();
	}

	int route_size = handle_route_.size();
	if ( route_size )
	{
		glLineWidth(2.0);
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_LINES);
		for ( int i=0; i<route_size; i++ )
		{
			int part_route_size = handle_route_[i].size()-1;
			for ( int j=0; j<part_route_size; j++ )
			{
				glVertex3fv(ptr_mesh_->get_vertex(handle_route_[i][j])->position_);
				glVertex3fv(ptr_mesh_->get_vertex(handle_route_[i][j+1])->position_);
			}
		}
		glEnd();
	}


	int fixed_size = fixed_index_.size();
	for ( int i=0; i<fixed_size; i++ )
	{
		vertex_colors_[fixed_index_[i]].r = 0.0;
		vertex_colors_[fixed_index_[i]].g = 0.0;
		vertex_colors_[fixed_index_[i]].b = 1.0;
	}

	
}

void RenderingWidget::SetLight()
{
	static GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat mat_shininess[] = { 50.0 };
	static GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	static GLfloat white_light[] = { 0.8, 0.8, 0.8, 1.0 };
	static GLfloat lmodel_ambient[] = { 0.3, 0.3, 0.3, 1.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void RenderingWidget::SetBackground()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("background color"));
	GLfloat r = (color.red()) / 255.0f;
	GLfloat g = (color.green()) / 255.0f;
	GLfloat b = (color.blue()) / 255.0f;
	GLfloat alpha = color.alpha() / 255.0f;
	glClearColor(r, g, b, alpha);
	updateGL();
}

void RenderingWidget::ReadMesh()
{
	QString filename = QFileDialog::
		getOpenFileName(this, tr("Read Mesh"),
		"..", tr("Meshes (*.obj)"));

	if (filename.isEmpty())
	{
		emit(operatorInfo(QString("Read Mesh Failed!")));
		return;
	}
	//中文路径支持
	QTextCodec *code = QTextCodec::codecForName("gd18030");
	QTextCodec::setCodecForLocale(code);

	QByteArray byfilename = filename.toLocal8Bit();
	ptr_mesh_->LoadFromOBJFile(byfilename.data());

	vertex_colors_.resize(ptr_mesh_->num_of_vertex_list());
	for (size_t i=0; i!=vertex_colors_.size(); i++)
	{
		vertex_colors_[i].r = 1.0;
		vertex_colors_[i].g = 1.0;
		vertex_colors_[i].b = 1.0;
	}

	is_dragging_ = is_selecting_fixed_ = is_selecting_handle_ = false;
	fixed_index_.clear();
	handle_index_.clear();
	handle_route_.clear();

	//	m_pMesh->LoadFromOBJFile(filename.toLatin1().data());
	emit(operatorInfo(QString("Read Mesh from") + filename + QString(" Done")));
	emit(meshInfo(ptr_mesh_->num_of_vertex_list(), ptr_mesh_->num_of_edge_list(), ptr_mesh_->num_of_face_list()));
	if (mesh_editor_)
		delete(mesh_editor_);
	mesh_editor_ = new Laplacian_Editor();
	pre_vertices_.resize(ptr_mesh_->num_of_vertex_list());
	for (int i = 0; i < pre_vertices_.size(); i++)
		pre_vertices_[i] = ptr_mesh_->get_vertex(i)->position_;
	updateGL();
}

void RenderingWidget::WriteMesh()
{
	if (ptr_mesh_->num_of_vertex_list() == 0)
	{
		emit(QString("The Mesh is Empty !"));
		return;
	}
	QString filename = QFileDialog::
		getSaveFileName(this, tr("Write Mesh"),
		"..", tr("Meshes (*.obj)"));

	if (filename.isEmpty())
		return;

	ptr_mesh_->WriteToOBJFile(filename.toLatin1().data());

	emit(operatorInfo(QString("Write Mesh to ") + filename + QString(" Done")));
}

void RenderingWidget::LoadTexture()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Texture"),
		"..", tr("Images(*.bmp *.jpg *.png *.jpeg)"));
	if (filename.isEmpty())
	{
		emit(operatorInfo(QString("Load Texture Failed!")));
		return;
	}


	glGenTextures(1, &texture_[0]);
	QImage tex1, buf;
	if (!buf.load(filename))
	{
		//        QMessageBox::warning(this, tr("Load Fialed!"), tr("Cannot Load Image %1").arg(filenames.at(0)));
		emit(operatorInfo(QString("Load Texture Failed!")));
		return;
		/*
		QImage dummy(128, 128, QImage::Format_ARGB32);
		dummy.fill(Qt::green);
		buf = dummy;
		*/
	}

	tex1 = QGLWidget::convertToGLFormat(buf);
	glBindTexture(GL_TEXTURE_2D, texture_[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, tex1.width(), tex1.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits());

	is_load_texture_ = true;
	emit(operatorInfo(QString("Load Texture from ") + filename + QString(" Done")));
}

void RenderingWidget::SelectFixedPoint()
{
	if ( is_selecting_fixed_ && current_index_.size()!=0 )
	{
		if ( fixed_index_.size()==0 )
		{
			fixed_index_ = current_index_;
		}
		else
		{
			int size_cur = current_index_.size();
			int size_fit = fixed_index_.size();
			for ( int i=0; i<size_cur; i++ )
			{
				int j = 0;
				for ( ; j<size_fit; j++ )
				{
					if ( current_index_[i] == fixed_index_[j] )
					{
						break;
					}
				}
				if ( j == size_fit )
				{
					fixed_index_.push_back(current_index_[i]);
				}
			}
		}
	}

	is_selecting_fixed_ = !is_selecting_fixed_;
	is_selecting_handle_ = false;
	current_index_.clear();

	updateGL();
}

void RenderingWidget::SelectHandle()
{
	if ( !is_selecting_handle_ )
	{
		for ( int i=0; i<current_index_.size(); i++)
		{
			vertex_colors_[current_index_[i]].r = 1.0;
			vertex_colors_[current_index_[i]].g = 1.0;
			vertex_colors_[current_index_[i]].b = 1.0;
		}
		current_index_.clear();
		handle_index_.clear();
		handle_route_.clear();
	}
	else
	{
		if ( handle_index_.size())
		{
			vector<int> part_route = DijkstraPath(handle_index_[handle_index_.size()-1],handle_index_[0]);
			handle_route_.push_back(part_route);
		}
	}

	is_selecting_handle_ = !is_selecting_handle_;
	is_selecting_fixed_ = false;
	current_index_.clear();

	updateGL();
}

int RenderingWidget::PickOnePoint(QPoint p, double &err)
{
	Vec3f vDstPos( p.rx(), p.ry(), 0 ); 

	////////////////////////////////////////////////////////////////////
	//	Step.1	Get Modelview/Projection Matrices
	double	matMV[16] , matProj[16];
	int		ViewPort[4];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(ptr_arcball_->GetBallMatrix ());
	glGetDoublev  (GL_MODELVIEW_MATRIX, matMV);
	glGetDoublev  (GL_PROJECTION_MATRIX, matProj);
	glGetIntegerv (GL_VIEWPORT, ViewPort);

	glPopMatrix();

	////////////////////////////////////////////////////////////////////
	//	Step.2	Searching the nearest vertex

	//	2.1	Store the value
	int		nNearestVrtIdx = -1;
	int		nNearestTriIdx	= -1;
	double	dNearestTriDist	= 10000;

	//	2.2	Search along the triangle
	int m_nTriangles = ptr_mesh_->num_of_face_list(); 
	for( int iTri=0; iTri<m_nTriangles; iTri++ )
	{
		// get the triangle
		HE_face *pTri = ptr_mesh_->get_face(iTri);

		// Get triangel center distance;
		double dTriDist = 0;
		{
			Vec3f& vTriCent = pTri->center();

			double dScrX,dScrY,dScrZ;
			gluProject (vTriCent[0], vTriCent[1], vTriCent[2], 
				matMV, matProj, ViewPort, 
				&dScrX,&dScrY,&dScrZ);

			dTriDist = dScrZ;
		}

		//	Check whether the nearest
		if (dTriDist < dNearestTriDist)	
		{	
			// Get Vertex Screen Positions		
			Vec3f pvSrcPos[3];	

			vector<HE_vert *> verts;
			pTri->face_verts(verts);
			{	// Project them
				for( int nVrt=0; nVrt<3; nVrt++)
				{

					Vec3f& vPos = verts[nVrt]->position_ ;

					double dScrX,dScrY,dScrZ;
					gluProject (vPos[0], vPos[1], vPos[2], 
						matMV, matProj, ViewPort, 
						&dScrX,&dScrY,&dScrZ);

					pvSrcPos[nVrt][0]	= float(dScrX);
					pvSrcPos[nVrt][1]	= float(dScrY);				
					pvSrcPos[nVrt][2]	= 0;
				}
			}

			// if the pixel is not in the area of the triangle, skip this triangle        
			if( igtPointInTriangle2D( vDstPos, pvSrcPos) )
			{
				//	Keep the best triangle info
				nNearestTriIdx	= iTri;
				dNearestTriDist	= dTriDist;


				// Get the nearest vertices
				float fNearestVrtDist  = 1000;
				for( int nVrt=0; nVrt<3; nVrt++)
				{
					// get the vertex
					int vIdx = verts[nVrt]->id_;				

					// the distance between the cursor and the projected vertex
					float fLen = (float)((vDstPos - pvSrcPos[nVrt]).length());

					if ( fLen < fNearestVrtDist )
					{
						fNearestVrtDist	= fLen;
						nNearestVrtIdx	= vIdx;
					}
				}
				err = fNearestVrtDist;
			}
		}
	}

	return nNearestVrtIdx;
}

void RenderingWidget::CheckDrawPoint(bool bv)
{
	is_draw_point_ = bv;
	updateGL();
}
void RenderingWidget::CheckDrawEdge(bool bv)
{
	is_draw_edge_ = bv;
	updateGL();
}
void RenderingWidget::CheckDrawFace(bool bv)
{
	is_draw_face_ = bv;
	updateGL();
}
void RenderingWidget::CheckLight(bool bv)
{
	has_lighting_ = bv;
	updateGL();
}
void RenderingWidget::CheckDrawTexture(bool bv)
{
	is_draw_texture_ = bv;
	if (is_draw_texture_)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	updateGL();
}
void RenderingWidget::CheckDrawAxes(bool bV)
{
	is_draw_axes_ = bV;
	updateGL();
}

void RenderingWidget::DrawAxes(bool bV)
{
	if (!bV)
		return;
	//x axis
	float long_x_ =3 * sqrt(pow(axis_x_[0], 2) + pow(axis_x_[1] , 2) + pow(axis_x_[2], 2));
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(center_[0], center_[1], center_[2]);
	glVertex3f(axis_x_[0]/ long_x_ + center_[0], axis_x_[1] / long_x_ + center_[1], axis_x_[2] / long_x_ + center_[2]);
	glEnd();
	glPushMatrix();
	glTranslatef(axis_x_[0] / long_x_ + center_[0], axis_x_[1] / long_x_ + center_[1], axis_x_[2] / long_x_ + center_[2]);
	glRotatef(90, axis_y_[0], axis_y_[1], axis_y_[2]);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	//y axis
	float long_y_ =3 * sqrt(pow(axis_y_[0], 2) + pow(axis_y_[1], 2) + pow(axis_y_[2], 2));
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(center_[0], center_[1], center_[2]);
	glVertex3f(axis_y_[0]/ long_y_ + center_[0], axis_y_[1]/ long_y_ + center_[1], axis_y_[2]/ long_y_ + center_[2]);
	glEnd();
	glPushMatrix();
	glTranslatef(axis_y_[0] / long_y_ + center_[0], axis_y_[1] / long_y_ + center_[1], axis_y_[2] / long_y_ + center_[2]);
	glRotatef(90, -axis_x_[0],-axis_x_[1],-axis_x_[2]);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	//z axis
	float long_z_ = 3 * sqrt(pow(axis_z_[0], 2) + pow(axis_z_[1], 2) + pow(axis_z_[2], 2));
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(center_[0], center_[1], center_[2]);
	glVertex3f(axis_z_[0]/ long_z_ + center_[0], axis_z_[1]/ long_z_ + center_[1], axis_z_[2]/ long_z_ + center_[2]);
	glEnd();
	glPushMatrix();
	glTranslatef(axis_z_[0] / long_z_ + center_[0], axis_z_[1] / long_z_ + center_[1], axis_z_[2] / long_z_ + center_[2]);
	glutSolidCone(0.02, 0.06, 20, 10);
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}

void RenderingWidget::DrawPoints(bool bv)
{
	if (!bv || ptr_mesh_ == NULL)
		return;
	if (ptr_mesh_->num_of_vertex_list() == 0)
	{
		return;
	}

	const std::vector<HE_vert*>& verts = *(ptr_mesh_->get_vertex_list());
	glPointSize(1.0);
	glBegin(GL_POINTS);
	for (size_t i = 0; i != ptr_mesh_->num_of_vertex_list(); ++i)
	{
		glColor3f(vertex_colors_[i].r, vertex_colors_[i].g, vertex_colors_[i].b);
		glNormal3fv(verts[i]->normal().data());
		glVertex3fv(verts[i]->position().data());
	}
	glEnd();
}

void RenderingWidget::DrawEdge(bool bv)
{
	if (!bv || ptr_mesh_ == NULL)
		return;

	if (ptr_mesh_->num_of_face_list() == 0)
	{
		return;
	}

	const std::vector<HE_face *>& faces = *(ptr_mesh_->get_faces_list());
	glLineWidth(1.0);
	for (size_t i = 0; i != faces.size(); ++i)
	{
		glBegin(GL_LINE_LOOP);
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
		glEnd();
	}
}

void RenderingWidget::DrawFace(bool bv)
{
	if (!bv || ptr_mesh_ == NULL)
		return;

	if (ptr_mesh_->num_of_face_list() == 0)
	{
		return;
	}

	const std::vector<HE_face *>& faces = *(ptr_mesh_->get_faces_list());

	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i != faces.size(); ++i)
	{
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
	}

	glEnd();
}

void RenderingWidget::DrawTexture(bool bv)
{
	if (!bv)
		return;
	if (ptr_mesh_->num_of_face_list() == 0 || !is_load_texture_)
		return;

	//默认使用球面纹理映射，效果不好
	ptr_mesh_->SphereTex();

	const std::vector<HE_face *>& faces = *(ptr_mesh_->get_faces_list());

	glBindTexture(GL_TEXTURE_2D, texture_[0]);
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i != faces.size(); ++i)
	{
		HE_edge *pedge(faces.at(i)->pedge_);
		do
		{
			/*请在此处绘制纹理，添加纹理坐标即可*/
			glTexCoord2fv(pedge->pvert_->texCoord_.data());
			glNormal3fv(pedge->pvert_->normal().data());
			glVertex3fv(pedge->pvert_->position().data());

			pedge = pedge->pnext_;

		} while (pedge != faces.at(i)->pedge_);
	}

	glEnd();
}

void RenderingWidget::DrawXORRect()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width(), height(),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINE_LOOP);
	glVertex2f(start_position_.x(), start_position_.y());
	glVertex2f(current_position_.x(), start_position_.y());
	glVertex2f(current_position_.x(), current_position_.y());
	glVertex2f(start_position_.x(), current_position_.y());
	glEnd();
	glDisable(GL_LOGIC_OP);

	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void RenderingWidget::PickVert()
{
	current_index_.clear();
	QPoint mid = (start_position_ + current_position_)/2;
	mid.setY(height() - mid.y());
	QPoint wid = (start_position_ - current_position_);
	if(wid.x() < 0) wid.setX(-wid.x());
	if(wid.y() < 0) wid.setY(-wid.y());
	if(wid.x() == 0 || wid.y() == 0) return;
	int sz = 5 * ptr_mesh_->num_of_vertex_list();
	GLuint *selectBuf = new GLuint[sz];
	glSelectBuffer(sz, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(-1);
	double mp[16];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(mid.x(), mid.y(), wid.x(), wid.y(), viewport);
	glMultMatrixd(mp);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	for(int i = 0; i != ptr_mesh_->num_of_vertex_list(); i++)
	{
		glLoadName(i);
		glBegin(GL_POINTS);
		glVertex3fv(ptr_mesh_->get_vertex(i)->position());
		glEnd();
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	long hits = glRenderMode(GL_RENDER);
	vector<pair<double,unsigned int>> H;
	for(long i=0; i < hits; i++)
	{
		H.push_back(pair<double,unsigned int>(selectBuf[i * 4 + 1]/4294967295.0,selectBuf[i * 4 + 3]));
	}
	sort(H.begin(), H.end());
	current_index_.resize(H.size());
	for(int i = 0; i != hits; i++)
	{
		current_index_[i] = H[i].second;

		vertex_colors_[H[i].second].r = 0.0;
		vertex_colors_[H[i].second].g = 1.0;
		vertex_colors_[H[i].second].b = 0.0;
	}
	delete [] selectBuf;
}

float RenderingWidget::Distance(int i,int j)
{
	if (i == j)
	{
		return 0;
	}
	else if ( ptr_mesh_->isNeighbors(ptr_mesh_->get_vertex(i),ptr_mesh_->get_vertex(j)) )
	{
		return (ptr_mesh_->get_vertex(i)->position()-ptr_mesh_->get_vertex(j)->position()).length();
	}
	else
	{
		return MAX_DISTANCE;
	}
}

vector<int> RenderingWidget::DijkstraPath(int i,int j)   
{
	int num_of_vertex = ptr_mesh_->num_of_vertex_list();             //*节点数目*/; 
	vector<float> dist;                //*各个节点到达源节点的距离*/;  
	vector<int> pre_vertex;            //*各个节点的前一个节点*/;  
	vector<bool> is_in;                //是否已经在S集合中;  
	vector<int> route;				   //存储路径（id）;

	dist.clear();
	pre_vertex.clear();
	is_in.clear();
	dist.resize(num_of_vertex);
	pre_vertex.resize(num_of_vertex);
	is_in.assign(num_of_vertex, false);   //初始化，所有的节点都不在S集合中; 

	//*初始化distance和prevVertex数组*/;  
	for(int k =0; k < num_of_vertex; k++)  
	{  
		dist[k] = Distance(i,k);
		pre_vertex[k] = i;
	} 

	//*开始使用贪心思想循环处理不在S集合中的每一个节点*/;  
	is_in[i] = true;          //开始节点放入S集合中;  
	int current_point = i;    //记录当前添加到S集合中的点;
	float current_least_dist = 0;  //U中点到S中点的最短距离;

	while (!is_in[j])
	{
		float least_dist = MAX_DISTANCE;
		for (int k=0; k<num_of_vertex; k++)
		{
			if (!is_in[k])
			{
				if (dist[k]<least_dist)
				{
					least_dist = dist[k];
					current_point = k;
				}
			}
		}
		current_least_dist = least_dist;
		is_in[current_point] = true;
		for (int k=0; k<num_of_vertex; k++)
		{
			if (!is_in[k])
			{
				float new_dist = Distance(current_point,k)+current_least_dist;
				if (new_dist<dist[k])
				{
					dist[k] = new_dist;
					pre_vertex[k] = current_point;
				}
			}
		}
	}
	route.push_back(j);

	int m = pre_vertex[j];
	do 
	{
		route.push_back(m);
		m = pre_vertex[m];
	} while (m!=i);
	route.push_back(i);

	vector<int> temp = route;
	for (int k=0; k<temp.size(); k++)
	{
		route[k] = temp[temp.size()-k-1];
	}

	return route;
}

void RenderingWidget::CheckLaplacianEditor(bool bv)
{
	is_laplacian_ = bv;
}

void RenderingWidget::CheckTranslation(bool bv)
{
	is_translation_ = bv;
	is_rotation_ = !bv;
	is_no_operation_ = !bv;
}

void RenderingWidget::CheckRotation(bool bv)
{
	is_rotation_ = bv;
	is_translation_ = !bv;
	is_no_operation_ = !bv;
}

void RenderingWidget::CheckOperation(bool bv)
{
	is_no_operation_ = bv;
	is_translation_ = !bv;
	is_rotation_ = !bv;
}

void RenderingWidget::RestoreMesh()
{
	if (!ptr_mesh_->num_of_vertex_list())
		return;
	fixed_index_.clear();
	is_selecting_fixed_ = false;
	handle_index_.clear();
	is_selecting_handle_ = false;
	int size_ = pre_vertices_.size();
	for (int i = 0; i < size_; i++)
		ptr_mesh_->get_vertex(i)->position_ = pre_vertices_[i];
	ptr_mesh_->UpdateNormal();
	if (mesh_editor_)
		delete(mesh_editor_);
	mesh_editor_ = NULL;
	updateGL();
}

void RenderingWidget::Predecomposition()
{
	if (!handle_index_.size())
		return;
	if (!mesh_editor_)
		mesh_editor_ = new Laplacian_Editor();
	mesh_editor_->Init(*(ptr_mesh_->get_vertex_list()), fixed_index_, handle_index_);
	CalculateCenter();
	DrawAxes(is_draw_axes_);
	updateGL();
}

void RenderingWidget::CalculateCenter()
// Get the center of handle points
{
	if (!handle_index_.size())
		return;
	double ave_x_ = 0, ave_y_ = 0, ave_z_ = 0;
	for (int i = 0; i < handle_index_.size(); i++)
	{
		ave_x_ = ave_x_ + ptr_mesh_->get_vertex(handle_index_[i])->position_[0];
		ave_y_ = ave_y_ + ptr_mesh_->get_vertex(handle_index_[i])->position_[1];
		ave_z_ = ave_z_ + ptr_mesh_->get_vertex(handle_index_[i])->position_[2];
	}
	ave_x_ = ave_x_ / handle_index_.size();
	ave_y_ = ave_y_ / handle_index_.size();
	ave_z_ = ave_z_ / handle_index_.size();
	center_[0] = ave_x_;
	center_[1] = ave_y_;
	center_[2] = ave_z_;
}


point RenderingWidget::screenPt_2_GLPt(QPoint screen_pos_)
{
	double	mat_mv_[16], mat_proj_[16];
	int		view_port_[4];
	GLfloat win_x_, win_y_, win_z_;
	GLdouble pos_x_, pos_y_, pos_z_;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(ptr_arcball_->GetBallMatrix());
	glGetDoublev(GL_MODELVIEW_MATRIX, mat_mv_);
	glGetDoublev(GL_PROJECTION_MATRIX, mat_proj_);
	glGetIntegerv(GL_VIEWPORT, view_port_);

	glPopMatrix();

	Vec3f ref_pos_ = ptr_mesh_->get_vertex(0)->position_;		// reference point

	double scr_x_, scr_y_, scr_z_;
	gluProject(ref_pos_[0], ref_pos_[1], ref_pos_[2],			// projection
		mat_mv_, mat_proj_, view_port_,
		&scr_x_, &scr_y_, &scr_z_);

	win_x_ = (float)screen_pos_.rx();
	win_y_ = (float)view_port_[3] - ((float)screen_pos_.ry());
	glReadPixels(int(win_x_), int(win_y_), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &win_z_);
	gluUnProject(win_x_, win_y_, scr_z_, mat_mv_, mat_proj_, view_port_, &pos_x_, &pos_y_, &pos_z_);
	return point(float(pos_x_), float(pos_y_), float(pos_z_));
}


void RenderingWidget::Translation(QPoint cur_pos_, QPoint pre_pos_)
{
	if (!handle_index_.size())
		return;
	point translation_ = screenPt_2_GLPt(cur_pos_) - screenPt_2_GLPt(pre_pos_);
	for (int i = 0; i < handle_index_.size(); i++)
		ptr_mesh_->get_vertex(handle_index_[i])->position_ = ptr_mesh_->get_vertex(handle_index_[i])->position_+ translation_;
	CalculateCenter();
	center_ = center_ + translation_;
	DrawAxes(is_draw_axes_);
}

void RenderingWidget::Rotation(QPoint cur_pos_)
{
	if (!handle_index_.size())
		return;
	ptr_arcball_local_->MouseMove(cur_pos_);
	glMultMatrixf(ptr_arcball_->GetBallMatrix());
	float *ball_matrix_ = ptr_arcball_local_->GetBallMatrix();
	double pos_x_, pos_y_, pos_z_;
	CalculateCenter();
	for (int i = 0; i < handle_index_.size(); i++)
	{
		// get the relevant coordinates
		pos_x_ = ptr_mesh_->get_vertex(handle_index_[i])->position_[0] - center_[0];
		pos_y_ = ptr_mesh_->get_vertex(handle_index_[i])->position_[1] - center_[1];
		pos_z_ = ptr_mesh_->get_vertex(handle_index_[i])->position_[2] - center_[2];
		// update the coordinates
		ptr_mesh_->get_vertex(handle_index_[i])->position_[0] = ball_matrix_[0] * pos_x_ + ball_matrix_[1] * pos_y_ + ball_matrix_[2] * pos_z_ + ball_matrix_[3] + center_[0];
		ptr_mesh_->get_vertex(handle_index_[i])->position_[1] = ball_matrix_[4] * pos_x_ + ball_matrix_[5] * pos_y_ + ball_matrix_[6] * pos_z_ + ball_matrix_[7] + center_[1];
		ptr_mesh_->get_vertex(handle_index_[i])->position_[2] = ball_matrix_[8] * pos_x_ + ball_matrix_[9] * pos_y_ + ball_matrix_[10] * pos_z_ + ball_matrix_[11] + center_[2];

	}
	// Update axis
	pos_x_ = axis_x_[0];
	pos_y_ = axis_x_[1];
	pos_z_ = axis_x_[2];
	axis_x_[0] = ball_matrix_[0] * pos_x_ + ball_matrix_[1] * pos_y_ + ball_matrix_[2] * pos_z_ + ball_matrix_[3];
	axis_x_[1] = ball_matrix_[4] * pos_x_ + ball_matrix_[5] * pos_y_ + ball_matrix_[6] * pos_z_ + ball_matrix_[7];
	axis_x_[2] = ball_matrix_[8] * pos_x_ + ball_matrix_[9] * pos_y_ + ball_matrix_[10] * pos_z_ + ball_matrix_[11];

	pos_x_ = axis_y_[0];
	pos_y_ = axis_y_[1];
	pos_z_ = axis_y_[2];
	axis_y_[0] = ball_matrix_[0] * pos_x_ + ball_matrix_[1] * pos_y_ + ball_matrix_[2] * pos_z_ + ball_matrix_[3];
	axis_y_[1] = ball_matrix_[4] * pos_x_ + ball_matrix_[5] * pos_y_ + ball_matrix_[6] * pos_z_ + ball_matrix_[7];
	axis_y_[2] = ball_matrix_[8] * pos_x_ + ball_matrix_[9] * pos_y_ + ball_matrix_[10] * pos_z_ + ball_matrix_[11];

	pos_x_ = axis_z_[0];
	pos_y_ = axis_z_[1];
	pos_z_ = axis_z_[2];
	axis_z_[0] = ball_matrix_[0] * pos_x_ + ball_matrix_[1] * pos_y_ + ball_matrix_[2] * pos_z_ + ball_matrix_[3];
	axis_z_[1] = ball_matrix_[4] * pos_x_ + ball_matrix_[5] * pos_y_ + ball_matrix_[6] * pos_z_ + ball_matrix_[7];
	axis_z_[2] = ball_matrix_[8] * pos_x_ + ball_matrix_[9] * pos_y_ + ball_matrix_[10] * pos_z_ + ball_matrix_[11];

	DrawAxes(is_draw_axes_);
}