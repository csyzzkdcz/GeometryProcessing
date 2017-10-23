#include"Boundary_Ellipse.h"
#define pi 3.1415
#define EPS_THETA 1e-3
#define POINT_NUM 100
Boundary_Ellipse::Boundary_Ellipse()
{
	long_axis = short_axis = 0;

}

Boundary_Ellipse::Boundary_Ellipse(double long_axis_, double short_axis_)
{
	long_axis = long_axis_;
	short_axis = short_axis_;
}

Boundary_Ellipse::~Boundary_Ellipse()
{
	long_axis = short_axis = 0;
}

void Boundary_Ellipse::Curves(vector<double> &rx_array_, vector<double> &ry_array_)
{
	rx_array_.clear();
	ry_array_.clear();
	for (int i = 0; i<POINT_NUM ; i++)
	{
		if (i == POINT_NUM / 2)				// To ensure the symmetry
		{
			rx_array_.push_back(-long_axis);
			ry_array_.push_back(0);
		}
		else
		{
			rx_array_.push_back(long_axis*cos(i * 2 * pi / POINT_NUM));
			ry_array_.push_back(short_axis*sin(i * 2 * pi / POINT_NUM));
		}
	}
}
