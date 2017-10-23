#pragma once
#include<vector>
using namespace std;
class BaryCoordinate
{
public:
	BaryCoordinate();
	virtual ~BaryCoordinate();

public:
	virtual void Calculate_Coodinate(double pos_x_,double pos_y_,double pos_z_,vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_,vector<double> rz_array_);

protected:
	double dist(double point_start_x, double point_start_y, double point_start_z_, double point_end_x, double point_end_y, double point_end_z_);		// calculate the dist from start point to end point
	double calculate_cosin(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z);		// calculate the cosin of <P3P1,P1P2>
	double calculate_half_tangent(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z); // calculate the tan of 0.5*<P3P1,P1P2>
	double calculate_cotangent(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z);	// calculate the cotangent of <P3P1,P1P2>

	int Near_Int(double x);											// find the nearest integer for x

};