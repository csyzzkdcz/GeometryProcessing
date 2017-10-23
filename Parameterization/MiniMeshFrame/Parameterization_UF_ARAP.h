#pragma once
#include "Parameterization_UnFixedBoundary.h"

class Parameterization_ARAP : public Parameterization_UnFixedBoundary
{
public:
	Parameterization_ARAP();
	Parameterization_ARAP(Mesh3D* ptr_mesh_);
	~Parameterization_ARAP();

public:
	void Do_Parameterizing(vector<HE_vert*> &vertices_);

private:
	Matrix2f Get_Lt(int tri_index_);
	void Local_Phase();

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};