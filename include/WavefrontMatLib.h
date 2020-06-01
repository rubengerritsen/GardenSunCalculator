#pragma once
#include "WavefrontMaterial.h"
#include "WavefrontObject.h"
#include <Eigen/Dense>
#include <map>
#include <string>
#include <vector>

class WavefrontMatLib {
public:
  void loadMaterialLibrary(std::string filepath);

  bool matExists(std::string mat) { return (material_library.count(mat) > 0); };
  double getOpacity(std::string mat) { return material_library[mat].d; }

private:
  std::map<std::string, WavefrontMaterial> material_library;
};
