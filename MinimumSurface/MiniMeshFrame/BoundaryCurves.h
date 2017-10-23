#pragma once
#include<vector>
using namespace std;
enum BC_TYPE
{
	kCircle,
	kEllipse,
	kRect,
	kNoneType
};
class BoundaryCurves
{
public:
	BoundaryCurves();
	virtual ~BoundaryCurves();
public:
	void virtual Curves(vector<double> &rx_array_, vector<double> &ry_array_);
};
