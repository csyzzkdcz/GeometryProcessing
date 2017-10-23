#include"Parameterization_FixedCircle.h"

Parameterization_FixedCircle::Parameterization_FixedCircle()
{

}

Parameterization_FixedCircle::~Parameterization_FixedCircle()
{

}

void Parameterization_FixedCircle::Do_Parameterizing(vector<HE_vert*> &vertices_)
{

}

void Parameterization_FixedCircle::Mapping_Boundary_to_Circle(vector<HE_vert*> vertices_)
{
	Get_Boundary_vertices(vertices_);
	int num_ = boundary_vertices_id_.size();
	int i;
	boundary_vertices_after_mapping_.resize(num_);
	for (i = 0; i < num_; i++)
	{
		boundary_vertices_after_mapping_[i][0] = RADIUS * cos(2 * i * PI / num_);
		boundary_vertices_after_mapping_[i][1] = RADIUS * sin(2 * i * PI / num_);
		boundary_vertices_after_mapping_[i][2] = 0.0;
	}
}