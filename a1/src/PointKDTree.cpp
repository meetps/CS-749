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


PointKDTree::Node *PointKDTree::buildTreeReturnRoot(std::vector<Point *> const & points)
{
  PointKDTree* tempTree = new PointKDTree(points);
  return tempTree->root;
}


void splitPoints(std::vector<Point*> &lowPoints, 
	               std::vector<Point*> &highPoints, 
                 std::vector<Point*> const &points, 
	               float midValue, 
                 int index)
{
	for(int i=0; i<points.size(); i++)
  {
		if((points[i]->getPosition())[index] < midValue)
    {
			lowPoints.push_back(points[i]);
		}
		else
    {
			highPoints.push_back(points[i]);
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

  // Make new node and assign all points
  Node * rootPointer = new Node();
  rootPointer->points = points;
  root = rootPointer; 

  // Add points to the bbox of the node
  for(int i=0; i<points.size(); i++)
  {
		root->bbox.addPoint((*points[i]).getPosition());
	}

  // If points in node are within limits, do not split
	if(points.size()<=MAX_POINTS_PER_LEAF)
  {
		root->lo = root->hi = NULL;
		for (int i=0; i<points.size(); i++)
    {
			(root->points).push_back(points[i]);
		}
	}
  // Else begin point splitting
	else 
  {
		std::vector<Point*> lowPoints;
		std::vector<Point*> highPoints;
		
    VectorN<3,float> highValues = root->bbox.getHigh();
		VectorN<3,float> lowValues = root->bbox.getLow();
		
    float maxLength = std::max(highValues[0]-lowValues[0], 
                               std::max(highValues[1] - lowValues[1], 
                                        highValues[2] - lowValues[2]));
		
    if(maxLength == highValues[0] - lowValues[0])
    {
			splitPoints(lowPoints, highPoints, points, (highValues[0] + lowValues[0])/2.0, 0);
		}
		else if(maxLength == highValues[1] - lowValues[1])
    {
			splitPoints(lowPoints, highPoints, points, (highValues[1] + lowValues[1])/2.0, 1);
		}
		else 
    {
			splitPoints(lowPoints, highPoints, points, (highValues[2] + lowValues[2])/2.0, 2);
		}
		
    root->lo = buildTreeReturnRoot(lowPoints);
		root->hi = buildTreeReturnRoot(highPoints);
	}
}