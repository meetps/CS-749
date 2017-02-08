#include "PointCloud.hpp"
#include "Viewer.hpp"
#include <cstdlib>

int
usage(int argc, char * argv[])
{
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Usage: " << argv[0] << " [OPTIONS] <points-in> [<points-out>]";
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Options:";
  DGP_CONSOLE << "--ransac <N> <iters> <thickness> <min-pts> :";
  DGP_CONSOLE << "                 Extract N planes using RANSAC with the given parameters";
  DGP_CONSOLE << "--downsample  :  Adaptively downsample the points based on the planes";
  DGP_CONSOLE << "                 detected by the RANSAC operation (needs --ransac)";
  DGP_CONSOLE << "";

  return -1;
}

int
main(int argc, char * argv[])
{
  if (argc < 2)
    return usage(argc, argv);

  bool ransac = false;
  long num_planes = -1;
  long num_iters = -1;
  Real thickness = 0;
  long min_points = -1;

  bool downsample = false;

  std::string in_path;
  std::string out_path;

  int pos_arg = 0;
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg == "--ransac")
    {
      ransac = true;
      if (i + 4 >= argc)
        return usage(argc, argv);

      num_planes = std::atol(argv[++i]);
      num_iters  = std::atol(argv[++i]);
      thickness  = std::atof(argv[++i]);
      min_points = std::atol(argv[++i]);

      DGP_CONSOLE << "RANSAC parameters: N = " << num_planes << ", iters = " << num_iters << ", thickness = " << thickness
                  << ", min-pts = " << min_points;
    }
    else if (arg == "--downsample")
    {
      downsample = true;

      DGP_CONSOLE << "Downsample = true";
    }
    else if (pos_arg == 0)
    {
      in_path = arg;
      pos_arg++;
    }
    else if (pos_arg == 1)
    {
      out_path = arg;
      pos_arg++;
    }
    else
      return usage(argc, argv);
  }

  if (pos_arg <= 0)
    return usage(argc, argv);

  if (downsample && !ransac)
  {
    DGP_ERROR << "Downsampling requires plane detection via RANSAC first. Specify the --ransac argument\n";
    return usage(argc, argv);
  }

  PointCloud pcloud;
  if (!pcloud.load(in_path))
    return -1;

  DGP_CONSOLE << "Read " << pcloud.numPoints() << " points from " << in_path;

  std::vector<Slab> slabs;
  bool save = false;
  if (ransac)
  {
    Real abs_thickness = thickness * pcloud.getAABB().getExtent().length();
    pcloud.ransacMultiple(num_planes, num_iters, abs_thickness, min_points, slabs);

    DGP_CONSOLE << "";
    DGP_CONSOLE << "[RANSAC] Detected " << slabs.size() << " plane(s)";
    for (size_t i = 0; i < slabs.size(); ++i)
      DGP_CONSOLE << "[RANSAC]   - Plane " << i << ": " << slabs[i].getPlane().toString();

    DGP_CONSOLE << "";

    if (downsample)
    {
      pcloud.adaptiveDownsample(slabs);
      save = true;
    }
  }

  if (save && !out_path.empty())
  {
    if (!pcloud.save(out_path))
      return -1;

    DGP_CONSOLE << "Saved point cloud to " << out_path;
  }

  Viewer viewer;
  viewer.setObject(&pcloud);
  viewer.setSlabs(slabs);
  viewer.launch(argc, argv);

  return 0;
}
