#pragma once
#include "SunTracker.h"
#include "WavefrontGeometry.h"
#include "tm_r.h"
#include <Eigen/Dense>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class ShadowCalculator {
public:
  ShadowCalculator(const std::vector<WavefrontObject> &objects, const std::vector<Eigen::Vector3d> &vertices, SunTracker &sun, boost::property_tree::ptree &options);
  Eigen::ArrayXXd computeShadow(tm_r tm, double height);
  void growSeasonAverage();
  void monthly();
  void specificMoment();
  void hourly();



private:
  Eigen::Vector3d origin;
  Eigen::Vector3d vector1;
  Eigen::Vector3d vector2;
  Eigen::Vector3d sunDir;
  Eigen::Vector3d ray_origin;
  Eigen::Vector3d z_axis{0.0, 0.0, 1.0};
  int stepsV1;
  int stepsV2;
  int nThreads;
  SunTracker sun;
  boost::property_tree::ptree options;
  const std::vector<WavefrontObject> &objects;
  const std::vector<Eigen::Vector3d> &vertices;

  bool ray_triangle_intersect(Eigen::Vector3d ray_origin,
                            Eigen::Vector3d ray_direction, Eigen::Vector3d v1,
                            Eigen::Vector3d v2, Eigen::Vector3d v3); 
  void checkForDirectory(std::string path);
  void writeEigenArray2DToFile(Eigen::ArrayXXd arr, std::string filename);
  void progressBar(double partDone);
};