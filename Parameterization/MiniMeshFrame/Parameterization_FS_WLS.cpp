#include"Parameterization_FS_WLS.h"
Parameterization_FS_WLS::Parameterization_FS_WLS()
{

}

Parameterization_FS_WLS::~Parameterization_FS_WLS()
{

}



void Parameterization_FS_WLS::Do_Parameterizing(vector<HE_vert*> &vertices_)
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
void Parameterization_FS_WLS::Generate_CoefMatrix(vector<HE_vert* > vertices_)
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
			for (int j = 0; j < neighbor_num_; j++)
			{
				float dist_ = Get_Distance(vertices_[i]->position_[0], vertices_[i]->position_[1], vertices_[i]->position_[2], vertices_[vertices_[i]->neighborIdx[j]]->position_[0], vertices_[vertices_[i]->neighborIdx[j]]->position_[1], vertices_[vertices_[i]->neighborIdx[j]]->position_[2]);
				sum_ = sum_ + 1.0 / dist_;
				triplets_.push_back(Eigen::Triplet<float >(i, vertices_[i]->neighborIdx[j], -1.0 / dist_));
			}
			triplets_.push_back(Eigen::Triplet<float >(i, i, sum_));
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