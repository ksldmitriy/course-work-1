#include "cars_ai.hpp"

void CarsAI::ComputeControls(span<CarControls> controls,
                             span<shared_ptr<NeuralNetwork>> nns,
                             int rays_count, span<float> raycast_results,
                             span<Transforn2D> transforms,
                             span<glm::fvec2> velocities) {

  for (int i = 0; i < controls.size(); i++) {
    span<float>::iterator raycast_first =
        raycast_results.begin() + i * rays_count;
    span<float>::iterator raycast_last =
        raycast_results.begin() + (i + 1) * rays_count;
    span<float> raycast_for_car(raycast_first, raycast_last);

    ComputeCarContorls(controls[i], nns[i], raycast_for_car, transforms[i],
                       velocities[i]);
  }
}

void CarsAI::ComputeCarContorls(CarControls &controls,
                                shared_ptr<NeuralNetwork> nn,
                                span<float> raycast_results,
                                Transforn2D &transform, glm::fvec2 velocity) {
  vector<float> &input = nn->GetInputLayer();

  float rot_sin = sin(transform.rot);
  float rot_cos = cos(transform.rot);

  glm::fmat2x2 rot_mat = {{rot_cos, -rot_sin}, {rot_sin, rot_cos}};

  glm::fvec2 relative_velocity = velocity * rot_mat;

  input[0] = relative_velocity.x;
  input[1] = relative_velocity.y;

  memcpy((char *)(input.data() + 2), raycast_results.data(),
         raycast_results.size() * sizeof(float));

  nn->Run();

  vector<float> output = nn->GetOutputLayer();
  controls.acceleration = output[0];
  controls.rotation = output[1];
}
