#ifndef __A1_Slab_hpp__
#define __A1_Slab_hpp__

#include "Point.hpp"
#include "DGP/Plane3.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Plane3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"
#include <vector>

/** A "thickened" plane, i.e. a plane which has been extruded along the normal upto 0.5 * thickness on both sides. */
class Slab
{
  private:
    Plane3 plane;
    Real thickness;
    Vector3 corners[4];

  public:
    /** Default constructor. */
    Slab() : thickness(0) {}

    /** Default constructor. */
    Slab(Plane3 const & plane_, Real thickness_) : plane(plane_), thickness(thickness_) {}

    /** Check if the slab contains a point (points lying exactly on the slab are assumed to be contained). */
    bool contains(Vector3 const & p) const;

    /** Check if the slab intersects an axis-aligned bounding box. */
    bool intersects(AxisAlignedBox3 const & box) const;

    /** Set the plane of the slab. */
    void setPlane(Plane3 const & p) { plane = p; }

    /** Get the plane of the slab. */
    Plane3 const & getPlane() const { return plane; }

    /** Set the thickness of the slab. */
    void setThickness(Real t) { thickness = t; }

    /** Get the thickness of the slab. */
    Real getThickness() const { return thickness; }

    /** Get a corner of the slab. */
    Vector3 const & getCorner(int i) const
    {
      alwaysAssertM(i >= 0 && i <= 3, "Corner index out of bounds");
      return corners[i];
    }

    /**
     * Update the corners of the slab to fit the projection of a set of points onto the supporting plane. Only points with
     * Point::isEnabled() = true are considered.
     */
    void updateCorners(std::vector<Point *> const & points);

    /**
     * Update the corners of the slab to fit the projection of a set of points onto the supporting plane. Only points with
     * Point::isEnabled() = true are considered.
     */
    void updateCorners(std::vector<Point> const & points);

    /** Draw the slab as a quad with a border using a render-system. */
    void draw(Graphics::RenderSystem & rs, ColorRGBA const & plane_color = ColorRGB(1.0, 1.0, 1.0),
                                           ColorRGBA const & outline_color = ColorRGB(1, 1, 0)) const;

}; // class Slab

#endif
