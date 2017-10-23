#pragma once
#include"BoundaryCurves.h"
class Boundary_Rect : public BoundaryCurves
{
public:
	Boundary_Rect();
	~Boundary_Rect();
	Boundary_Rect(double width_, double height_);

public:
	void Curves(vector<double> &rx_array_, vector<double> &ry_array_);

private:
	double width;
	double height;
};