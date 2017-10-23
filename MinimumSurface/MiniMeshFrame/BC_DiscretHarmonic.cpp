#include"BC_DiscretHarmonic.h"
BC_DiscretHarmonic::BC_DiscretHarmonic()
{

}

BC_DiscretHarmonic::~BC_DiscretHarmonic()
{

}

void BC_DiscretHarmonic::Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_)
{
	int i, size_ = rx_array_.size();
	BC_.clear();
	BC_.resize(size_);
	double sum_ = 0;
	for (i = 0; i < size_; i++)
	{
		double cot_ri_ = calculate_cotangent(rx_array_[(i - 1 + size_) % size_], ry_array_[(i - 1 + size_) % size_], rz_array_[(i - 1 + size_) % size_], pos_x_, pos_y_, pos_z_, rx_array_[(i + 1) % size_], ry_array_[(i + 1) % size_], rz_array_[(i + 1) % size_]);
		double cot_bi_ = calculate_cotangent(rx_array_[(i + 1) % size_], ry_array_[(i + 1) % size_], rz_array_[(i + 1) % size_], pos_x_, pos_y_, pos_z_, rx_array_[(i - 1 + size_) % size_], ry_array_[(i - 1 + size_) % size_], rz_array_[(i - 1 + size_) % size_]);
		BC_[i] = cot_ri_ + cot_bi_;
		sum_ = sum_ + BC_[i];
	}
	for (i = 0; i < size_; i++)
		BC_[i] = BC_[i] / sum_;
}