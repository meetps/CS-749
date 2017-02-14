#include "Slab.hpp"
#include "DGP/Math.hpp"

bool Slab::contains(Vector3 const & p) const{
  /*****
  Logic - Obtain slab's central' plane equation 
        - Get equation of the bounding planes ( upperPlane and lowerPlane )
        - Check if the point p has opposite signs on both planes (return true if yes)
        - Else return false 
  *****/
  
  float a, b, c, d;
	
  plane.getEquation(a, b, c, d);

	float distance = std::sqrt(a*a + b*c + c*c) * getThickness()/2;
	
	Plane3 upperPlane = Plane3::fromEquation(a, b, c, d+distance);
	Plane3 lowerPlane = Plane3::fromEquation(a, b, c, d-distance);
	
	if(upperPlane.positiveHalfSpaceContains(p) && lowerPlane.negativeHalfSpaceContains(p) )
		return true;

	if(upperPlane.negativeHalfSpaceContains(p) && lowerPlane.positiveHalfSpaceContains(p) )
		return true;		

	return false;
}

bool Slab::intersects(AxisAlignedBox3 const & box) const{
  /*****
  Logic - Obtain slab's central' plane equation 
        - Get equation of the bounding planes ( upperPlane and lowerPlane )
        - Check iteratively if all 8 points of the boundingBox have opposite signs on both planes
        - Return true if all 8 checks are true.
  *****/  

  float a, b, c, d;
	plane.getEquation(a, b, c, d);

	float distance = std::sqrt(a*a + b*c + c*c) * getThickness()/2;
  
	Plane3 upperPlane = Plane3::fromEquation(a, b, c, d+distance);
	Plane3 lowerPlane = Plane3::fromEquation(a, b, c, d-distance);
	
  int pointsLeft=0, pointsRight=0;

  for(int i=0; i<8; i++)
  {
		Vector3 p = box.getCorner(i);

    if( (!upperPlane.positiveHalfSpaceContains(p)) && (!lowerPlane.positiveHalfSpaceContains(p)) )
      pointsLeft += 1;
    if( (!upperPlane.negativeHalfSpaceContains(p)) && (!lowerPlane.negativeHalfSpaceContains(p)) )
      pointsRight += 1;
  }

  return !( (pointsLeft==8) || (pointsRight==8) );
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
