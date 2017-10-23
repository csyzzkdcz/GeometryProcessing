#include"MiniSurface.h"

MiniSurface::MiniSurface()
{
	tar_mesh_ = NULL;
	BC_Editor_ = NULL;
}


MiniSurface::~MiniSurface()
{
	if (BC_Editor_)
	{
		delete(BC_Editor_);
		BC_Editor_ = NULL;
	}
	if (tar_mesh_)
	{
		delete(tar_mesh_);
		tar_mesh_ = NULL;
	}
}

void MiniSurface::Init(Mesh3D *ptr_mesh_)
{
	tar_mesh_ = new Mesh3D();
	tar_mesh_ = ptr_mesh_;
	Get_Init_Vertices();
}

void MiniSurface::MiniSurface_Method()
{

}

void MiniSurface::get_neighborId(const size_t& vertid, std::vector<size_t>& neighbors)
	{
		HE_edge* edge = tar_mesh_->get_vertex(vertid)->pedge_;
		neighbors.clear();
		do 
		{
			neighbors.push_back(edge->pvert_->id());
			edge = edge->ppair_->pnext_;
		} while (edge!=tar_mesh_->get_vertex(vertid)->pedge_ && edge != NULL);

		std::vector<size_t> tmp = neighbors;
		for (size_t i=0; i!= tmp.size(); ++i)
			neighbors[i] = tmp[tmp.size()-i-1];
	}

void MiniSurface::Calculate_BC(int index_, vector<double> &BC_weight_, vector<size_t> neighbors)
{
	int size_ = neighbors.size();
	if (size_ == 0 && BC_Editor_ == NULL)
		return;
	vector<double> rx_array_;
	vector<double> ry_array_;
	vector<double> rz_array_;
	rx_array_.clear();
	ry_array_.clear();
	rz_array_.clear();
	BC_weight_.clear();
	rx_array_.resize(size_);
	ry_array_.resize(size_);
	rz_array_.resize(size_);
	BC_weight_.resize(size_);
	for (int j = 0; j < size_; j++)
	{
		rx_array_[j] = init_vertices_[neighbors[j]](0);
		ry_array_[j] = init_vertices_[neighbors[j]](1);
		rz_array_[j] = init_vertices_[neighbors[j]](2);
	}
	BC_Editor_->Calculate_Coodinate(init_vertices_[index_](0), init_vertices_[index_](1), init_vertices_[index_](2), BC_weight_, rx_array_, ry_array_, rz_array_);

}
void MiniSurface::Get_Init_Vertices()
{
	init_vertices_.clear();
	Vector3f vertex_;
	for(int i = 0; i < tar_mesh_->num_of_vertex_list();i++)
	{
		vertex_(0) = tar_mesh_->get_vertex(i)->position_.x();
		vertex_(1) = tar_mesh_->get_vertex(i)->position_.y();
		vertex_(2) = tar_mesh_->get_vertex(i)->position_.z();
		init_vertices_.push_back(vertex_);
		is_boundary_.push_back(tar_mesh_->get_vertex(i)->isOnBoundary());
	}
}

void MiniSurface::Update_Surface()
{
	for(int i=0;i<final_vertices_.size();i++)
	{
		Vec3f vertex_(final_vertices_[i](0),final_vertices_[i](1),final_vertices_[i](2));
		tar_mesh_->get_vertex(i)->position_ = vertex_;
	}

}

