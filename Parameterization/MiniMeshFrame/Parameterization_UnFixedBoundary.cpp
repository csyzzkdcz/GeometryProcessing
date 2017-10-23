#include"Parameterization_UnFixedBoundary.h"

Parameterization_UnFixedBoundary::Parameterization_UnFixedBoundary()
{

}

Parameterization_UnFixedBoundary::~Parameterization_UnFixedBoundary()
{

}

void Parameterization_UnFixedBoundary::Do_Parameterizing(vector<HE_vert*> &vertices_)
{

}

void Parameterization_UnFixedBoundary::Mapping_tir3D_to_tri2D(int tri_index_, point &vertex_1_, point &vertex_2_, point &vertex_3_)
{
	point p1, p2, p3;
	float d12, d23, d31;
	p1 = vertices_record_[tmpt_mesh_->get_face(tri_index_)->pedge_->ppair_->pvert_->id_];
	p2 = vertices_record_[tmpt_mesh_->get_face(tri_index_)->pedge_->pvert_->id_];
	p3 = vertices_record_[tmpt_mesh_->get_face(tri_index_)->pedge_->pnext_->pvert_->id_];
	d12 = Get_Distance(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	d23 = Get_Distance(p2[0], p2[1], p2[2], p3[0], p3[1], p3[2]);
	d31 = Get_Distance(p3[0], p3[1], p3[2], p1[0], p1[1], p1[2]);
	vertex_1_[0] = vertex_1_[1] = vertex_1_[2] = 0.0;
	vertex_2_[0] = d12;
	vertex_2_[1] = vertex_2_[2] = 0.0;
	vertex_3_[0] = (d31*d31 + d12*d12 - d23*d23) / (2 * d12);
	vertex_3_[1] = sqrt(d31*d31 - vertex_3_[0] * vertex_3_[0]);
	vertex_3_[2] = 0.0;
}

void Parameterization_UnFixedBoundary::Initial_Parameterization(vector<HE_vert*> &vertices_)
{
	Parameterization_FS_SP* Editor_;
	Editor_ = new Parameterization_FS_SP();
	Editor_->Do_Parameterizing(vertices_);
}

void Parameterization_UnFixedBoundary::Generate_Initial_Coor()
{
	int i, size_ = tmpt_mesh_->num_of_face_list();
	vertices_1_.clear();
	vertices_2_.clear();
	vertices_3_.clear();
	vertices_1_.resize(size_);
	vertices_2_.resize(size_);
	vertices_3_.resize(size_);
	for (i = 0; i < size_; i++)
		Mapping_tir3D_to_tri2D(i, vertices_1_[i], vertices_2_[i], vertices_3_[i]);
}

float Parameterization_UnFixedBoundary::Get_cot(int tri_index_, int index_)
{
	point p1, p2, p3;
	int k = (index_ + 2) % 3;
	if (k == 0)
	{
		p1 = vertices_1_[tri_index_];
		p2 = vertices_2_[tri_index_];
		p3 = vertices_3_[tri_index_];
	}
	else if (k == 1)
	{
		p1 = vertices_2_[tri_index_];
		p2 = vertices_3_[tri_index_];
		p3 = vertices_1_[tri_index_];
	}
	else
	{
		p1 = vertices_3_[tri_index_];
		p2 = vertices_1_[tri_index_];
		p3 = vertices_2_[tri_index_];
	}
	float d12, d23, d31;
	d12 = Get_Distance(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	d23 = Get_Distance(p2[0], p2[1], p2[2], p3[0], p3[1], p3[2]);
	d31 = Get_Distance(p3[0], p3[1], p3[2], p1[0], p1[1], p1[2]);
	float cos_ = (d12*d12 + d31*d31 - d23*d23) / (2 * d12*d31);
	return cos_ / sqrt(1 - cos_*cos_);
}

float Parameterization_UnFixedBoundary::Get_cot(point p1, point p2, point p3)
{
	float d12, d23, d31;
	d12 = Get_Distance(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	d23 = Get_Distance(p2[0], p2[1], p2[2], p3[0], p3[1], p3[2]);
	d31 = Get_Distance(p3[0], p3[1], p3[2], p1[0], p1[1], p1[2]);
	float cos_ = (d12*d12 + d31*d31 - d23*d23) / (2 * d12*d31);
	return cos_ / sqrt(1 - cos_*cos_);
}

int  Parameterization_UnFixedBoundary::Location_Vertex(int face_index_, HE_vert* vertex_)
{
	int pos_index_ = 0;
	HE_edge* tmpt_edge_ = tmpt_mesh_->get_face(face_index_)->pedge_;
	for (; pos_index_ < 3 && vertex_->id_ != tmpt_edge_->ppair_->pvert_->id_;)
	{
		pos_index_++;
		tmpt_edge_ = tmpt_edge_->pnext_;
	}
	return pos_index_;
}

Vector2f  Parameterization_UnFixedBoundary::Get_X(int pos_index_, int face_index_)
{
	Vector2f vec_;
	if (pos_index_ == 0)
	{
		vec_ << vertices_1_[face_index_][0],
			vertices_1_[face_index_][1];
	}
	else if (pos_index_ == 1)
	{
		vec_ << vertices_2_[face_index_][0],
			vertices_2_[face_index_][1];
	}
	else if (pos_index_ == 2)
	{
		vec_ << vertices_3_[face_index_][0],
			vertices_3_[face_index_][1];
	}
	return vec_;
}

void Parameterization_UnFixedBoundary::Global_Phase(vector<HE_vert*> vertices_)
// Store the coefficient matrix A and predecomposite it
{
	int i, j, size_ = vertices_.size();
	A = SparseMatrix<float>(size_, size_);
	vector<Eigen::Triplet<float> > triplets_;
	//Generate 
	for (i = 0; i < size_; i++)
	{
		HE_edge* cur_edge_ = vertices_[i]->pedge_;
		float sum_i_ = 0;
		for (j = 0; j < vertices_[i]->degree_; j++)
		{
			int index_ = cur_edge_->pvert_->id_;
			float cot_ij_ = 0, cot_ji_ = 0;
			if (cur_edge_->pface_)
				cot_ij_ = Get_cot(vertices_record_[cur_edge_->pnext_->pvert_->id_], vertices_record_[i], vertices_record_[cur_edge_->pvert_->id_]);
			if (cur_edge_->ppair_->pface_)
				cot_ji_ = Get_cot(vertices_record_[cur_edge_->ppair_->pnext_->pvert_->id_], vertices_record_[i], vertices_record_[cur_edge_->pvert_->id_]);
			triplets_.push_back(Eigen::Triplet<float >(i, index_, -(cot_ij_ + cot_ji_)));
			sum_i_ = sum_i_ + cot_ij_ + cot_ji_;
			cur_edge_ = cur_edge_->ppair_->pnext_;
		}
		triplets_.push_back(Eigen::Triplet<float>(i, i, sum_i_));
	}
	A.setFromTriplets(triplets_.begin(), triplets_.end());
	A.makeCompressed();
	chol_.compute(A);
}

