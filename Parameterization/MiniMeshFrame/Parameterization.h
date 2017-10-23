#pragma once
#include"HE_mesh\Mesh3D.h"
#include <vector>
using namespace std;

class Parameterization
{
public:
	Parameterization();
	virtual ~Parameterization();
	void virtual Do_Parameterizing(vector<HE_vert*> &vertices_);

protected:
	float Get_Distance(float start_rx_, float start_ry_, float start_rz_, float end_rx_, float end_ry_, float end_rz_);

};