#include"GenerateMesh.h"
GenerateMesh::GenerateMesh()
{
	rx_array_.resize(4);
	ry_array_.resize(4);
	rx_array_[0] = -3;
	rx_array_[1] = 3;
	rx_array_[2] = 3;
	rx_array_[3] = -3;
	ry_array_[0] = -1;
	ry_array_[1] = -1;
	ry_array_[2] = 1;
	ry_array_[3] = 1;
	min_x_ = -3;
	min_y_ = -1;
	max_x_ = 3;
	max_y_ = 1;
	area_ = 12;
	my_mesh_ = NULL;
}

GenerateMesh::GenerateMesh(vector<double> BC_x_array_, vector<double> BC_y_array_)
{
	int i, size_ = BC_x_array_.size();
	rx_array_.clear();
	ry_array_.clear();
	min_x_=0, min_y_=0, max_x_=0, max_y_=0;
	for (i = 0; i < size_; i++)
	{
		if (BC_x_array_[i] < min_x_)
			min_x_ = BC_x_array_[i];
		if (BC_x_array_[i] > max_x_)
			max_x_ = BC_x_array_[i];
		if (BC_y_array_[i] < min_y_)
			min_y_ = BC_y_array_[i];
		if (BC_y_array_[i] > max_y_)
			max_y_ = BC_y_array_[i];
		rx_array_.push_back(BC_x_array_[i]);
		ry_array_.push_back(BC_y_array_[i]);
	}
	area_ = (max_x_ - min_x_) * (max_y_ - min_y_);
}

GenerateMesh::~GenerateMesh()
{
	rx_array_.clear();
	ry_array_.clear();
	if (my_mesh_)
		delete(my_mesh_);
}

