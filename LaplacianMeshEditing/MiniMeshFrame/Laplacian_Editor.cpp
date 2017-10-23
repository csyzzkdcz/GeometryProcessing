#include "Laplacian_Editor.h"

Laplacian_Editor::Laplacian_Editor()
{
	is_initial_ = false;
	vertices_num_ = fixed_num_ = handle_num_ = 0;

	Lx_array_.clear();
	Ly_array_.clear();
	Lz_array_.clear();

	fixed_id_array_.clear();
	handle_id_array_.clear();

	init_vertices_.clear();
}

Laplacian_Editor::~Laplacian_Editor()
{
	is_initial_ = false;
	vertices_num_ = fixed_num_ = handle_num_ = 0;

	Lx_array_.clear();
	Ly_array_.clear();
	Lz_array_.clear();

	fixed_id_array_.clear();
	handle_id_array_.clear();

	init_vertices_.clear();
}

void Laplacian_Editor::Init(vector<HE_vert*> init_vertices, vector<int> fixed_id_array, vector<int> handle_id_array)
// initialization
{
	if (is_initial_)
		return;
	vertices_num_ = init_vertices.size();
	fixed_num_ = fixed_id_array.size();
	handle_num_ = handle_id_array.size();
	init_vertices_.resize(vertices_num_);
	fixed_id_array_.resize(fixed_num_);
	handle_id_array_.resize(handle_num_);
	for (int i = 0; i < vertices_num_; i++)
		init_vertices_[i] = init_vertices[i];
	for (int i = 0; i < fixed_num_; i++)
		fixed_id_array_[i] = fixed_id_array[i];
	for (int i = 0; i < handle_num_; i++)
		handle_id_array_[i] = handle_id_array[i];
	Generate_Laplacian_Coord();
	Generate_Coef_Matrix();
	Predecomposite_Matrix();
	is_initial_ = true;
}

void Laplacian_Editor::Generate_Laplacian_Coord()
// generate Laplacian coordinates
{
	Lx_array_.clear();
	Ly_array_.clear();
	Lz_array_.clear();
	Lx_array_.resize(vertices_num_);
	Ly_array_.resize(vertices_num_);
	Lz_array_.resize(vertices_num_);
	int i,j;
	float temp_x_, temp_y_, temp_z_;
	for (i = 0; i < vertices_num_; i++)
	{
		temp_x_ = temp_y_ = temp_z_ = 0;
		int degree_i_ = init_vertices_[i]->degree_;
		for (j = 0; j < degree_i_; j++)
		{
			temp_x_ = temp_x_ + init_vertices_[init_vertices_[i]->neighborIdx[j]]->position_[0];
			temp_y_ = temp_y_ + init_vertices_[init_vertices_[i]->neighborIdx[j]]->position_[1];
			temp_z_ = temp_z_ + init_vertices_[init_vertices_[i]->neighborIdx[j]]->position_[2];
		}
		temp_x_ = init_vertices_[i]->position_[0] - 1.0 / degree_i_ * temp_x_;
		temp_y_ = init_vertices_[i]->position_[1] - 1.0 / degree_i_ * temp_y_;
		temp_z_ = init_vertices_[i]->position_[2] - 1.0 / degree_i_ * temp_z_;
		Lx_array_[i] = temp_x_;
		Ly_array_[i] = temp_y_;
		Lz_array_[i] = temp_z_;
	}

}

