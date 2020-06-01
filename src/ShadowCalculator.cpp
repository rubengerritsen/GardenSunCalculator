#include "ShadowCalculator.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <omp.h> // OpenMP functions and pragmas

ShadowCalculator::ShadowCalculator(const std::vector<WavefrontObject> &objects,
                                   const std::vector<Eigen::Vector3d> &vertices,
                                   SunTracker &sun,
                                   boost::property_tree::ptree &options)
    : objects(objects), vertices(vertices), sun(sun), options(options) {
  stepsV1 = options.get<int>("stepsV1");
  stepsV2 = options.get<int>("stepsV2");

  nThreads = options.get<int>("nrOfThreads");

  std::stringstream originstream(options.get_child("regionO").data());
  std::stringstream vector1stream(options.get_child("regionV1").data());
  std::stringstream vector2stream(options.get_child("regionV2").data());

  double x, y, z;
  originstream >> x >> y >> z;
  origin << x, y, z;
  vector1stream >> x >> y >> z;
  vector1 << x, y, z;
  vector2stream >> x >> y >> z;
  vector2 << x, y, z;
}

void ShadowCalculator::writeEigenArray2DToFile(Eigen::ArrayXXd arr,
                                               std::string filename) {
  std::ofstream outFile;
  outFile.open(filename);

  if (outFile.is_open()) {
    for (int i = 0; i < arr.rows(); i++) {
      for (int j = 0; j < arr.cols(); j++) {
        outFile << boost::format("%6.2f ") % arr(i, j);
      }
      outFile << "\n";
    }
  } else {
    std::cout << "Could not open output file: " << filename << "\n";
  }
}

void ShadowCalculator::checkForDirectory(std::string foldername) {
  boost::filesystem::path dir(foldername);
  if (!boost::filesystem::exists(dir)) {
    if (!boost::filesystem::create_directory(dir)) {
      std::cout
          << "Was not able to create output directory, terminating program\n";
      exit(EXIT_FAILURE);
    }
  }
}

void ShadowCalculator::progressBar(double partDone) {
  std::cout << boost::format("\rprogress: %6.2f%%") % (partDone * 100)
            << std::flush;
}

void ShadowCalculator::growSeasonAverage() {
  // Setup folder for the output
  checkForDirectory(options.get<std::string>("outputPath"));
  std::string outputDir =
      options.get<std::string>("outputPath") + "/growseason";
  checkForDirectory(outputDir);

  int iterations = 0;

  tm_r tm;
  tm.year = options.get<int>("date.year");

  // Doing the calculations
  Eigen::ArrayXXd cumSum;
  std::string outputFile;
  double maxHeight = options.get<double>("maxHeight");
  double increment = options.get<double>("heightIncr");
  for (double height = 0; height < maxHeight; height += increment) {
    iterations = 0;
    cumSum = Eigen::ArrayXXd::Zero(stepsV1, stepsV2);
    for (tm.month = 5; tm.month < 10; tm.month++) {
      progressBar((height / maxHeight +
                   increment * (tm.month - 5.0) / 5.0 / maxHeight));
      for (tm.day = 1; tm.day < 31; tm.day++) {
        for (tm.hour = 0; tm.hour < 24; tm.hour++) {
          for (tm.min = 0; tm.min < 60; tm.min += 5) {
            cumSum += computeShadow(tm, height);
            iterations++;
          }
        }
      }
    }

    cumSum = 24.0 * cumSum / iterations;
    // write the results to a file
    outputFile =
        outputDir + (boost::format("/height_%3.0f.txt") % (height * 100)).str();
    writeEigenArray2DToFile(cumSum, outputFile);
  }
  std::cout << std::endl; // end line after progress bar
}

void ShadowCalculator::monthly() {
  // Setup folder for the output
  checkForDirectory(options.get<std::string>("outputPath"));
  std::string outputDir = options.get<std::string>("outputPath") + "/monthly";
  checkForDirectory(outputDir);

  int iterations = 0;
  tm_r tm;
  tm.year = options.get<int>("date.year");

  Eigen::ArrayXXd cumSum;
  std::string outputFile;
  double maxHeight = options.get<double>("maxHeight");
  double increment = options.get<double>("heightIncr");

  // Doing the calculations
  for (tm.month = 1; tm.month <= 12; tm.month++) {
    for (double height = 0; height < maxHeight; height += increment) {
      iterations = 0;
      cumSum = Eigen::ArrayXXd::Zero(stepsV1, stepsV2);
      progressBar(((tm.month - 1.0) / 12 + height / maxHeight / 12));
      for (tm.day = 1; tm.day < 31; tm.day++) {
        for (tm.hour = 0; tm.hour < 24; tm.hour++) {
          for (tm.min = 0; tm.min < 60; tm.min += 5) {
            cumSum += computeShadow(tm, height);
            iterations++;
          }
        }
      }
      cumSum = 24.0 * cumSum / iterations;
      // write the results to a file
      outputFile = outputDir + (boost::format("/month_%d_height_%3.0f.txt") %
                                tm.month % (height * 100))
                                   .str();
      writeEigenArray2DToFile(cumSum, outputFile);
    }
  }
  std::cout << std::endl; // end line after progress bar
}

