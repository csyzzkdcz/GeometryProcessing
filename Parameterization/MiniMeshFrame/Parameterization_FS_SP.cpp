#include"Parameterization_FS_SP.h"
Parameterization_FS_SP::Parameterization_FS_SP()
{

}


Parameterization_FS_SP::~Parameterization_FS_SP()
{

}

void Parameterization_FS_SP::Shape_Preserve_Mapping(HE_vert* vertex_, vector<float> &neighbor_rx_, vector<float> &neighbor_ry_)
{
	neighbor_rx_.clear();
	neighbor_ry_.clear();
	float tmpt_angle_ = 0, cur_angle_, sum_angle_ = 0;
	vector<float> angles_array_;
	int size_ = vertex_->degree_;
	int i;
	float dist1_, dist2_, dist3_, dist_;
	HE_edge *cur_edge_ = vertex_->pedge_;
	for (i = 0; i < size_; i++)
	{
		dist1_ = Get_Distance(vertex_->position_[0], vertex_->position_[1], vertex_->position_[2], cur_edge_->pvert_->position_[0], cur_edge_->pvert_->position_[1], cur_edge_->pvert_->position_[2]);
		dist2_ = Get_Distance(vertex_->position_[0], vertex_->position_[1], vertex_->position_[2], cur_edge_->ppair_->pnext_->pvert_->position_[0], cur_edge_->ppair_->pnext_->pvert_->position_[1], cur_edge_->ppair_->pnext_->pvert_->position_[2]);
		dist3_ = Get_Distance(cur_edge_->pvert_->position_[0], cur_edge_->pvert_->position_[1], cur_edge_->pvert_->position_[2], cur_edge_->ppair_->pnext_->pvert_->position_[0], cur_edge_->ppair_->pnext_->pvert_->position_[1], cur_edge_->ppair_->pnext_->pvert_->position_[2]);
		cur_angle_ = acos((dist1_*dist1_ + dist2_*dist2_ - dist3_*dist3_) / (2 * dist1_*dist2_));
		angles_array_.push_back(cur_angle_);
		cur_edge_ = cur_edge_->ppair_->pnext_;
		sum_angle_ = sum_angle_ + cur_angle_;
	}
	cur_edge_ = vertex_->pedge_;
	for (i = 0; i < size_; i++)
	{
		dist_ = Get_Distance(vertex_->position_[0], vertex_->position_[1], vertex_->position_[2], cur_edge_->pvert_->position_[0], cur_edge_->pvert_->position_[1], cur_edge_->pvert_->position_[2]);
		float x = cos(0.0);
		float rx_ = dist_ * cos(tmpt_angle_ / sum_angle_ * 2 * PI);
		float ry_ = dist_ * sin(tmpt_angle_ / sum_angle_ * 2 * PI);
		neighbor_rx_.push_back(rx_);
		neighbor_ry_.push_back(ry_);
		tmpt_angle_ = tmpt_angle_ + angles_array_[i];
		cur_edge_ = cur_edge_->ppair_->pnext_;
	}

}


float Parameterization_FS_SP::Tri_Area(float rx_1_, float ry_1_, float rz_1_, float rx_2_, float ry_2_, float rz_2_, float rx_3_, float ry_3_, float rz_3_)
{
	float d1_, d2_, d3_, p;
	d1_ = Get_Distance(rx_1_, ry_1_, rz_1_, rx_2_, ry_2_, rz_2_);
	d2_ = Get_Distance(rx_1_, ry_1_, rz_1_, rx_3_, ry_3_, rz_3_);
	d3_ = Get_Distance(rx_3_, ry_3_, rz_3_, rx_2_, ry_2_, rz_2_);
	p = (d1_ + d2_ + d3_) / 2;
	return sqrt(p*(p - d1_)*(p - d2_)*(p - d3_));
}

