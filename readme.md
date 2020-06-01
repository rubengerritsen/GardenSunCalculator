# Garden Sun Calculator

This garden sun calculator is an easy tool for the calculation of sun exposure of different parts of a garden or balcony. This exposure can then be used to make better planting plans in garden or balcony designs.

The calculator consists of three parts, there is a wavefront parser to import the garden or balcony geometry, a sun tracker that gives the direction of the sun as a function of time and a very rudimentary ray tracer to compute the sun exposure. Besides the calculator also some python scripts are provided that help with visualizing the results.

## Dependencies and Build
The calculator uses functionaly from the Eigen and Boost libraries, see http://eigen.tuxfamily.org/ and https://www.boost.org/ and for parallelization OpenMP is used.

To build the garden sun calculator:
```bash
git clone https://github.com/rubengerritsen/GardenSunCalculator
cd GardenSunCalculator
mkdir build
cd build
cmake ..
cmake --build .
```
