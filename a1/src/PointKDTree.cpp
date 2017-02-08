#include "PointKDTree.hpp"

PointKDTree::Node::~Node()
{
  delete lo;
  delete hi;
}

PointKDTree::PointKDTree(std::vector<Point> const & points)
: root(NULL)
{
  build(points);
}

PointKDTree::PointKDTree(std::vector<Point *> const & points)
: root(NULL)
{
  build(points);
}

PointKDTree::PointKDTree(PointCloud const & pcloud)
: root(NULL)
{
  build(pcloud.getPoints());
}

void
PointKDTree::build(std::vector<Point> const & points)
{
  std::vector<Point *> pp(points.size());
  for (size_t i = 0; i < pp.size(); ++i)
    pp[i] = const_cast<Point *>(&points[i]);  // removing the const is not the greatest thing to do, be careful...

  build(pp);
}


PointKDTree::Node *PointKDTree::node(std::vector<Point *> const & points)
{
  PointKDTree* temp = new PointKDTree(points);
  return temp->root;
}


void fillLoAndHi(std::vector<Point*> &loPoints, 
	std::vector<Point*> &hiPoints, std::vector<Point*> const &points, 
	float midValue, int index){

	for(int i=0; i<points.size(); i++){
		if((points[i]->getPosition())[index] < midValue){
			loPoints.push_back(points[i]);
		}
		else{
			hiPoints.push_back(points[i]);
		}
	}
}


void
PointKDTree::build(std::vector<Point *> const & points)
{
  static size_t const MAX_POINTS_PER_LEAF = 5;

  // A kd-tree is just a binary search tree, and is constructed in a near-identical way.
  //
  // - Initially assign (pointers to) all points to the root node.
  // - Recursively subdivide the points, splitting the parent box in half along the longest axis and creating two child nodes
  //   for each split. Stop when number of points in node <= MAX_POINTS_PER_LEAF.
  // - Don't forget to save space by clearing the arrays of points in internal nodes. Only the leaves need to store references
  //   to points.
	
  // this->
  root->points = points; 

  for(int i=0; i<points.size(); i++)
  {
		root->bbox.addPoint((*points[i]).getPosition());
	}

	if(points.size()<=MAX_POINTS_PER_LEAF)
  {
		root->lo = root->hi = NULL;
		for (int i=0; i<points.size(); i++)
    {
			(root->points).push_back(points[i]);
		}
	}
	else 
  {
		std::vector<Point*> loPoints;
		std::vector<Point*> hiPoints;
		
    // std::vector<Point*> loPoints = new std::vector<Point*>;
		// std::vector<Point*> hiPoints= new std::vector<Point*>;

    VectorN<3,float> high = root->bbox.getHigh();
		VectorN<3,float> low = root->bbox.getLow();
		
    float maxLength = std::max(high[0]-low[0], std::max(high[1] - low[1], high[2] - low[2]));
		
    if(maxLength == high[0] - low[0])
    {
			fillLoAndHi(loPoints, hiPoints, points, (high[0] + low[0])/2.0, 0);
		}
		else if(maxLength == high[1] - low[1])
    {
			fillLoAndHi(loPoints, hiPoints, points, (high[1] + low[1])/2.0, 1);
		}
		else 
    {
			fillLoAndHi(loPoints, hiPoints, points, (high[2] + low[2])/2.0, 2);
		}
		
    root->lo = node(loPoints);
		root->hi = node(hiPoints);
	}
}