void Laplacian_Editor::Generate_Coef_Matrix()
// generate the coeffficient matrix
{
	int i, j;
	int rows_ = 3*(vertices_num_ + fixed_num_ + handle_num_);
	int cols_ = 3*vertices_num_;
	coef_mat_ = SparseMatrix<float>(rows_, cols_);
	vector<Eigen::Triplet<float> > triplets_;
	HE_vert* tmpt_vert_;
	for (i = 0; i < vertices_num_; i++)
	{
		int degree_i_ = init_vertices_[i]->degree_;
		MatrixXf tmpt_mat_i_(3,7);
		tmpt_mat_i_.setZero();

		// initialize this matrix
		/*			 Lx_i,	0,		Lz_i,	-Ly_i,	0,	0,	0
		tmmt_mat_i_ = Ly_i,	-Lz_i	0,		Lx_i,	0,	0,	0, we do not need to consider the translation vector, because the forth element of every delta_i is 0
					 Lz_i,	Ly_i,	-Lx_i,	0,		0,	0,	0
		*/
		tmpt_mat_i_(0, 0) = Lx_array_[i];
		tmpt_mat_i_(0, 2) = Lz_array_[i];
		tmpt_mat_i_(0, 3) = -Ly_array_[i];

		tmpt_mat_i_(1, 0) = Ly_array_[i];
		tmpt_mat_i_(1, 1) = -Lz_array_[i];
		tmpt_mat_i_(1, 3) = Lx_array_[i];

		tmpt_mat_i_(2, 0) = Lz_array_[i];
		tmpt_mat_i_(2, 1) = Ly_array_[i];
		tmpt_mat_i_(2, 2) = -Lx_array_[i];

		MatrixXf A_i(3 * degree_i_ + 3,7);
		/*				Vx,	0,		Vz,		-Vy,		1,	0,	0
		A_i_(3*j,7) =	Vy,	-Vz,		0,		Vx,		0,	1,	0
						Vz,	Vy,		-Vx,		0,		0,	0,	1
		*/
		A_i.setZero();
		A_i.block(0, 0, 3, 7) << init_vertices_[i]->position_[0], 0, init_vertices_[i]->position_[2], -init_vertices_[i]->position_[1], 1, 0, 0,
								init_vertices_[i]->position_[1], -init_vertices_[i]->position_[2], 0, init_vertices_[i]->position_[0], 0, 1, 0,
								init_vertices_[i]->position_[2], init_vertices_[i]->position_[1], -init_vertices_[i]->position_[0], 0, 0, 0, 1;
		for (j = 0; j < degree_i_; j++)
		{
			tmpt_vert_ = init_vertices_[init_vertices_[i]->neighborIdx[j]];
			A_i.block(3 * (j + 1), 0, 3, 7) << tmpt_vert_->position_[0], 0, tmpt_vert_->position_[2], -tmpt_vert_->position_[1], 1, 0, 0,
											 tmpt_vert_->position_[1], -tmpt_vert_->position_[2], 0, tmpt_vert_->position_[0], 0, 1, 0,
											 tmpt_vert_->position_[2], tmpt_vert_->position_[1], -tmpt_vert_->position_[0], 0, 0, 0, 1;
		}
		//cout << A_i << endl<<endl;
		//cout << tmpt_mat_i_ << endl << endl;
		MatrixXf coef_mat_i_ = tmpt_mat_i_ * (A_i.transpose() * A_i).inverse() * A_i.transpose();
		//cout << coef_mat_i_ << endl;
		// Set Coefficient Matrix
		triplets_.push_back(Eigen::Triplet<float >(i, i, 1 - coef_mat_i_(0, 0)));
		triplets_.push_back(Eigen::Triplet<float >(i, i + vertices_num_, -coef_mat_i_(0, 1)));
		triplets_.push_back(Eigen::Triplet<float >(i, i + 2 * vertices_num_, -coef_mat_i_(0, 2)));

		triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, i, -coef_mat_i_(1, 0)));
		triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, i + vertices_num_, 1 - coef_mat_i_(1, 1)));
		triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, i + 2 * vertices_num_, -coef_mat_i_(1, 2)));

		triplets_.push_back(Eigen::Triplet<float >(i + 2*vertices_num_, i, -coef_mat_i_(2, 0)));
		triplets_.push_back(Eigen::Triplet<float >(i + 2*vertices_num_, i + vertices_num_, -coef_mat_i_(2, 1)));
		triplets_.push_back(Eigen::Triplet<float >(i + 2*vertices_num_, i + 2*vertices_num_, 1 - coef_mat_i_(2, 2)));

		for (int m = 0; m < degree_i_; m++)
		{
			tmpt_vert_ = init_vertices_[init_vertices_[i]->neighborIdx[m]];
			triplets_.push_back(Eigen::Triplet<float >(i, tmpt_vert_->id_, -1.0 / degree_i_ - coef_mat_i_(0, 3 * (m + 1))));
			triplets_.push_back(Eigen::Triplet<float >(i, tmpt_vert_->id_ + vertices_num_,-coef_mat_i_(0, 3 * (m + 1)+1)));
			triplets_.push_back(Eigen::Triplet<float >(i, tmpt_vert_->id_ + 2*vertices_num_, -coef_mat_i_(0, 3 * (m + 1)+2)));

			triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, tmpt_vert_->id_, -coef_mat_i_(1, 3 * (m + 1))));
			triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, tmpt_vert_->id_ + vertices_num_, -1.0 / degree_i_ - coef_mat_i_(1, 3 * (m + 1) + 1)));
			triplets_.push_back(Eigen::Triplet<float >(i + vertices_num_, tmpt_vert_->id_ + 2 * vertices_num_, -coef_mat_i_(1, 3 * (m + 1) + 2)));

			triplets_.push_back(Eigen::Triplet<float >(i + 2 * vertices_num_, tmpt_vert_->id_,  -coef_mat_i_(2, 3 * (m + 1))));
			triplets_.push_back(Eigen::Triplet<float >(i + 2 * vertices_num_, tmpt_vert_->id_ + vertices_num_, -coef_mat_i_(2, 3 * (m + 1) + 1)));
			triplets_.push_back(Eigen::Triplet<float >(i + 2 * vertices_num_, tmpt_vert_->id_ + 2 * vertices_num_, -1.0 / degree_i_ - coef_mat_i_(2, 3 * (m + 1) + 2)));
		}
	}
	for (i = 0; i < fixed_num_; i++)
	{
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * vertices_num_, fixed_id_array_[i], 1.0 * MAX));
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * vertices_num_ + fixed_num_, fixed_id_array_[i] + vertices_num_, 1.0 * MAX));
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * vertices_num_ + 2 * fixed_num_, fixed_id_array_[i] + 2 * vertices_num_, 1.0 * MAX));
	}
	for (i = 0; i < handle_num_; i++)
	{
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * (vertices_num_ + fixed_num_), handle_id_array_[i], 1.0 * MAX));
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * (vertices_num_ + fixed_num_) + handle_num_, handle_id_array_[i] + vertices_num_, 1.0 * MAX));
		triplets_.push_back(Eigen::Triplet<float >(i + 3 * (vertices_num_ + fixed_num_) + 2 * handle_num_, handle_id_array_[i] + 2 * vertices_num_, 1.0 * MAX));
	}
	coef_mat_.setFromTriplets(triplets_.begin(), triplets_.end());
}

