#include "Boundary_Rect.h"
Boundary_Rect::Boundary_Rect()
{
	width = height = 0;
}

Boundary_Rect::~Boundary_Rect()
{
	width = height = 0;
}

Boundary_Rect::Boundary_Rect(double width_, double height_)
// Origin is the center
{
	width = width_;
	height = height_;
}

void Boundary_Rect::Curves(vector<double> &rx_array_, vector<double> &ry_array_)
{
	rx_array_.clear();
	ry_array_.clear();
	rx_array_.push_back(-width / 2);
	rx_array_.push_back(width / 2);
	rx_array_.push_back(width / 2);
	rx_array_.push_back(-width / 2);
	ry_array_.push_back(-height / 2);
	ry_array_.push_back(-height / 2);
	ry_array_.push_back(height / 2);
	ry_array_.push_back(height / 2);
}