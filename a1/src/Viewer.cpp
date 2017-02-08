#include "Viewer.hpp"
#include "DGP/Graphics/RenderSystem.hpp"

#ifdef DGP_OSX
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

Graphics::RenderSystem * Viewer::render_system = NULL;
PointCloud * Viewer::pcloud = NULL;
std::vector<Slab> Viewer::slabs;
int Viewer::width = 640;
int Viewer::height = 480;
Camera Viewer::camera;
Vector3 Viewer::camera_look_at = -Vector3::unitZ();
bool Viewer::dragging = false;
int Viewer::modifier_keys = 0;
int Viewer::last_x = -1;
int Viewer::last_y = -1;
int Viewer::drag_start_x = -1;
int Viewer::drag_start_y = -1;
bool Viewer::show_normals = false;
bool Viewer::show_bbox = false;

Matrix3
dragToRotation(int x1, int y1, int x2, int y2, int width, int height, Camera const & camera)
{
  static Real const ROT_SPEED = 5;

  // Pixel coordinates increase top to bottom, so dy is downwards in camera space
  int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0)
    return Matrix3::identity();

  Vector3 axis = dy * camera.getRightDirection() + dx * camera.getUpDirection();

  int size = (width < height ? width : height);
  Real angle = ROT_SPEED * Vector2(dx, -dy).length() / size;

  return Matrix3::rotationAxisAngle(axis, angle);
}

Real
dragToScale(int x1, int y1, int x2, int y2, int width, int height, Camera const & camera)
{
  static Real const SCALE_INC_SPEED = 4;
  static Real const SCALE_DEC_SPEED = 2;
  static Real const MIN_SCALE = 0.25;

  // Pixel coordinates increase top to bottom, so dy is downwards in camera space
  int dy = y2 - y1;
  if (dy < 0)  // drag up => increase
    return std::max(1 - SCALE_INC_SPEED * dy / (Real)height, MIN_SCALE);
  else  // drag down => decrease
    return std::max(1 - SCALE_DEC_SPEED * dy / (Real)height, MIN_SCALE);
}

AffineTransform3
zoomTransform(Real zoom, Real camera_separation, Vector3 const & zoom_dir)
{
  static Real const MIN_ZOOM = 0.25;
  Vector3 trn = (1.0f / std::max(zoom, MIN_ZOOM) - 1) * camera_separation * zoom_dir;
  return AffineTransform3(Matrix3::identity(), trn);
}

void
Viewer::fitCameraToObject()
{
  static Real const DIST = 10;
  static Real const NEAR = 1.7f;

  if (!pcloud)
    return;

  Real aspect_ratio = (width / (Real)height);
  Real left, right, bottom, top;
  if (aspect_ratio > 1)
  {
    left = -aspect_ratio;
    right = aspect_ratio;
    bottom = -1;
    top = 1;
  }
  else
  {
    left = -1;
    right = 1;
    bottom = -1.0f / aspect_ratio;
    top = 1.0f / aspect_ratio;
  }

  AxisAlignedBox3 const & bbox = pcloud->getAABB();
  camera_look_at = bbox.getCenter();
  Real scale = bbox.getExtent().length();
  Real camera_separation = DIST * scale;
  CoordinateFrame3 cframe = camera.getFrame();
  cframe.setTranslation(camera_look_at - camera_separation * camera.getLookDirection());

  camera.set(cframe,
             Camera::ProjectionType::PERSPECTIVE,
             (left / DIST) * scale,
             (right / DIST) * scale,
             (bottom / DIST) * scale,
             (top / DIST) * scale,
             NEAR * scale,
             camera_separation + 1000 * scale,
             Camera::ProjectedYDirection::UP);
}

void
Viewer::setObject(PointCloud * o)
{
  pcloud = o;
  fitCameraToObject();
}

void
Viewer::setSlabs(std::vector<Slab> const & slabs_)
{
  slabs = slabs_;
}

void
Viewer::launch(int argc, char * argv[])
{
  // Create a GL context via a GLUT window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("A1::Viewer");

  // Create a rendersystem
  render_system = new Graphics::RenderSystem("RenderSystem");
  DGP_CONSOLE << render_system->describeSystem();

  // Set up callbacks
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyPress);
  glutMouseFunc(mousePress);
  glutMotionFunc(mouseMotion);

  // Start event processing loop
  glutMainLoop();
}

