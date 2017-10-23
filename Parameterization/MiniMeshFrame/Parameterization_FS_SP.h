#pragma once
#include"Parameterization.h"
#include"Parameterization_FixedBoundary.h"
#include"Parameterization_FixedSquare.h"
#ifndef EPS
#define EPS 1e-10
#endif

class Parameterization_FS_SP : public Parameterization_FixedSquare
{
public:
	Parameterization_FS_SP();
	~Parameterization_FS_SP();

public:
	void Do_Parameterizing(vector<HE_vert*> &vertices_);

private:
	void Generate_CoefMatrix(vector<HE_vert* > vertices_);
	void Get_SP_Barycentry(HE_vert* vertex_, vector<float> &wight_);
	void Shape_Preserve_Mapping(HE_vert* vertex_, vector<float> &neighbor_rx_, vector<float> &neighbor_ry_);
	float Tri_Area(float rx_1_, float ry_1_, float rz_1_, float rx_2_, float ry_2_, float rz_2_, float rx_3_, float ry_3_, float rz_3_);
};