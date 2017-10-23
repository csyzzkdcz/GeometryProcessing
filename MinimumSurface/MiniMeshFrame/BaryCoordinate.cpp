#include"BaryCoordinate.h"
#define eps_ 1e-6
BaryCoordinate::BaryCoordinate()
{

}
BaryCoordinate::~BaryCoordinate()
{

}

void BaryCoordinate::Calculate_Coodinate(double pos_x_, double pos_y_, double pos_z_, vector<double> &BC_, vector<double> rx_array_, vector<double> ry_array_, vector<double> rz_array_)
{

}

int BaryCoordinate::Near_Int(double x)
{
	int  y = int(x);
	if (x - y>0.5)
		y = y + 1;
	return y;
}
double BaryCoordinate::dist(double point_start_x, double point_start_y, double point_start_z, double point_end_x, double point_end_y, double point_end_z)
// Calculate the distance from point_start_ to point_end_
{
	double dist_ = sqrt(pow(point_end_x - point_start_x, 2.0) + pow(point_end_y - point_start_y, 2.0) + pow(point_end_z - point_start_z, 2.0));
	return dist_ < eps_ ? eps_ : dist_;
}

double BaryCoordinate::calculate_cosin(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z)
// calculate the cosin of <P3P1,P1P2>
{
	double dist_12_, dist_23_, dist_31_;
	dist_12_ = dist(P1_x, P1_y, P1_z, P2_x, P2_y, P2_z);
	dist_23_ = dist(P2_x, P2_y, P2_z, P3_x, P3_y, P3_z);
	dist_31_ = dist(P3_x, P3_y, P3_z, P1_x, P1_y, P1_z);
	if (dist_12_ <= eps_ || dist_31_ <= eps_)
		return eps_;
	if (dist_23_ <= eps_)
		return 1 - eps_;
	double cos_ = (dist_31_ * dist_31_ + dist_12_ * dist_12_ - dist_23_ * dist_23_) / (2 * dist_12_ * dist_31_);
	if (0 < cos_ && cos_< eps_)
		cos_ = eps_;
	if (-eps_ < cos_ && cos_ < 0)
		cos_ = -eps_;
	if (abs(1 - cos_) < eps_)
		cos_ = 1 - eps_;
	if (abs(1 + cos_) < eps_)
		cos_ = -1 + eps_;
	return cos_;
}

double BaryCoordinate::calculate_half_tangent(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z)
// calculate the tan of 0.5*<P3P1,P1P2>
{
	double cos_ = calculate_cosin(P1_x, P1_y, P1_z, P2_x, P2_y, P2_z, P3_x, P3_y, P3_z);
	return sqrt((1 - cos_) / (1 + cos_));
}


double BaryCoordinate::calculate_cotangent(double P1_x, double P1_y, double P1_z, double P2_x, double P2_y, double P2_z, double P3_x, double P3_y, double P3_z)
// calculate the cotan of <P3P1,P1P2>
{
	double cos_ = calculate_cosin(P1_x, P1_y, P1_z, P2_x, P2_y, P2_z, P3_x, P3_y, P3_z);
	return cos_ / sqrt(1 - cos_*cos_);
}