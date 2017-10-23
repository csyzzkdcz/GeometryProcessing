#include"BC_Average.h"
#include<iostream>
using std::cout;
BC_Average::BC_Average()
{

}

BC_Average::~BC_Average()
{

}

void BC_Average::Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_)
{
	int i, size_ = rx_array_.size();
	BC_.resize(size_);
	if (!size_)
	{
		cout << "The vector is empty!" << endl;
		return;
	}
	for (i = 0; i < size_; i++)
		BC_[i] = 1.0 / size_;
}