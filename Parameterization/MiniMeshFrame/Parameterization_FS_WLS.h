#pragma once
#include"Parameterization.h"
#include"Parameterization_FixedBoundary.h"
#include"Parameterization_FixedSquare.h"

class Parameterization_FS_WLS : public Parameterization_FixedSquare
{
public:
	Parameterization_FS_WLS();
	~Parameterization_FS_WLS();

public:
	void Do_Parameterizing(vector<HE_vert*> &vertices_);

private:
	void Generate_CoefMatrix(vector<HE_vert* > vertices_);
};