void GenerateMesh::Triangulation_with_angle_constraint()
{
	struct triangulateio initial_frame_, mid_frame_, vorout;
	int i;
	initial_frame_.numberofpoints = rx_array_.size();
	initial_frame_.numberofpointattributes = 0;
	initial_frame_.pointlist = (REAL *)malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.pointmarkerlist = (int *)malloc(initial_frame_.numberofpoints * sizeof(REAL));
	initial_frame_.pointattributelist = (REAL *)malloc(initial_frame_.numberofpoints *
		initial_frame_.numberofpointattributes * 0 *
		sizeof(REAL));
	initial_frame_.numberofregions = 0;
	initial_frame_.numberofsegments = initial_frame_.numberofpoints;
	initial_frame_.numberofholes = 0;
	initial_frame_.regionlist = (REAL *)malloc(initial_frame_.numberofregions * 5 * sizeof(REAL));
	initial_frame_.segmentlist = (int *)malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.segmentmarkerlist = (int *)malloc(initial_frame_.numberofpoints * sizeof(REAL));

	for (i = 0; i<rx_array_.size(); i++)
	{
		initial_frame_.pointlist[2 * i] =rx_array_[i];
		initial_frame_.segmentlist[2 * i] = i;
		initial_frame_.pointlist[2 * i + 1] =ry_array_[i];
		initial_frame_.segmentlist[2 * i + 1] = (i + 1) % rx_array_.size();
		initial_frame_.pointmarkerlist[i] = 1;
		initial_frame_.segmentmarkerlist[i] = 1;
	}
	mid_frame_.pointlist = (REAL *)NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of point attributes is zero: */
	mid_frame_.pointattributelist = (REAL *)NULL;
	mid_frame_.pointmarkerlist = (int *)NULL; /* Not needed if -N or -B switch used. */
	mid_frame_.trianglelist = (int *)NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	mid_frame_.triangleattributelist = (REAL *)NULL;
	mid_frame_.neighborlist = (int *)NULL;         /* Needed only if -n switch used. */
	/* Needed only if segments are output (-p or -c) and -P not used: */
	mid_frame_.segmentlist = (int *)NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid_frame_.segmentmarkerlist = (int *)NULL;
	mid_frame_.edgelist = (int *)NULL;             /* Needed only if -e switch used. */
	mid_frame_.edgemarkerlist = (int *)NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *)NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *)NULL;
	vorout.edgelist = (int *)NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *)NULL;         /* Needed only if -v switch used. */

	// Initial triangular frame
	triangulate("pzAevn", &initial_frame_, &mid_frame_, &vorout);

	/* Needed only if -r and -a switches used: */
	mid_frame_.trianglearealist = (REAL *)malloc(mid_frame_.numberoftriangles * sizeof(REAL));
	for (int i = 0; i < mid_frame_.numberoftriangles; i++)
	{
		mid_frame_.trianglearealist[i] = area_ * MAX_AREA_RATIO;
	}

	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `out'.                                    */

	triangle_frame_.pointlist = (REAL *)NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of attributes is zero: */
	triangle_frame_.pointattributelist = (REAL *)NULL;
	triangle_frame_.trianglelist = (int *)NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	triangle_frame_.triangleattributelist = (REAL *)NULL;

	/* Refine the triangulation according to the attached */
	/*   triangle area constraints.                       */

	triangulate("prazBPa30", &mid_frame_, &triangle_frame_, (struct triangulateio *) NULL);

	// free 
	free(initial_frame_.pointlist);
	free(initial_frame_.pointattributelist);
	free(initial_frame_.pointmarkerlist);
	free(initial_frame_.regionlist);
	free(mid_frame_.pointlist);
	free(mid_frame_.pointattributelist);
	free(mid_frame_.pointmarkerlist);
	free(mid_frame_.trianglelist);
	free(mid_frame_.triangleattributelist);
	free(mid_frame_.trianglearealist);
	free(mid_frame_.neighborlist);
	free(mid_frame_.segmentlist);
	free(mid_frame_.segmentmarkerlist);
	free(mid_frame_.edgelist);
	free(mid_frame_.edgemarkerlist);
	free(vorout.pointlist);
	free(vorout.pointattributelist);
	free(vorout.edgelist);
	free(vorout.normlist);
}

void GenerateMesh::DoGenerating()
{
	Triangulation_with_angle_constraint();
	Vec3f vert_;
	int i;
	vector<Vec3f> my_vertices_;
	vector<int> triInd_;
	my_mesh_ = new Mesh3D();
	for (i = 0; i < triangle_frame_.numberofpoints; i++)
	{
		vert_[0] = triangle_frame_.pointlist[2 * i];
		vert_[1] = triangle_frame_.pointlist[2 * i + 1];
		// Rolling mesh
		if (abs(vert_[0])<1 / 2.0 * max_x_)
			vert_[2] = 1 / 2.0 * max_x_ - sqrt(pow(1 / 2.0 * max_x_, 2) - vert_[0] * vert_[0]);
		else
		{
			vert_[2] = 1 / 2.0 * max_x_ + sqrt(pow(1 / 2.0 * max_x_, 2) - pow(2 * 1 / 2.0 * max_x_ - abs(vert_[0]), 2));
			if (vert_[0] < 0 )
				vert_[0] = -max_x_ + abs(vert_[0]);
			else
				vert_[0] =  max_x_ - abs(vert_[0]);
		}
		/*If you want to generate folded mesh, please use this below code*/
		/*if (vert_[0] > FOLD_RATIO * min_x_ + (1-FOLD_RATIO)* max_x_ && vert_[0] <= max_x_)
		{
			vert_[2] = vert_[0] - (FOLD_RATIO * min_x_ + (1 - FOLD_RATIO)* max_x_);
			vert_[0] = FOLD_RATIO * min_x_ + (1 - FOLD_RATIO)* max_x_;
		}
		else if (vert_[0] < FOLD_RATIO*max_x_ + (1 - FOLD_RATIO) * min_x_ && vert_[0] >= min_x_)
		{
			vert_[2] = FOLD_RATIO*max_x_ + (1 - FOLD_RATIO) * min_x_ - vert_[0] ;
			vert_[0] = FOLD_RATIO*max_x_ + (1 - FOLD_RATIO) * min_x_;
		}
		else if (vert_[0] <= FOLD_RATIO * min_x_ + (1 - FOLD_RATIO)* max_x_ && vert_[0] >= 0)
		{
			vert_[2] = 1.2-vert_[0];
		}
		else if (vert_[0] >= FOLD_RATIO*max_x_ + (1 - FOLD_RATIO) * min_x_ && vert_[0] <= 0 )
		{
			vert_[2] = vert_[0] + 1.2;
		}*/
		my_vertices_.push_back(vert_);
	}
	for (i = 0; i < triangle_frame_.numberoftriangles; i++)
	{
		triInd_.push_back(triangle_frame_.trianglelist[3 * i]);
		triInd_.push_back(triangle_frame_.trianglelist[3 * i + 1]);
		triInd_.push_back(triangle_frame_.trianglelist[3 * i + 2]);
	}
	my_mesh_->CreateMesh(my_vertices_,triInd_);
}