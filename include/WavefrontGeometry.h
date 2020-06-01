#pragma once
#include "WavefrontMatLib.h"
#include "WavefrontObject.h"
#include <Eigen/Dense>
#include <string>
#include <vector>

class WavefrontGeometry {
public:
  WavefrontGeometry(std::string filepath);

  const std::vector<WavefrontObject>& getObjects() const { return objects; }
  const std::vector<Eigen::Vector3d>& getVertices() const { return vertices; }

private:
  std::vector<WavefrontObject> objects;
  std::vector<Eigen::Vector3d> vertices;
  std::vector<Eigen::Vector2d> textures;
  std::vector<Eigen::Vector3d> normals;
};
