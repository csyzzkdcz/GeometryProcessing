#include "HE_mesh\Mesh3D.h"
#include<vector>
#define MAX_AREA_RATIO 0.001
#define FOLD_RATIO 0.3
extern"C"
{
#include"triangle.h"
};
using namespace std;
class GenerateMesh
{
public:
	GenerateMesh();
	~GenerateMesh();
	GenerateMesh(vector<double> BC_x_array_, vector<double> BC_y_array_);

public:
	void DoGenerating();

public:
	Mesh3D *my_mesh_;

private:
	vector<double> rx_array_;
	vector<double> ry_array_;
	struct triangulateio triangle_frame_;
	void Triangulation_with_angle_constraint();
	double area_;
	double min_x_, min_y_, max_x_, max_y_;
};