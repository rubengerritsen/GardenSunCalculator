#include "WavefrontGeometry.h"
#include "WavefrontMaterial.h"
#include "stringtools.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

WavefrontGeometry::WavefrontGeometry(std::string filepath) {
  // Check for file type
  if (filepath.substr(filepath.find_last_of('.')) != ".obj") {
    std::cout << "File: " << filepath << "\nis not a wavefront .obj file.\n";
    std::cout << "Terminating program execution\n";
    exit(EXIT_FAILURE);
  }

  std::ifstream file(filepath);
  if (file.is_open()) {
    std::cout << "Started loading geometry from: " << filepath << std::endl;
    std::string line;
    std::string temp;
    double x, y, z;
    bool firstObject = true;
    WavefrontObject tempObj;
    WavefrontMatLib mat;
    Eigen::Vector3d tempVert;
    Eigen::Vector2d tempText;
    Eigen::Matrix3d tempFace;
    std::string delimiters("/ ");
    std::vector<std::string> parts;

    while (std::getline(file, line)) {
      if (line[0] != '#') { // ignore comments
        std::istringstream iss(line);
        if (stringtools::firstToken(line) == "mtllib") {
          iss >> temp >> temp;
          mat.loadMaterialLibrary(
              stringtools::changeFileNameInPath(filepath, temp));
        } else if (stringtools::firstToken(line) == "o") {
          if (firstObject) {
            firstObject = false;
          } else {
            objects.push_back(tempObj);
          }
          iss >> temp >> temp;
          tempObj.reset();
          tempObj.setObjectName(temp);
        } else if (stringtools::firstToken(line) == "v") {
          iss >> temp >> x >> y >> z;
          tempVert << x, y, z;
          vertices.push_back(tempVert);
        } else if (stringtools::firstToken(line) == "vt") {
          iss >> temp >> x >> y;
          tempText << x, y;
          textures.push_back(tempText);
        } else if (stringtools::firstToken(line) == "vn") {
          iss >> temp >> x >> y >> z;
          tempVert << x, y, z;
          normals.push_back(tempVert);
        } else if (stringtools::firstToken(line) == "f") {
          parts.clear();
          boost::split(parts, line, boost::is_any_of(delimiters));
          if (parts.size() == 10) { // a triangle
            tempFace << std::stod(parts[1]), std::stod(parts[2]),
                std::stod(parts[3]), std::stod(parts[4]), std::stod(parts[5]),
                std::stod(parts[6]), std::stod(parts[7]), std::stod(parts[8]),
                std::stod(parts[9]);
            tempObj.pushFace(tempFace);
          } else if (parts.size() == 13) { // a square
            // Triangle 1
            tempFace << std::stod(parts[1]), std::stod(parts[2]),
                std::stod(parts[3]), std::stod(parts[4]), std::stod(parts[5]),
                std::stod(parts[6]), std::stod(parts[7]), std::stod(parts[8]),
                std::stod(parts[9]);
            tempObj.pushFace(tempFace);
            // Triangle 2
            tempFace << std::stod(parts[1]), std::stod(parts[2]),
                std::stod(parts[3]), std::stod(parts[10]), std::stod(parts[11]),
                std::stod(parts[12]), std::stod(parts[7]), std::stod(parts[8]),
                std::stod(parts[9]);
            tempObj.pushFace(tempFace);
          } else { // 'higher order' shapes such as pentagons etc.
            std::cout
                << "Wavefront parser can not parse faces with more than 4 "
                   "vertices.\n"
                << "Please use other software to triangulate the faces first\n"
                << "e.g. in Blender in edit mode press 'Face -> Triangulate "
                   "Faces' (Ctrl T) \n"
                << "Terminating program for now.\n";
            exit(EXIT_FAILURE);
          }

        } else if (stringtools::firstToken(line) == "usemtl") {
          iss >> temp >> temp;
          tempObj.setMaterial(temp);
          if (mat.matExists(temp)) {
            tempObj.setOpacity(mat.getOpacity(temp));
          }
        }
      }
    }
    objects.push_back(tempObj); // push back last geometry
    std::cout << "Done loading geometry.\n\n";

  } else {
    std::cout << "Was unable to open wavefront (.obj) file: " << filepath
              << std::endl;
    std::cout << "Terminating program.\n";
    exit(EXIT_FAILURE);
  }
}

void WavefrontMatLib::loadMaterialLibrary(std::string filepath) {
  // Check for file type
  if (filepath.substr(filepath.find_last_of('.')) != ".mtl") {
    std::cout << "File: " << filepath
              << "\n is not a wavefront material (.mtl) file.\n"
              << "Terminating program execution\n";
    exit(EXIT_FAILURE);
  }

  std::ifstream file(filepath);
  if (file.is_open()) {
    std::cout << "Loading materials from: " << filepath << std::endl;
    std::string line;
    std::string temp;
    std::string currentMat;
    double x, y, z;
    bool firstMaterial = true;
    WavefrontMaterial tempMat;
    Eigen::Vector3d tempVector;

    while (std::getline(file, line)) {
      if (line[0] != '#') { // ignore comments
        std::istringstream iss(line);
        if (stringtools::firstToken(line) == "newmtl") {
          iss >> temp >> currentMat;
          material_library[currentMat] = tempMat;
        } else if (stringtools::firstToken(line) == "Ns") {
          iss >> temp >> x;
          material_library[currentMat].Ns = x;
        } else if (stringtools::firstToken(line) == "Ni") {
          iss >> temp >> x;
          material_library[currentMat].Ni = x;
        } else if (stringtools::firstToken(line) == "d") {
          iss >> temp >> x;
          material_library[currentMat].d = x;
        } else if (stringtools::firstToken(line) == "illum") {
          iss >> temp >> x;
          material_library[currentMat].illum = x;
        } else if (stringtools::firstToken(line) == "Ka") {
          iss >> temp >> x >> y >> z;
          material_library[currentMat].Ka << x, y, z;
          ;
        } else if (stringtools::firstToken(line) == "Kd") {
          iss >> temp >> x >> y >> z;
          material_library[currentMat].Kd << x, y, z;
          ;
        } else if (stringtools::firstToken(line) == "Ks") {
          iss >> temp >> x >> y >> z;
          material_library[currentMat].Ks << x, y, z;
          ;
        } else if (stringtools::firstToken(line) == "Ke") {
          iss >> temp >> x >> y >> z;
          material_library[currentMat].Ke << x, y, z;
          ;
        }
      }
    }

  } else {
    std::cout << "Was unable to open wavefront materials (.mtl) file: "
              << filepath << std::endl;
    std::cout << "Terminating program.\n";
    exit(EXIT_FAILURE);
  }
}
