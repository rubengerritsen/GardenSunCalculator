#pragma once
#include <Eigen/Dense>

struct WavefrontMaterial {
  Eigen::Vector3d Ka;
  Eigen::Vector3d Kd;
  Eigen::Vector3d Ks;
  Eigen::Vector3d Ke;
  double d = 1.0;
  double Ni = 1.0;
  double illum = 1.0;
  double Ns = 250;
};