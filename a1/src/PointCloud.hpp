#ifndef __A1_PointCloud_hpp__
#define __A1_PointCloud_hpp__

#include "Point.hpp"
#include "Slab.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"
#include <vector>

/** A set of 3D points. */
class PointCloud
{
  private:
    std::vector<Point> points;
    AxisAlignedBox3 bbox;

  public:
    /** Default constructor. */
    PointCloud() {}

    /** Construct from a list of points. */
    PointCloud(std::vector<Point> const & points_);

    /** Construct from a list of positions and normals. */
    PointCloud(std::vector<Vector3> const & positions, std::vector<Vector3> const & normals);

    /** Get the number of points. */
    long numPoints() const { return (long)points.size(); }

    /** Check if the point cloud is empty. */
    bool isEmpty() const { return points.empty(); }

    /** Reset the point cloud to an empty state. */
    void clear() { points.clear(); bbox.setNull(); }

    /** Get the list of points. */
    std::vector<Point> const & getPoints() const { return points; }

    /** Get the i'th point. */
    Point const & getPoint(long i) const { return points[(size_t)i]; }

    /** Add a point to the point cloud. */
    void addPoint(Point const & p) { points.push_back(p); bbox.merge(p.getPosition()); }

    /** Add a point, specified by its position and normal, to the point cloud. */
    void addPoint(Vector3 const & p, Vector3 const & n) { points.push_back(Point(p, n)); bbox.merge(p); }

    /** Get the bounding box of the point cloud. */
    AxisAlignedBox3 const & getAABB() const { return bbox; }

    /** Load the point cloud from a disk file. */
    bool load(std::string const & path);

    /** Save the point cloud to a disk file. */
    bool save(std::string const & path) const;

    /** Draw the point cloud using a render-system. */
    void draw(Graphics::RenderSystem & rs, Real normal_len = -1) const;

    /**
     * Estimate the plane best fitting the largest subset of the points, using RANSAC. Ignore all points for which
     * Point::isEnabled() returns false.
     *
     * @param num_iters Number of random triplets to consider.
     * @param slab_thickness Twice the distance between the central plane and the slab surface.
     * @param min_points The minimum number of enabled points matching a plane for the latter to be considered a candidate.
     * @param slab The slab of the computed plane, if one is found [return value].
     * @param slab_points The points in the returned slab [return value].
     *
     * @return The number of enabled points matching the best plane, if one was found, else 0.
     */
    long ransac(long num_iters, Real slab_thickness, long min_points, Slab & slab, std::vector<Point *> & slab_points) const;

    /** Try to extract \a num_planes planes using RANSAC and return the number of planes actually found. */
    long ransacMultiple(long num_planes, long num_iters, Real slab_thickness, long min_points, std::vector<Slab> & slabs) const;

    /** Adaptively resample the point cloud to reduce the number of points in smooth areas. Don't introduce any new points. */
    void adaptiveDownsample(std::vector<Slab> const & slabs);

  private:
    /** Recompute the bounding box. */
    void recomputeAABB();

}; // class PointCloud

#endif
