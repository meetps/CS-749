# Assignment 1 | Meet Shah | 13d070003

I've added exhaustive comments in the code as and when needed in order 
to explain my logic behind the code. I'm repeating them here function wise.

### Slab::contains

/*****
  Logic - Obtain slab's central' plane equation 
        - Get equation of the bounding planes ( upperPlane and lowerPlane )
        - Check if the point p has opposite signs on both planes (return true if yes)
        - Else return false 
*****/

### Slab::intersects

/*****
  Logic - Obtain slab's central' plane equation 
        - Get equation of the bounding planes ( upperPlane and lowerPlane )
        - Check iteratively if all 8 points of the boundingBox have opposite signs on both planes
        - Return true if all 8 checks are true.
*****/  

### PointKDTree::buildTreeReturnRoot

/*****
  Logic - Make a new kd-tree with all points to root 
        - Return the pointer to the root Node. 
*****/

### PointKDTree::build

/*****
  Logic - Make new node and assign all points 
        - Add points to the bbox of the node
        - If points in node are within limits, do not split
        - Else begin point splitting and make a new subtrees wih the split points.
*****/

### PointKDTree::splitPoints

/*****
  Logic - Split the given vector of points into low and high
          based on the given midvalue.
*****/

### PointKDTree::rangeQuery

/*****
  Logic - Write a private recursive helper function rangeQuery(node, range, points_in_range):
        - If node->bbox does not intersect range, return
        - If the node is a leaf, add all the points contained in range to points_in_range
        - If node->lo && node->lo->bbox intersects range, rangeQuery(node->lo, range, points_in_range)
        - If node->hi && node->hi->bbox intersects range, rangeQuery(node->hi, range, points_in_range)
*****/

### PointKDTree::rangeQueryHelper

/*****
  Logic - Helper function for rangeQuery to handle multiple template input types
*****/

### PointCloud::ransac

/*****
  Logic - Construct a kd-tree on the enabled points (remember to build the kd-tree with pointers to existing points -- you
          shouldn't be copying the points themselves, either explicitly or implicitly).
        - Generate num_iters random triplets of enabled points and fit a plane to them.
        - Generate 3 points && fit a plane through them
        - Using the kd-tree, see how many other enabled points are contained in the slab supported by this plane with thickness
          slab_thickness (extends to distance 0.5 * slab_thickness on each side of the plane).
        - If this number is >= min_points and > the previous maximum, the plane is the current best fit. Set the 'slab' argument
          to be the slab for this plane, and update slab_points to be the set of (enabled) matching points for this plane.
        - At the end, for visualization purposes, update the corners of the best slab using its set of matching points, and
          return the number of (enabled) matching points.
*****/

## Standard Library Functionality Usage

- I used an unbiased Sampler from stdlib from random integer generation
  in a given range.

## REFERENCES

None

## Additional Notes

- I've added an extra file makerun.sh that compiles and runs the ransac code
  with ideal parameters for the cube.pts data and detects all 6 planes.
