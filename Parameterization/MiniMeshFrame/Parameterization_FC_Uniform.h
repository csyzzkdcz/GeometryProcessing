#pragma once
#include"Parameterization.h"
#include"Parameterization_FixedBoundary.h"
#include"Parameterization_FixedCircle.h"

class Parameterization_FC_Uniform : public Parameterization_FixedCircle
{
public:
	Parameterization_FC_Uniform();
	~Parameterization_FC_Uniform();

public:
	void Do_Parameterizing(vector<HE_vert*> &vertices_);

private:
	void Generate_CoefMatrix(vector<HE_vert* > vertices_);
};