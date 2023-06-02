#pragma once
#include "cars_physics.hpp"
#include "nn.hpp"
#include <memory>
#include <span>
#include <string.h>
#include <iostream>

using namespace std;

class CarsAI {
private:
  void ComputeCarContorls(CarControls &controls, shared_ptr<NeuralNetwork> nn,
                          span<float> raycast_results, Transforn2D &transform,
                          glm::fvec2 velocity);

public:
  void ComputeControls(span<CarControls> controls,
                       span<shared_ptr<NeuralNetwork>> nns,
					   int rays_count,
                       span<float> raycast_results,
                       span<Transforn2D> transforms,
                       span<glm::fvec2> velocities);
};
