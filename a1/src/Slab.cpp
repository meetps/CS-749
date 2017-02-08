#include "Slab.hpp"
#include "DGP/Math.hpp"

bool
Slab::contains(Vector3 const & p) const
{
  // TODO
	// Find the distace from the central plane, if d < dist/2 then do next
	// Project the point on the central plane and check condition given on slab
	// Real d = distace(p);
	// if(2*d<thickness){
	// 	//
	// }
	// else
		return false;
}

bool
Slab::intersects(AxisAlignedBox3 const & box) const
{
  // TODO
  return false;
}

void
Slab::updateCorners(std::vector<Point *> const & points)
{
  static int const NUM_ROTS = 64;

  if (points.empty())
    return;

  if (points.size() == 1)
  {
    if (points[0]->isEnabled())
      corners[0] = corners[1] = corners[2] = corners[3] = points[0]->getPosition();

    return;
  }

  Vector3 c = plane.getPoint();
  Vector3 n = plane.getNormal();
  Vector3 u, v;
  n.createOrthonormalBasis(u, v);

  Real min_area = 0;
  for (int r = 0; r < NUM_ROTS; ++r)
  {
    Real angle = (r / (Real)NUM_ROTS) * Math::twoPi();
    Real sin_a = std::sin(angle);
    Real cos_a = std::cos(angle);

    Vector3 rot_u = cos_a * u + sin_a * v;
    Vector3 rot_v = cos_a * v - sin_a * u;

    Real min_u = 0, min_v = 0, max_u = 0, max_v = 0;
    for (size_t i = 0; i < points.size(); ++i)
    {
      if (!points[i]->isEnabled())
        continue;

      Vector3 diff = points[i]->getPosition() - c;
      Real proj_u = diff.dot(rot_u);
      Real proj_v = diff.dot(rot_v);

      if (i > 0)
      {
        min_u = std::min(min_u, proj_u);
        min_v = std::min(min_v, proj_v);

        max_u = std::max(max_u, proj_u);
        max_v = std::max(max_v, proj_v);
      }
      else
      {
        min_u = max_u = proj_u;
        min_v = max_v = proj_v;
      }
    }

    Real area = (max_u - min_u) * (max_v - min_v);
    if (r == 0 || area < min_area)
    {
      min_area = area;
      corners[0] = c + min_u * rot_u + min_v * rot_v;
      corners[1] = c + min_u * rot_u + max_v * rot_v;
      corners[2] = c + max_u * rot_u + max_v * rot_v;
      corners[3] = c + max_u * rot_u + min_v * rot_v;
    }
  }
}

void
Slab::updateCorners(std::vector<Point> const & points)
{
  std::vector<Point *> pp(points.size());
  for (size_t i = 0; i < pp.size(); ++i)
    pp[i] = const_cast<Point *>(&points[i]);  // removing the const is not the greatest thing to do, be careful...

  updateCorners(pp);
}

void
Slab::draw(Graphics::RenderSystem & rs, ColorRGBA const & plane_color, ColorRGBA const & outline_color) const
{
  rs.pushShader();
    rs.setShader(NULL);

    rs.pushColorFlags();

      rs.setColor(plane_color);
      rs.beginPrimitive(Graphics::RenderSystem::Primitive::QUADS);
        rs.sendVertex(corners[0]);
        rs.sendVertex(corners[1]);
        rs.sendVertex(corners[2]);
        rs.sendVertex(corners[3]);
      rs.endPrimitive();

      rs.setColor(outline_color);
      rs.beginPrimitive(Graphics::RenderSystem::Primitive::LINE_LOOP);
        rs.sendVertex(corners[0]);
        rs.sendVertex(corners[1]);
        rs.sendVertex(corners[2]);
        rs.sendVertex(corners[3]);
      rs.endPrimitive();

    rs.popColorFlags();

  rs.popShader();
}
