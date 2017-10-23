#pragma once
#include"BaryCoordinate.h"
class BC_MeanValue : public BaryCoordinate
{
public:
	BC_MeanValue();
	~BC_MeanValue();

public:
	void Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_);

};