void Parameterization_FS_SP::Get_SP_Barycentry(HE_vert* vertex_, vector<float> &weight_)
{
	vector<float> neighbor_rx_, neighbor_ry_;
	Shape_Preserve_Mapping(vertex_, neighbor_rx_, neighbor_ry_);
	int i, j, size_ = neighbor_rx_.size();
	weight_.clear();
	weight_.resize(size_);
	MatrixXd weight_mat_(size_, size_);
	weight_mat_.setZero();
	for (i = 0; i < size_; i++)
	{
		for (j = i + 1; j < i + size_ - 1; j++)
		{
			/*float area_ = Tri_Area(neighbor_rx_[i], neighbor_ry_[i], 0, neighbor_rx_[j%size_], neighbor_ry_[j%size_], 0, neighbor_rx_[(j + 1) % size_], neighbor_ry_[(j + 1) % size_], 0);
			float area_1_ = Tri_Area(0,0, 0, neighbor_rx_[j%size_], neighbor_ry_[j%size_], 0, neighbor_rx_[(j + 1) % size_], neighbor_ry_[(j + 1) % size_], 0);
			float area_2_ = Tri_Area(neighbor_rx_[i], neighbor_ry_[i], 0, 0, 0, 0, neighbor_rx_[(j + 1) % size_], neighbor_ry_[(j + 1) % size_], 0);
			float area_3_ = Tri_Area(neighbor_rx_[i], neighbor_ry_[i], 0, neighbor_rx_[j%size_], neighbor_ry_[j%size_], 0, 0 ,0 , 0);
			if (area_-area_1_-area_2_- area_3_ >-EPS)
			{
			weight_mat_(i, i) = area_1_ / area_;
			weight_mat_(i, j%size_) = area_2_ / area_;
			weight_mat_(i, (j + 1) % size_) = area_3_ / area_;
			}*/
			MatrixXf A(3, 3);
			Vector3f coef_vec_;
			coef_vec_(0) = 0.0;
			coef_vec_(1) = 0.0;
			coef_vec_(2) = 1.0;
			A(2, 0) = A(2, 1) = A(2, 2) = 1.0;
			A(0, 0) = neighbor_rx_[i];
			A(1, 0) = neighbor_ry_[i];
			A(0, 1) = neighbor_rx_[j%size_];
			A(1, 1) = neighbor_ry_[j%size_];
			A(0, 2) = neighbor_rx_[(j + 1) % size_];
			A(1, 2) = neighbor_ry_[(j + 1) % size_];
			Vector3f sol_ = A.partialPivLu().solve(coef_vec_);
			if (sol_(0) > -EPS && sol_(1) > -EPS && sol_(2) > -EPS)
			{
				weight_mat_(i, i) = sol_(0);
				weight_mat_(i, j%size_) = sol_(1);
				weight_mat_(i, (j + 1) % size_) = sol_(2);
			}
		}
	}
	for (j = 0; j < size_; j++)
	{
		double sum_ = 0;
		for (i = 0; i < size_; i++)
		{
			sum_ = sum_ + weight_mat_(i, j);
		}
		weight_[j] = 1.0 / size_ * sum_;
		//weight_[j] = sum_;
	}
}

void Parameterization_FS_SP::Generate_CoefMatrix(vector<HE_vert* > vertices_)
{
	Mapping_Boundary_to_Square(vertices_);
	int num_ = vertices_.size();
	int i;
	SparseMatrix<float> Coef_Matrix_(num_, num_);
	vector<Eigen::Triplet<float> > triplets_;

	// Generate the coeffient matrix in AX = b
	for (i = 0; i < num_; i++)
	{
		if (vertices_[i]->isOnBoundary())
			triplets_.push_back(Eigen::Triplet<float >(i, i, 1));
		else
		{
			int neighbor_num_ = vertices_[i]->neighborIdx.size();
			float sum_ = 0;
			vector<float> weight_;
			Get_SP_Barycentry(vertices_[i], weight_);
			HE_edge *cur_edge_ = vertices_[i]->pedge_;
			for (int j = 0; j < neighbor_num_; j++)
			{
				triplets_.push_back(Eigen::Triplet<float >(i, cur_edge_->pvert_->id_, -weight_[j]));
				cur_edge_ = cur_edge_->ppair_->pnext_;
			}
			triplets_.push_back(Eigen::Triplet<float >(i, i, 1));
		}
	}
	Coef_Matrix_.setFromTriplets(triplets_.begin(), triplets_.end());
	Coef_Matrix_.makeCompressed();
	LU.compute(Coef_Matrix_);

	// Generate b in AX = b
	b.setZero(num_, 3);
	for (i = 0; i < boundary_vertices_id_.size(); i++)
	{
		b(boundary_vertices_id_[i], 0) = boundary_vertices_after_mapping_[i][0];
		b(boundary_vertices_id_[i], 1) = boundary_vertices_after_mapping_[i][1];
		b(boundary_vertices_id_[i], 2) = boundary_vertices_after_mapping_[i][2];
	}
}

void Parameterization_FS_SP::Do_Parameterizing(vector<HE_vert*> &vertices_)
{
	int num_ = vertices_.size();
	MatrixXf sol_(num_, 3);
	Generate_CoefMatrix(vertices_);
	sol_ = LU.solve(b);
	for (int i = 0; i < num_; i++)
	{
		Vec3f vertex_(sol_(i, 0), sol_(i, 1), sol_(i, 2));
		vertices_[i]->position_ = vertex_;
	}
}