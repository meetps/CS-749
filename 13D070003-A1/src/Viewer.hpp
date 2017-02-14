#ifndef __A1_Viewer_hpp__
#define __A1_Viewer_hpp__

#include "Common.hpp"
#include "PointCloud.hpp"
#include "DGP/AffineTransform3.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Camera.hpp"
#include "DGP/Matrix3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"

/* Displays an object using OpenGL and GLUT. */
class Viewer
{
  private:
    static Graphics::RenderSystem * render_system;
    static PointCloud * pcloud;
    static std::vector<Slab> slabs;

    static int width;
    static int height;
    static Camera camera;
    static Vector3 camera_look_at;
    static bool dragging;
    static int modifier_keys;
    static int last_x, last_y;
    static int drag_start_x, drag_start_y;
    static bool show_normals;
    static bool show_bbox;

  public:
    /** Set the object to be displayed. The object must persist as long as the viewer does. */
    static void setObject(PointCloud * o);

    /** Set the slabs to be displayed. */
    static void setSlabs(std::vector<Slab> const & slabs_);

    /**
     * Call this function to launch the viewer. It will not return under normal circumstances, so make sure stuff is set up
     * before you call it!
     */
    static void launch(int argc, char * argv[]);

  private:
    /** Callback for drawing the object. */
    static void draw();

    /** Callback when window is resized. */
    static void reshape(int w, int h);

    /** Callback when a key is pressed. */
    static void keyPress(unsigned char key, int x, int y);

    /** Callback when a mouse button is pressed. */
    static void mousePress(int button, int state, int x, int y);

    /** Callback when the mouse moves with a button pressed. */
    static void mouseMotion(int x, int y);

    /** Draw a bounding box as an outline. */
    static void drawOutlineBox(AxisAlignedBox3 const & bbox);

    /** Position the camera to center the object and fit it in the frame, without changing orientation. */
    static void fitCameraToObject();

    /** Update the camera view by adding an additional transform. */
    static void incrementViewTransform(AffineTransform3 const & tr);

}; // class Viewer

#endif
