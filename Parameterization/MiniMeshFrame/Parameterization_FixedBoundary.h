#pragma once
#include"Parameterization.h"

class Parameterization_FixedBoundary : public Parameterization
{
public:
	Parameterization_FixedBoundary();
	virtual ~Parameterization_FixedBoundary();

public:
	void virtual Do_Parameterizing(vector<HE_vert*> &vertices_);

protected:
	void Get_Boundary_vertices(vector<HE_vert*> vertices_);				// get the boundary vertices of the mesh

protected:
	vector<int> boundary_vertices_id_;									// store the index of the boudary vertices
	vector<point > boundary_vertices_after_mapping_;					// store the coordinate of the boundary vertices after mapping
};