void ShadowCalculator::specificMoment() {
  // Setup folder for the output
  checkForDirectory(options.get<std::string>("outputPath"));
  std::string outputDir =
      options.get<std::string>("outputPath") + "/specificmoment";
  checkForDirectory(outputDir);

  int iterations = 0;
  tm_r tm;
  tm.year = options.get<int>("date.year");
  tm.month = options.get<int>("month");
  tm.day = options.get<int>("day");
  tm.hour = options.get<int>("hour");
  tm.min = options.get<int>("minute");

  Eigen::ArrayXXd cumSum;
  std::string outputFile;
  double maxHeight = options.get<double>("maxHeight");
  double increment = options.get<double>("heightIncr");

  // Doing the calculations
  for (double height = 0; height < maxHeight; height += increment) {
    outputFile =
        outputDir + (boost::format("/%d%d%d_%d%d_height_%3.0f.txt") % tm.year %
                     tm.month % tm.day % tm.hour % tm.min % (height * 100))
                        .str();
    writeEigenArray2DToFile(computeShadow(tm, height), outputFile);
  }
}

void ShadowCalculator::hourly() {
  // Setup folder for the output
  checkForDirectory(options.get<std::string>("outputPath"));
  std::string outputDir =
      options.get<std::string>("outputPath") + "/hourly";
  checkForDirectory(outputDir);

  int iterations = 0;

  tm_r tm;
  tm.year = options.get<int>("date.year");
  tm.month = options.get<int>("date.month");
  tm.day = options.get<int>("date.day");

  // Doing the calculations
  Eigen::ArrayXXd cumSum;
  std::string outputFile;
  double maxHeight = options.get<double>("maxHeight");
  double increment = options.get<double>("heightIncr");
  for (tm.hour = 0; tm.hour < 24; tm.hour++) {
    for (double height = 0; height < maxHeight; height += increment) {
      progressBar(1.0 * tm.hour / 24.0 + (height + increment) / maxHeight / 24);
      iterations = 0;
      cumSum = Eigen::ArrayXXd::Zero(stepsV1, stepsV2);
      for (tm.min = 0; tm.min < 60; tm.min++) {
        cumSum += computeShadow(tm, height);
        iterations++;
      }
      outputFile =
          outputDir + (boost::format("/%d%d%d_h%d_height_%3.0f.txt") % tm.year %
                       tm.month % tm.day % tm.hour % (height * 100))
                          .str();
      writeEigenArray2DToFile(cumSum, outputFile);
    }
  }
  std::cout << std::endl; // for the progress bar
}

Eigen::ArrayXXd ShadowCalculator::computeShadow(tm_r tm, double height) {
  Eigen::ArrayXXd sunCollector = Eigen::ArrayXXd::Zero(stepsV1, stepsV2);

  double lightGoingThrough = 1.0;
  sunDir = sun.getSunDirection(tm);
#pragma omp parallel for num_threads(nThreads) private(                        \
    lightGoingThrough, ray_origin) shared(sunCollector, height)
  for (int i = 0; i < stepsV1; i++) {
    for (int j = 0; j < stepsV2; j++) {
      // why (i+0.5):  0.5 gets us to the center of a cell
      ray_origin = origin + (vector1 - origin) / stepsV1 * (i + 0.5) +
                   (vector2 - origin) * (j + 0.5) / stepsV2 + height * z_axis;
      lightGoingThrough = 1.0;
      for (auto &obj : objects) {
        for (auto &face : obj.getFaces()) {
          if (ray_triangle_intersect(
                  ray_origin, sunDir, vertices[face(0, 0) - 1],
                  vertices[face(1, 0) - 1], vertices[face(2, 0) - 1])) {
            if (lightGoingThrough > (1.0 - obj.getOpacity())) {
              lightGoingThrough = 1.0 - obj.getOpacity();
            }
          }
        }
      }
      sunCollector(i, j) += lightGoingThrough;
    }
  }
  return sunCollector;
}

bool ShadowCalculator::ray_triangle_intersect(Eigen::Vector3d ray_origin,
                                              Eigen::Vector3d ray_direction,
                                              Eigen::Vector3d v1,
                                              Eigen::Vector3d v2,
                                              Eigen::Vector3d v3) {
  double eps = 1e-6;
  Eigen::Vector3d edge1 = v2 - v1;
  Eigen::Vector3d edge2 = v3 - v1;
  Eigen::Vector3d h = ray_direction.cross(edge2);
  double a = edge1.dot(h);

  if (abs(a) < eps) {
    return false;
  }

  double f = 1.0 / a;
  Eigen::Vector3d s = ray_origin - v1;
  double u = f * s.dot(h);

  if (u < 0.0 || u > 1.0) {
    return false;
  }

  Eigen::Vector3d q = s.cross(edge1);
  double v = f * ray_direction.dot(q);

  if ((v < 0.0) || (u + v) > 1.0) {
    return false;
  }

  double t = f * edge2.dot(q);

  if (t > eps) {
    return true;
  } else {
    return false;
  }
}