#pragma once
#include "render_structs.hpp"
#include <span>

using namespace std;

struct CarControls {
  float acceleration;
  float rotation;
};

class CarsPhysics {
private:
  void ApplyAcceleration(span<Transforn2D> transforms,
                         span<glm::fvec2> velocities,
                         span<CarControls> controls);
  void ApplyRotation(span<Transforn2D> transforms, span<CarControls> controls);

public:
  void CalculatePhysics(span<Transforn2D> transforms,
                        span<glm::fvec2> velocities,
                        span<CarControls> controls);
};
