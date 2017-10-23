#include"Parameterization_FixedBoundary.h"

Parameterization_FixedBoundary::Parameterization_FixedBoundary()
{

}

Parameterization_FixedBoundary::~Parameterization_FixedBoundary()
{

}

void Parameterization_FixedBoundary::Do_Parameterizing(vector<HE_vert* > &vertices_)
{

}

void Parameterization_FixedBoundary::Get_Boundary_vertices(vector<HE_vert*> vertices_)
{
	boundary_vertices_id_.clear();
	int num_ = vertices_.size();
	int i = 0;
	while (!vertices_[i]->isOnBoundary())			// start from a vertex on the boundary
		i++;
	boundary_vertices_id_.push_back(i);
	int j = i;
	bool finished = false;
	while (!finished)
	{
		int k=0;
		while (!vertices_[vertices_[j]->neighborIdx[k]]->isOnBoundary())
		{
			k++;
		}
		j = vertices_[j]->neighborIdx[k];
		if (j == i)
		{
			finished = true;
		}
		else
			boundary_vertices_id_.push_back(j);
	}
}