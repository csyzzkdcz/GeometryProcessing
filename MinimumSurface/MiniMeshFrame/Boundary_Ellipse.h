#pragma once
#include"BoundaryCurves.h"

class Boundary_Ellipse : public BoundaryCurves
{
public:
	Boundary_Ellipse();
	~Boundary_Ellipse();
	Boundary_Ellipse(double long_axis_, double short_axis_);

public:
	void Curves(vector<double> &rx_array_, vector<double> &ry_array_);

private:
	double long_axis;
	double short_axis;
};