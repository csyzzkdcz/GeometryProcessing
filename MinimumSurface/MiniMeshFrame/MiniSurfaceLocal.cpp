#include"MiniSurfaceLocal.h"

MiniSurfaceLocal::MiniSurfaceLocal()
{
}

MiniSurfaceLocal::~MiniSurfaceLocal()
{
	if(tar_mesh_)
		delete(tar_mesh_);
}

void MiniSurfaceLocal::MiniSurface_Method()
{
	if (BC_Editor_ == NULL)
		return;
	HE_vert *cur_vert_,*neighbor_vert_;
	int i,j,num_ = init_vertices_.size();
	Vec3f cur_pos_;
	double sum_x_,sum_y_,sum_z_;
	float cur_x_,cur_y_,cur_z_;
	vector <size_t> neighbor_id_;
	vector<double> BC_weight_;
	final_vertices_.clear();
	final_vertices_.resize(num_);
	for(i=0;i<num_;i++)
	{
		if(is_boundary_[i])
		{
			final_vertices_[i](0) = init_vertices_[i](0);
			final_vertices_[i](1) = init_vertices_[i](1);
			final_vertices_[i](2) = init_vertices_[i](2);
		}
		else
		{
			sum_x_=0,sum_y_=0,sum_z_ = 0;
			neighbor_id_.clear();
			get_neighborId(i,neighbor_id_);
			int size_ = neighbor_id_.size();
			Calculate_BC(i, BC_weight_, neighbor_id_);
			for (int j = 0; j < size_; j++)
			{
				sum_x_ = sum_x_ + BC_weight_[j] * init_vertices_[neighbor_id_[j]](0);
				sum_y_ = sum_y_ + BC_weight_[j] * init_vertices_[neighbor_id_[j]](1);
				sum_z_ = sum_z_ + BC_weight_[j] * init_vertices_[neighbor_id_[j]](2);
			}
			final_vertices_[i](0) = init_vertices_[i](0) + REFINED_RATIO * (sum_x_ - init_vertices_[i](0));
			final_vertices_[i](1) = init_vertices_[i](1) + REFINED_RATIO * (sum_y_ - init_vertices_[i](1));
			final_vertices_[i](2) = init_vertices_[i](2) + REFINED_RATIO * (sum_z_ - init_vertices_[i](2));
		}
		Update_Surface();
	}
}