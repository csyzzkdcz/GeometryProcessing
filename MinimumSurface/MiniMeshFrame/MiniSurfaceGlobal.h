#pragma once
#include"MiniSurface.h"
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
using namespace Eigen;

class MiniSurfaceGlobal : public MiniSurface
{
public:
	MiniSurfaceGlobal();
	~MiniSurfaceGlobal();

public:
	void MiniSurface_Method();

private:
	SparseLU <SparseMatrix<float>, COLAMDOrdering<int>>  LU;	//store the coefficient matrix as the formation of LU decomposition


public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};