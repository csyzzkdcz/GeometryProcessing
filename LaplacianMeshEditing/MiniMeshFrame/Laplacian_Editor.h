#pragma once
#include"HE_mesh\Mesh3D.h"
#include<vector>
#include <Eigen>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
using namespace Eigen;
using namespace std;
#ifndef MAX
#define MAX 20
#endif

class Laplacian_Editor
{
public:
	Laplacian_Editor();
	~Laplacian_Editor();
	void Init(vector<HE_vert*> init_vertices, vector<int> fixed_id_array, vector<int> handle_id_array);
	// initialization
	void Do_Laplacian_Editor(vector<HE_vert*> &init_vertices, vector<int> fixed_id_array, vector<int> handle_id_array);
	// laplacian editoring the mesh

private:
	void Generate_Laplacian_Coord();							// generate Laplacian coordinates
	void Generate_Coef_Matrix();								// generate the coeffficient matrix
	void Predecomposite_Matrix();								// predecomposite the coefficient matrix
	

private:
	// vertices record along with fixed and handle vertices
	vector<HE_vert*> init_vertices_;
	vector<int>  fixed_id_array_;
	vector<int>  handle_id_array_;

	// laplacian coordinates
	vector<float> Lx_array_;
	vector<float> Ly_array_;
	vector<float> Lz_array_;

	// handle changing record
	float		delta_handle_x_;
	float		delta_handle_y_;
	float		delta_handle_z_;

	// Coefficient Matrix and its Cholesky decomposition record
	SparseMatrix<float > coef_mat_;
	SimplicialCholesky<SparseMatrix<float> > chol_;

	// vertices size, fixed vertices number and handle vertices number
	int			vertices_num_;
	int			fixed_num_;
	int			handle_num_;

	// initial flag
	bool			is_initial_;

};