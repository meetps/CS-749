#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "MeshVertex.hpp"
#include "DGP/Vector4.hpp"

#define EPSILON 1e-6

MeshEdge *
MeshEdge::nextAroundEndpoint(int i)
{
  debugAssertM(i == 0 || i == 1, "MeshEdge: Invalid endpoint index");

  if (numFaces() > 2)  // non-manifold
    return NULL;

  // Find which incident face has this endpoint as the origin of the edge when stepping round the face. The required edge
  // is then the predecessor of this edge around the face.
  for (FaceIterator fi = facesBegin(); fi != facesEnd(); ++fi)
  {
    Face * face = *fi;
    MeshEdge * prev = face->getPredecessor(this);
    if (prev->hasEndpoint(endpoints[i]))  // found it!
      return prev;
  }

  return NULL;
}

void
MeshEdge::updateQuadricCollapseError()
{
  // Update both quadric_collapse_error and quadric_collapse_position, using the existing endpoint quadrics and the method of
  // Garland/Heckbert.
  Vertex* a = getEndpoint(0);
  Vertex* b = getEndpoint(1);

  DMat4 q_ = a->getQuadric() + b->getQuadric();
  DMat4 q_new = q_;

  Vector4 unit_vector_4 = Vector4(0.0, 0.0, 0.0, 1.0);
  q_new.setRow(3, unit_vector_4);
  // NOTE: Remember to check if the quadric Q' is invertible. If not, you will have to use a fallback option such as the
  // midpoint of the edge (or in the worst case, set the error to a negative value to indicate this edge should not be
  // collapsed).

  // Check invertibility of the q_new
  double det = q_new.determinant();
  bool is_invertible = abs(det) >= EPSILON;

  // Update the collapse position if invertible
  if(is_invertible)
  {
    auto productVector = q_new.inverse() * unit_vector_4;
    for(int i=0; i<3; i++)
    {
      quadric_collapse_position[i] = productVector[i];
    }  
  }
  // Else just take the average
  else
  {
    quadric_collapse_position = (a->getPosition() + b->getPosition())/2.0;
  }

  // Update error now
  quadric_collapse_error = Vector4(quadric_collapse_position,1).dot(q_*Vector4(quadric_collapse_position,1));
}
