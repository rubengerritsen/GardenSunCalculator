#include "SunTracker.h"
#include "WavefrontGeometry.h"
#include <Eigen/Dense>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "tm_r.h"
#include "ShadowCalculator.h"
#include <chrono>


void setupAndRun(int ac, char *av[]) {
  // First we parse the command line arguments
  std::string optionFile;
  try {
    boost::program_options::options_description desc("Possible options");
    desc.add_options()("help,h", "produce help message")(
        "optionfile,o", boost::program_options::value<std::string>(),
        "path to option file");

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(ac, av, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help") || ac == 1) {
      std::cout << "General usage: " << av[0]
                << " -o <path/to/optionfile.xml>\n";
      std::cout << desc << "\n";
    }

    if (vm.count("optionfile")) {
      optionFile = vm["optionfile"].as<std::string>();
      std::cout << "Using optionfile: " << optionFile << ".\n";
    } else {
      std::cout << "No option file specified.\nTrying default option file: "
                   "../input/options.xml\n \n";
      optionFile = "../input/options.xml";
    }

  } catch (std::exception &e) {
    std::cerr << "error: " << e.what() << "\n";
    exit(EXIT_FAILURE);
  } catch (...) {
    std::cerr << "Exception of unknown type!\n";
    exit(EXIT_FAILURE);
  }

  // Next the option file
  boost::property_tree::ptree pt, options;
  read_xml(optionFile, pt);
  options = pt.get_child("options");

  // Load the geometry
  WavefrontGeometry geometry(options.get<std::string>("geometryFile"));
  // Initialize the sun
  SunTracker sun(options.get<double>("latitude"),
                 options.get<double>("longitude"),
                 options.get<double>("timezone"));
  sun.setRelativeRotationAroundZ(options.get<double>("geometryRotation"));

  // Initialize the ShadowCalculator
  ShadowCalculator shadowCalc(geometry.getObjects(), geometry.getVertices(), sun, options);


  // Execute the mode of the options
  auto start = std::chrono::steady_clock::now();
  if(options.get<std::string>("mode") == "growseason"){
    std::cout << "Computing average daily sun exposure over the growseason.\n";
    shadowCalc.growSeasonAverage();
  } else if (options.get<std::string>("mode") == "specificmoment"){
    std::cout << "Computing sun exposure at a specific moment.\n";
    shadowCalc.specificMoment();
  } else if (options.get<std::string>("mode") == "monthly"){
    std::cout << "Computing average daily sun exposure for every month.\n";
    shadowCalc.monthly();
  } else if (options.get<std::string>("mode") == "hourly"){
    std::cout << "Computing average sun exposure for every hour.\n";
    shadowCalc.hourly();
  } else {
    std::cout << options.get<std::string>("mode") << " is not a valid mode.\n";
  }
  auto end = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end-start);

  std::cout << "Garden sun calculator is done.\n";
  std::cout << "The computation took " << duration.count() << "s\n";
  std::cout << "Results can be found in: " << options.get<std::string>("outputPath") << std::endl;
}

int main(int ac, char *av[]) {

  setupAndRun(ac, av);

  return 0;
}
