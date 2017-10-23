#include"MiniSurfaceGlobal.h"

MiniSurfaceGlobal::MiniSurfaceGlobal()
{
}

MiniSurfaceGlobal::~MiniSurfaceGlobal()
{
	if(tar_mesh_)
		delete(tar_mesh_);
}

void MiniSurfaceGlobal::MiniSurface_Method()
{
	if (BC_Editor_ == NULL)
		return;
	int i,num_ = tar_mesh_->num_of_vertex_list();
	SparseMatrix<float> Coef_Matrix_(num_,num_);
	vector<Eigen::Triplet<float> > triplets_;
	MatrixXf b,sol_(num_,3);
	b.setZero(num_,3);
	vector<size_t> neighbor_;
	vector<double> BC_weight_;
	for(int i=0;i<num_;i++)
	{
		if(is_boundary_[i])
		{
			triplets_.push_back(Eigen::Triplet<float>(i, i, 1));
			b(i,0) = init_vertices_[i](0);
			b(i,1) = init_vertices_[i](1);
			b(i,2) = init_vertices_[i](2);
		}
		else
		{
			neighbor_.clear();
			get_neighborId(i,neighbor_);
			int size_ = neighbor_.size();
			Calculate_BC(i, BC_weight_, neighbor_);
			for(int j=0;j<size_;j++)
			{
				triplets_.push_back(Eigen::Triplet<float>(i, neighbor_[j], -BC_weight_[j]));
			}
			triplets_.push_back(Eigen::Triplet<float > (i,i,1));
		}
	}
	Coef_Matrix_.setFromTriplets(triplets_.begin(),triplets_.end());
	Coef_Matrix_.makeCompressed();
	LU.compute(Coef_Matrix_);
	sol_ = LU.solve(b);
	final_vertices_.clear();
	final_vertices_.resize(num_);
	for(int i=0;i<num_;i++)
	{
		final_vertices_[i](0) = sol_(i,0);
		final_vertices_[i](1) = sol_(i,1);
		final_vertices_[i](2) = sol_(i,2);
	}
	Update_Surface();
}
