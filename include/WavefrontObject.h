#pragma once
#include <Eigen/Dense>
#include <array>
#include <string>
#include <vector>
#include <iostream>

class WavefrontObject {
public:
  WavefrontObject(){};

  void pushFace(Eigen::Matrix3d face) { faces.push_back(face); }

  void reset(){
    material = "";
    faces.clear();
  }

  void setMaterial(std::string mat) {
    if (material == "") {
      material = mat;
    } else {
      std::cout << name << std::endl;
      std::cout << "Material already defined, only one material "
                   "per object supported!\nMaterial "
                << material << " will be used.\n";
    }
  }
  void setObjectName(std::string n) { name = n; }
  void setOpacity(double op) { opacity = op; }

  double getOpacity() const {return opacity;}

  const std::vector<Eigen::Matrix3d>& getFaces() const {return faces;}

  void printToConsole() const {
    std::cout << name << ": has opacity " << opacity << " and material " << material << " and #faces " << faces.size() << std::endl;
  }

private:
  std::vector<Eigen::Matrix3d> faces;
  std::string name = "empty";
  std::string material = "";
  float opacity = 1.0;
};