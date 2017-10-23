#include "Model.h"
#include<iostream>
using namespace std;

Model::Model(void) : vertexesOfTriangles(NULL)
{
	sourceData = new Mesh3D();
}


Model::~Model(void)
{
}

Vec3f Model::GenerateLaplace(HE_vert* vert_)
{
	Vec3f tmpt_(0, 0, 0);
	int i, degree_ = vert_->degree_;
	HE_edge* tmpt_edge_;
	tmpt_edge_ = vert_->pedge_;
	for (i = 0; i < degree_; i++)
	{
		tmpt_ = tmpt_ + tmpt_edge_->pvert_->position_;
		tmpt_edge_ = tmpt_edge_->ppair_->pnext_;
	}
	tmpt_[0] = vert_->position_[0] - 1.0 / degree_*tmpt_[0];
	tmpt_[1] = vert_->position_[1] - 1.0 / degree_*tmpt_[1];
	tmpt_[2] = vert_->position_[2] - 1.0 / degree_*tmpt_[2];
	return tmpt_;
}

void Model::GenerateVertexesFromSource()
{
	if (sourceData->num_of_vertex_list() == 0)
	{
		return;
	}
	
	vertexesOfTriangles.clear();
	std::vector<HE_face *> *facesList = sourceData->get_faces_list();

	for (int i=0; i<sourceData->num_of_face_list(); i++)
	{
		Vertex tempVertex;
		std::vector<Vertex> tempVertexVec;
		HE_face *nowFace = facesList->at(i);

		HE_edge* nowEdge = nowFace->pedge_;
		do 
		{
			tempVertex.position = nowEdge->pvert_->position();
			tempVertex.lap_coord_ = GenerateLaplace(nowEdge->pvert_);
			tempVertex.color = nowEdge->pvert_->color();
			tempVertex.normal = nowEdge->pvert_->normal();
			tempVertex.texCoord = Vec2f(nowEdge->pvert_->texCoordinate()[0],nowEdge->pvert_->texCoordinate()[1]);
			tempVertexVec.push_back(tempVertex);
			nowEdge = nowEdge->pnext_;
		} while (nowEdge!=nowFace->pedge_);

		for (int j=0; j<tempVertexVec.size()-2; j++)
		{
			vertexesOfTriangles.push_back(tempVertexVec[0]);
			vertexesOfTriangles.push_back(tempVertexVec[j+1]);
			vertexesOfTriangles.push_back(tempVertexVec[j+2]);
		}
	}


}

