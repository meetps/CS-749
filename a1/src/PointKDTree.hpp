#ifndef __A1_PointKDTree_hpp__
#define __A1_PointKDTree_hpp__

#include "Common.hpp"
#include "Point.hpp"
#include "PointCloud.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Noncopyable.hpp"
#include "DGP/Vector3.hpp"

/* A kd-tree on points. */
class PointKDTree : private Noncopyable
{
  private:
    /** A node of the kd-tree. */
    struct Node
    {
      /** Constructor. */
      Node() : lo(NULL), hi(NULL) {}

      /** Destructor. */
      ~Node();

      AxisAlignedBox3 bbox;         ///< Bounding box of the node.
      Node * lo;                    ///< Child of the node with smaller coordinates.
      Node * hi;                    ///< Child of the node with larger coordinates.
      std::vector<Point *> points;  ///< Set of points in the node (empty unless leaf node).
    };

    /** The root node of the kd-tree. */
    Node * root;

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    void build(std::vector<Point *> const & points);

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    void build(std::vector<Point> const & points);

  public:
    /** Construct from a set of points. The points must last as long this kd-tree does. */
    PointKDTree(std::vector<Point> const & points);

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    PointKDTree(std::vector<Point *> const & points);

    /** Construct from a point cloud. The point cloud must last as long this kd-tree does. */
    PointKDTree(PointCloud const & pcloud);

    /** Destructor. */
    ~PointKDTree() { clear(); }

    /** Reset the tree to an empty state. */
    void clear() { delete root; root = NULL; }

    /**
     * Do a range query, returning the set of points within a query range. The range type RangeT must support the following
     * functions:
     *
     *   bool contains(Vector3 const & p) const;              // check if range contains a point
     *   bool intersects(AxisAlignedBox3 const & box) const;  // check if range intersects bounding box (of kd-tree node)
     *
     * The AxisAlignedBox3 class already has these functions and can be used for testing.
     *
     * @param range The query range.
     * @param points_in_range Used to return the set of points within the query range.
     */
    template <typename RangeT>
    void rangeQuery(RangeT const & range, std::vector<Point *> & points_in_range) const
    {
      // TODO

      points_in_range.clear();

      // Write a private recursive helper function rangeQuery(node, range, points_in_range):
      //   - If node->bbox does not intersect range, return
      //   - If the node is a leaf, add all the points contained in range to points_in_range
      //   - If node->lo && node->lo->bbox intersects range, rangeQuery(node->lo, range, points_in_range)
      //   - If node->hi && node->hi->bbox intersects range, rangeQuery(node->hi, range, points_in_range)
    }

}; // class PointKDTree

#endif
