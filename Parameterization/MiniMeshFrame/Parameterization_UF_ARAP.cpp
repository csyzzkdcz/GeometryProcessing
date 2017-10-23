#include "Parameterization_UF_ARAP.h"
Parameterization_ARAP::Parameterization_ARAP()
{
	tmpt_mesh_ = NULL;
}

Parameterization_ARAP::Parameterization_ARAP(Mesh3D *ptr_mesh_)
{
	tmpt_mesh_ = ptr_mesh_;
	vertices_record_.clear();
	for (int i = 0; i < ptr_mesh_->num_of_vertex_list(); i++)
	{
		vertices_record_.push_back(ptr_mesh_->get_vertex(i)->position_);
	}
	Initial_Parameterization(*(tmpt_mesh_->get_vertex_list()));
	Generate_Initial_Coor();
	Global_Phase(*(tmpt_mesh_->get_vertex_list()));
}

Parameterization_ARAP::~Parameterization_ARAP()
{
	if (tmpt_mesh_)
		delete(tmpt_mesh_);
	vertices_record_.clear();
	vertices_1_.clear();
	vertices_2_.clear();
	vertices_3_.clear();
	Lt_array_.clear();
}


Matrix2f Parameterization_ARAP::Get_Lt(int tri_index_)
{
	Matrix2f Lt_,St_;
	St_.setZero();
	Lt_.setZero();
	MatrixXf ut_, xt_;
	ut_.setZero(2, 3);
	xt_.setZero(2, 3);
	xt_(0, 0) = vertices_1_[tri_index_][0];
	xt_(1, 0) = vertices_1_[tri_index_][1];
	xt_(0, 1) = vertices_2_[tri_index_][0];
	xt_(1, 1) = vertices_2_[tri_index_][1];
	xt_(0, 2) = vertices_3_[tri_index_][0];
	xt_(1, 2) = vertices_3_[tri_index_][1];
	ut_(0, 0) = tmpt_mesh_->get_face(tri_index_)->pedge_->ppair_->pvert_->position_[0];
	ut_(1, 0) = tmpt_mesh_->get_face(tri_index_)->pedge_->ppair_->pvert_->position_[1];
	ut_(0, 1) = tmpt_mesh_->get_face(tri_index_)->pedge_->pvert_->position_[0];
	ut_(1, 1) = tmpt_mesh_->get_face(tri_index_)->pedge_->pvert_->position_[1];
	ut_(0, 2) = tmpt_mesh_->get_face(tri_index_)->pedge_->pnext_->pvert_->position_[0];
	ut_(1, 2) = tmpt_mesh_->get_face(tri_index_)->pedge_->pnext_->pvert_->position_[1];
	for (int i = 0; i <= 2; i++)
	{
		MatrixXf delta_u_(2,1);
		MatrixXf delta_x_(2,1); 
		for (int j = 0; j < 2; j++)
		{
			delta_u_(j,0) = ut_(j, i % 3) - ut_(j, (i + 1) % 3);
			delta_x_(j,0) = xt_(j, i % 3) - xt_(j, (i + 1) % 3);
		}
		St_ = St_ + Get_cot(tri_index_, i)* delta_u_ * delta_x_.transpose();
	}
	JacobiSVD<MatrixXf> svd(St_, ComputeThinU | ComputeThinV);
	Lt_ = svd.matrixU()*svd.matrixV().transpose();
	return Lt_;

}


void Parameterization_ARAP::Local_Phase()
{
	int i, size_ = tmpt_mesh_->num_of_face_list();
	Lt_array_.clear();
	Lt_array_.resize(size_);
	for (i = 0; i < size_; i++)
	{
		Lt_array_[i] = Get_Lt(i);
	}
}


void Parameterization_ARAP::Do_Parameterizing(vector<HE_vert*> &vertices_)
{
	float max_error_ = INF;
	for (int itr_time_ = 0; itr_time_ < MAX_ITR_NUM && max_error_ > MIN_ERROR; itr_time_++)
	{
		Local_Phase();
		int i, j, size_ = vertices_.size();
		int index_ij_, index_ji_, pos_i_, pos_j_;
		float cot_ij_, cot_ji_;
		Vector2f x_i_;
		Vector2f x_j_;
		Vector2f tmpt_result_;
		MatrixXf rhs_vector_(size_, 2);
		MatrixXf sol_(size_, 2);
		rhs_vector_.setZero();
		//Generate 
		for (i = 0; i < size_; i++)
		{
			HE_edge* cur_edge_ = vertices_[i]->pedge_;
			float sum_i_ = 0;
			for (j = 0; j < vertices_[i]->degree_; j++)
			{
				tmpt_result_.setZero();
				if (cur_edge_->pface_)
				{
					index_ij_ = cur_edge_->pface_->id_;
					cot_ij_ = Get_cot(vertices_record_[cur_edge_->pnext_->pvert_->id_], vertices_record_[i], vertices_record_[cur_edge_->pvert_->id_]);
					pos_i_ = Location_Vertex(index_ij_, vertices_[i]);
					pos_j_ = Location_Vertex(index_ij_, cur_edge_->pvert_);
					x_i_ = Get_X(pos_i_, index_ij_);
					x_j_ = Get_X(pos_j_, index_ij_);
					tmpt_result_ = cot_ij_ * Lt_array_[index_ij_] * (x_i_ - x_j_);
				}
				if (cur_edge_->ppair_->pface_)
				{
					index_ji_ = cur_edge_->ppair_->pface_->id_;
					cot_ji_ = Get_cot(vertices_record_[cur_edge_->ppair_->pnext_->pvert_->id_], vertices_record_[i], vertices_record_[cur_edge_->pvert_->id_]);
					pos_i_ = Location_Vertex(index_ji_, vertices_[i]);
					pos_j_ = Location_Vertex(index_ji_, cur_edge_->pvert_);
					x_i_ = Get_X(pos_i_, index_ji_);
					x_j_ = Get_X(pos_j_, index_ji_);
					tmpt_result_ = tmpt_result_ + cot_ji_ * Lt_array_[index_ji_] * (x_i_ - x_j_);
				}
				rhs_vector_(i, 0) = tmpt_result_(0) + rhs_vector_(i, 0);
				rhs_vector_(i, 1) = tmpt_result_(1) + rhs_vector_(i, 1);
				cur_edge_ = cur_edge_->ppair_->pnext_;
			}

		}
		sol_ = chol_.solve(rhs_vector_);
		max_error_ = Get_Distance(sol_(0, 0), sol_(0, 1), 0.0, vertices_[0]->position_[0], vertices_[0]->position_[1], vertices_[0]->position_[2]);
		for (i = 1; i < size_; i++)
		{
			float tmpt_ = Get_Distance(sol_(i, 0), sol_(i, 1), 0.0, vertices_[i]->position_[0], vertices_[i]->position_[1], vertices_[i]->position_[2]);
			if (tmpt_ > max_error_)
				max_error_ = tmpt_;
		}
		for (i = 0; i < size_; i++)
		{
			vertices_[i]->position_[0] = sol_(i, 0);
			vertices_[i]->position_[1] = sol_(i, 1);
			vertices_[i]->position_[2] = 0.0;
		}
	}
	tmpt_mesh_ = NULL;
}