void
Viewer::draw()
{
  alwaysAssertM(render_system, "Rendersystem not created");

  render_system->setColorClearValue(ColorRGB(0.25, 0.25, 0.25));
  render_system->clear();

  if (pcloud)
  {
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::MODELVIEW); render_system->pushMatrix();
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::PROJECTION); render_system->pushMatrix();

      render_system->setCamera(camera);

      Real scale = pcloud->getAABB().getExtent().length();
      pcloud->draw(*render_system, (show_normals ? 0.05 * scale : -1));

      if (show_bbox)
      {
        render_system->setColor(ColorRGB(1, 1, 0));
        drawOutlineBox(pcloud->getAABB());
      }

      for (size_t i = 0; i < slabs.size(); ++i)
        slabs[i].draw(*render_system);

    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::PROJECTION); render_system->popMatrix();
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::MODELVIEW); render_system->popMatrix();
  }

  glutSwapBuffers();
}

void
Viewer::reshape(int w, int h)
{
  width = w;
  height = h;
  glViewport(0, 0, width, height);
  fitCameraToObject();

  glutPostRedisplay();
}

void
Viewer::keyPress(unsigned char key, int x, int y)
{
  if (key == 27)
  {
    exit(0);
  }
  else if (key == 'b' || key == 'B')
  {
    show_bbox = !show_bbox;
    glutPostRedisplay();
  }
  else if (key == 'n' || key == 'N')
  {
    show_normals = !show_normals;
    glutPostRedisplay();
  }
  else if (key == 'f' || key == 'F')
  {
    fitCameraToObject();
    glutPostRedisplay();
  }
}

void
Viewer::incrementViewTransform(AffineTransform3 const & tr)
{
  AffineTransform3 inv_vt = tr.inverse();
  CoordinateFrame3 const & old_cframe = camera.getFrame();
  camera.setFrame(CoordinateFrame3(RigidTransform3::_fromAffine(AffineTransform3(inv_vt.getLinear() * old_cframe.getRotation(),
                                                                                 inv_vt * old_cframe.getTranslation()))));

  glutPostRedisplay();
}

void
Viewer::mousePress(int button, int state, int x, int y)
{
  dragging = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
  modifier_keys = glutGetModifiers();

  last_x = x;
  last_y = y;

  if (dragging)
  {
    drag_start_x = x;
    drag_start_y = y;
  }
}

void
Viewer::mouseMotion(int x, int y)
{
  if (!dragging)
    return;

  if (modifier_keys == GLUT_ACTIVE_SHIFT)
  {
    Real zoom = dragToScale(last_x, last_y, x, y, width, height, camera);

    // Zoom in at mouse position, zoom out at screen center
    Vector3 dir;
    if (zoom > 1)
    {
      Vector2 drag_pos(2 * drag_start_x / (Real)width - 1, 1 - 2 * drag_start_y / (Real)height);
      dir = camera.computePickRay(drag_pos).getDirection().unit();
    }
    else
      dir = camera.getLookDirection();

    Real camera_separation = (camera_look_at - camera.getPosition()).length();
    incrementViewTransform(zoomTransform(zoom, camera_separation, dir));
  }
  else
  {
    Matrix3 rot = dragToRotation(last_x, last_y, x, y, width, height, camera);
    Vector3 trn = camera_look_at - rot * camera_look_at;
    incrementViewTransform(AffineTransform3(rot, trn));
  }

  last_x = x;
  last_y = y;
}

void
Viewer::drawOutlineBox(AxisAlignedBox3 const & bbox)
{
  if (!render_system)
    return;

  render_system->pushShader();
    render_system->setShader(NULL);

    render_system->beginPrimitive(Graphics::RenderSystem::Primitive::LINES);
      Vector3 p, q;
      for (int i = 0; i < 12; ++i)
      {
        bbox.getEdge(i, p, q);
        render_system->sendVertex(p);
        render_system->sendVertex(q);
      }
    render_system->endPrimitive();

  render_system->popShader();
}
