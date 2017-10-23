#include"Parameterization.h"

Parameterization::Parameterization()
{

}

Parameterization::~Parameterization()
{

}

void Parameterization::Do_Parameterizing(vector<HE_vert*> &vertices_)
{

}

float Parameterization::Get_Distance(float start_rx_, float start_ry_, float start_rz_, float end_rx_, float end_ry_, float end_rz_)
{
	return sqrt((start_rx_ - end_rx_)*(start_rx_ - end_rx_) + (start_ry_ - end_ry_)*(start_ry_ - end_ry_) + (start_rz_ - end_rz_)*(start_rz_ - end_rz_));
}