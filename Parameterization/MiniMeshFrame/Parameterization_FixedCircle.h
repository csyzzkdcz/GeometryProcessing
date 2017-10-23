#pragma once
#ifndef RADIUS
#define RADIUS 1.5
#endif

#ifndef PI
#define PI 3.1415926
#endif

#include"Parameterization.h"
#include"Parameterization_FixedBoundary.h"
#include <Eigen/Eigen>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
using namespace Eigen;

class Parameterization_FixedCircle : public Parameterization_FixedBoundary
{
public:
	Parameterization_FixedCircle();
	virtual ~Parameterization_FixedCircle();

public:
	void virtual Do_Parameterizing(vector<HE_vert*> &vertices_);

protected:
	void Mapping_Boundary_to_Circle(vector<HE_vert*> vertices_);

protected:
	SparseLU <SparseMatrix<float>, COLAMDOrdering<int>>  LU;	//store the coefficient matrix as the formation of LU decomposition
	MatrixXf b;					// store the vector b in Ax = b
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};