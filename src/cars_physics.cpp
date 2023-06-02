#include "cars_physics.hpp"

void CarsPhysics::CalculatePhysics(span<Transforn2D> transforms,
                                   span<glm::fvec2> velocities,
                                   span<CarControls> controls) {
  ApplyRotation(transforms, controls);
  ApplyAcceleration(transforms, velocities, controls);
}

void CarsPhysics::ApplyAcceleration(span<Transforn2D> transforms,
                                    span<glm::fvec2> velocities,
                                    span<CarControls> controls) {
  for (int i = 0; i < transforms.size(); i++) {
    Transforn2D &trans = transforms[i];
    float rot = -trans.rot;
    glm::fvec2 dir = {sin(rot), cos(rot)};

    glm::fvec2 &vel = velocities[i];
    velocities[i] *= 0.4;
    velocities[i] += dir * controls[i].acceleration * 0.08f;

    trans.pos += vel;
  }
}

void CarsPhysics::ApplyRotation(span<Transforn2D> transforms,
                                span<CarControls> controls) {
  for (int i = 0; i < transforms.size(); i++) {
    transforms[i].rot += controls[i].rotation / 2;
  }
}
