#pragma once
#define REFINED_RATIO 0.5
#include"HE_mesh\Mesh3D.h"
#include"BaryCoordinate.h"
#include"BC_Average.h"
#include"BC_MeanValue.h"
#include"BC_DiscretHarmonic.h"
#include<vector>
#include "Eigen"
using namespace std;
using namespace Eigen;

class MiniSurface
{
public:
	MiniSurface();
	virtual ~MiniSurface();

public:
	void virtual MiniSurface_Method();
	void Init(Mesh3D *ptr_mesh_);
	void Get_Init_Vertices();
	Mesh3D						*tar_mesh_;						// target mesh to be edit
	BaryCoordinate				*BC_Editor_;						// get the barycentric coordinate

protected:
	void get_neighborId(const size_t& vertid, std::vector<size_t>& neighbors);
	void Calculate_BC(int index_, vector<double> &BC_weight_, vector<size_t> neighbors_);
	void Update_Surface();

protected:
	vector<Vector3f>			init_vertices_;					// initial vertices
	vector<bool>				is_boundary_;					// judge whether a vertex is on the boundary
	vector<Vector3f>			final_vertices_;					// vertices after editting

};