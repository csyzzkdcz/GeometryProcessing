#pragma once
#include"BaryCoordinate.h"

class BC_Average : public  BaryCoordinate
{
public:
	BC_Average();
	~BC_Average();

public:
	void Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_);
};