void Laplacian_Editor::Predecomposite_Matrix()
// predecomposite the coefficient matrix
{
	SparseMatrix<float > AAT;
	AAT = coef_mat_.transpose() * coef_mat_;
	AAT.makeCompressed();
	chol_.compute(AAT);
}

void Laplacian_Editor::Do_Laplacian_Editor(vector<HE_vert*> &init_vertices, vector<int> fixed_id_array, vector<int> handle_id_array)
// laplacian editoring the mesh
{
	if (!is_initial_)		// initialize only one time
		Init(init_vertices, fixed_id_array, handle_id_array);

	// update coordinates of handle vertices
	int i;
	VectorXf rhs_vec_(3 * (vertices_num_ + fixed_num_ + handle_num_));
	VectorXf sol_;
	rhs_vec_.setZero();
	for (i = 0; i < fixed_num_; i++)
	{
		rhs_vec_(i + 3 * vertices_num_) = MAX * init_vertices[fixed_id_array_[i]]->position_[0];
		rhs_vec_(i + 3 * vertices_num_ + fixed_num_) = MAX * init_vertices[fixed_id_array_[i]]->position_[1];
		rhs_vec_(i + 3 * vertices_num_ + 2 * fixed_num_) = MAX * init_vertices[fixed_id_array_[i]]->position_[2];
	}

	for (i = 0; i < handle_num_; i++)
	{
		rhs_vec_(i + 3 * (vertices_num_ + fixed_num_)) = MAX * init_vertices[handle_id_array[i]]->position_[0];
		rhs_vec_(i + 3 * (vertices_num_ + fixed_num_) + handle_num_) = MAX * init_vertices[handle_id_array[i]]->position_[1];
		rhs_vec_(i + 3 * (vertices_num_ + fixed_num_) + 2 * handle_num_) = MAX * init_vertices[handle_id_array[i]]->position_[2];
	}
	sol_ = chol_.solve(coef_mat_.transpose()*rhs_vec_);
	for (i = 0; i < vertices_num_; i++)
	{
		init_vertices[i]->position_[0] = sol_(i);
		init_vertices[i]->position_[1] = sol_(i + vertices_num_);
		init_vertices[i]->position_[2] = sol_(i + 2 * vertices_num_);
	}
}

