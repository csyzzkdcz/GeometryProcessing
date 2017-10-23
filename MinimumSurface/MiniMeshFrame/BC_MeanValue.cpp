#include"BC_MeanValue.h"
BC_MeanValue::BC_MeanValue()
{
}

BC_MeanValue::~BC_MeanValue()
{
}

void BC_MeanValue::Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_)
{
	int i, size_ = rx_array_.size();
	double sum_ = 0;
	BC_.clear();
	BC_.resize(size_);
	for (i = 0; i < size_; i++)
	{
		double dist_i_ = dist(pos_x_,pos_y_,pos_z_, rx_array_[i],ry_array_[i],rz_array_[i]);
		double half_tan_ri_ = calculate_half_tangent(pos_x_, pos_y_, pos_z_, rx_array_[i], ry_array_[i], rz_array_[i], rx_array_[(i - 1 + size_) % size_], ry_array_[(i - 1 + size_) % size_], rz_array_[(i - 1 + size_) % size_]);
		double half_tan_bi_ = calculate_half_tangent(pos_x_, pos_y_, pos_z_, rx_array_[i], ry_array_[i], rz_array_[i], rx_array_[(i + 1) % size_], ry_array_[(i + 1) % size_], rz_array_[(i + 1) % size_]);
		BC_[i] = (half_tan_ri_ + half_tan_bi_) / dist_i_;
		sum_ = sum_ + BC_[i];
	}
	for (i = 0; i < size_; i++)
		BC_[i] = BC_[i] / sum_;
}