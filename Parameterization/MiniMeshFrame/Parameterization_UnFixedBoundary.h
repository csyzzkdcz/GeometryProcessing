#pragma once
#include"Parameterization.h"
#include "Parameterization_FS_SP.h"
#include <Eigen/Eigen>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
using namespace Eigen;

#ifndef ITR_NUM
#define MAX_ITR_NUM 50
#endif
#ifndef MIN_ERROR
#define MIN_ERROR 1e-3
#endif
#ifndef INF
#define INF 65536
#endif

class Parameterization_UnFixedBoundary : public Parameterization
{
public:
	Parameterization_UnFixedBoundary();
	virtual ~Parameterization_UnFixedBoundary();

public:
	void virtual Do_Parameterizing(vector<HE_vert*> &vertices_);

protected:
	void Mapping_tir3D_to_tri2D(int tri_index_, point &vertex_1_, point &vertex_2_, point &vertex_3_); // Mapping 3D triangle to 2D triangle
	void Initial_Parameterization(vector<HE_vert*> &vertices_);			 // use SP method to initialize the mesh								
	void Generate_Initial_Coor();
	float Get_cot(int tri_index_, int index_);							 // get the cot(theta) theta is the angle opposite the edge (x_t^i,x_t^(i+1)),t = tri_index_, i = index_ 
	float Get_cot(point p1, point p2, point p3);						// get the cot<p2p1,p3p1>
	void Global_Phase(vector<HE_vert*> vertices_);						// generate the coefficient matrix A and predecomposite it
	int Location_Vertex(int face_index_, HE_vert* vertex_);				// local order of vertex on the specific face
	Vector2f Get_X(int pos_index_, int face_index_);					// get the corresponding xi

protected:
	Mesh3D		*tmpt_mesh_;
	vector<Vec3f> vertices_record_;
	vector<Matrix2f> Lt_array_;
	vector<point> vertices_1_;
	vector<point> vertices_2_;
	vector<point> vertices_3_;
	SparseMatrix<float > A;
	SimplicialCholesky<SparseMatrix<float> > chol_;
};