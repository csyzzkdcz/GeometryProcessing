#include"Parameterization_FixedSquare.h"

Parameterization_FixedSquare::Parameterization_FixedSquare()
{

}

Parameterization_FixedSquare::~Parameterization_FixedSquare()
{

}

void Parameterization_FixedSquare::Do_Parameterizing(vector<HE_vert*> &vertices_)
{

}

void Parameterization_FixedSquare::Mapping_Boundary_to_Square(vector<HE_vert*> vertices_)
{
	Get_Boundary_vertices(vertices_);
	int num_ = boundary_vertices_id_.size();
	int i;
	boundary_vertices_after_mapping_.resize(num_);
	int edge_num_ = num_ / 4;
	for (i = 0; i < edge_num_; i++)
	{
		boundary_vertices_after_mapping_[i][0] = 1.5 - i * 3.0 / edge_num_;
		boundary_vertices_after_mapping_[i][1] = 1.5;
		boundary_vertices_after_mapping_[i][2] = 0.0;
	}
	for (i = edge_num_; i < 2 * edge_num_; i++)
	{
		boundary_vertices_after_mapping_[i][0] = -1.5;
		boundary_vertices_after_mapping_[i][1] = 1.5 - (i-edge_num_) * 3.0 / edge_num_;
		boundary_vertices_after_mapping_[i][2] = 0.0;
	}
	for (i = 2 * edge_num_; i < 3 * edge_num_; i++)
	{
		boundary_vertices_after_mapping_[i][0] = -1.5 + (i - 2*edge_num_) * 3.0 / edge_num_;
		boundary_vertices_after_mapping_[i][1] = -1.5;
		boundary_vertices_after_mapping_[i][2] = 0.0;
	}
	for (i = 3 * edge_num_; i < num_; i++)
	{
		int size_ = num_ - 3 * edge_num_;
		boundary_vertices_after_mapping_[i][0] = 1.5;
		boundary_vertices_after_mapping_[i][1] = -1.5 + (i - 3 * edge_num_) * 3.0 / size_;
		boundary_vertices_after_mapping_[i][2] = 0.0;
	}
}