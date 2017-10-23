#pragma once
#include "Mesh3D.h"

class Model
{
public:
	typedef struct Vertex
	{
		Vertex()
		{
			position = Vec3f(0,0,0);
			lap_coord_ = Vec3f(0, 0, 0);
			texCoord = Vec2f(0,0);
			normal = Vec3f(0,0,0);
			color = Vec4f(0,0,0,0);
		}
		Vec3f position;
		Vec3f lap_coord_;
		Vec2f texCoord;
		Vec3f normal;
		Vec4f color;
	}Vertex;

public:
	Model(void);
	~Model(void);

	void GenerateVertexesFromSource();

private:
	Vec3f GenerateLaplace(HE_vert* vert_);			// Calculate laplace coordinate of the vertex


public:
	Mesh3D *sourceData;
	std::vector<Vertex> vertexesOfTriangles;
};

