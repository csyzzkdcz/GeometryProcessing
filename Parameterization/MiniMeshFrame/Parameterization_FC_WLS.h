#pragma once
#include"Parameterization.h"
#include"Parameterization_FixedBoundary.h"
#include"Parameterization_FixedCircle.h"

class Parameterization_FC_WLS : public Parameterization_FixedCircle
{
public:
	Parameterization_FC_WLS();
	~Parameterization_FC_WLS();

public:
	void Do_Parameterizing(vector<HE_vert*> &vertices_);

private:
	void Generate_CoefMatrix(vector<HE_vert* > vertices_);
};