#include "simulation.hpp"

Simulation::Simulation(SimulationCreateInfo &create_info) {
  map_borders = create_info.map_borders;
  road_collider = create_info.road_collider;
  const_delta_time = create_info.const_delta_time;

  SetRenderers(create_info.renderers);

  Transforn2D cars;
  cars.pos = {0, 0};
  cars.rot = 1;

  raycast_results.resize(10 * rays_layout.size());

  for (int i = 0; i < 10; i++) {
    cars_transforms.push_back(cars);
    cars_velocity.push_back({0.02 * (i + 1), 0});
  }

  CreateRaysLayout();
  CreateNeuralNerworks();
}

void Simulation::SetRenderers(SimulationRenderers &renderers) {
  cars_renderer = renderers.cars_renderer;
  debug_renderer = renderers.debug_renderer;
}

void Simulation::Update() {
  UpdateRaycast();
  UpdateMovement();
  DeleteLosers();
}

void Simulation::UpdateRaycast() {
  raycast_results.resize(cars_transforms.size() * rays_layout.size());

  for (int i = 0; i < cars_transforms.size(); i++) {
    vector<float>::iterator raycast_first, raycast_last;
    GetRaycatsResultsRange(i, raycast_first, raycast_last);
    span<float> raycast_results_span(raycast_first, raycast_last);

    map_borders->Raycast(cars_transforms[i], raycast_results_span);
  }
}

void Simulation::UpdateMovement() {
  for (int i = 0; i < cars_transforms.size(); i++) {
    cars_transforms[i].pos += cars_velocity[i] * const_delta_time;
  }
}

void Simulation::DeleteLosers() {
  for (int i = 0; i < cars_transforms.size(); i++) {
    bool is_on_the_road = road_collider->IsInsideRoad(cars_transforms[i].pos);
    if (!is_on_the_road) {
      DeleteCar(i);
      i--;
    }
  }
}

void Simulation::CreateNeuralNerworks() {
  NeuralNetworkCreateInfo create_info;
  create_info.layers_sizes = {10, 10, 10, 10};

  for (int i = 0; i < cars_transforms.size(); i++) {
    unique_ptr<NeuralNetwork> nn = make_unique<NeuralNetwork>(create_info);
    neural_networks.push_back(move(nn));
  }
}

void Simulation::DeleteCar(int index) {
  cars_velocity.erase(cars_velocity.begin() + index);
  cars_transforms.erase(cars_transforms.begin() + index);

  vector<float>::iterator raycast_first, raycast_last;
  GetRaycatsResultsRange(index, raycast_first, raycast_last);
  raycast_results.erase(raycast_first, raycast_last);
}

void Simulation::GetRaycatsResultsRange(int index,
                                        vector<float>::iterator &first,
                                        vector<float>::iterator &last) {
  first = raycast_results.begin() + index * rays_layout.size();
  last = raycast_results.begin() + (index + 1) * rays_layout.size();
}

void Simulation::Render(bool draw_debug) {
  RenderCars();

  if (draw_debug) {
    RenderRays();
  }
}

void Simulation::RenderCars() { cars_renderer->LoadSprites(cars_transforms); }

void Simulation::RenderRays() {
  CreateRaycastLines();

  debug_renderer->LoadLines(raycast_lines);
}

void Simulation::CreateRaycastLines() {
  raycast_lines.resize(raycast_results.size());

  int pos = 0;
  for (int c = 0; c < cars_transforms.size(); c++) {
    float car_rot = cars_transforms[c].rot;

    float rot_sin = sin(car_rot);
    float rot_cos = cos(car_rot);

    glm::fmat2x2 rot_mat = {{rot_cos, -rot_sin}, {rot_sin, rot_cos}};

    for (int r = 0; r < rays_layout.size(); r++) {
      Line l;
      l.start = cars_transforms[c].pos;

      glm::vec2 dir = rays_layout[r] * rot_mat;
      l.end = l.start + dir * raycast_results[pos];

      raycast_lines[pos] = l;
      pos++;
    }
  }
}

void Simulation::CreateRaysLayout() {
  vector<float> angles;

  angles.push_back(0);
  const float step = 0.03;
  for (float i = step; i < 2; i += step) {
    angles.push_back(i);
    angles.push_back(-i);
  }

  for (auto i : angles) {
    glm::fvec2 ray = {sin(i), cos(i)};
    rays_layout.push_back(ray);
  }

  map_borders->LoadRaysLayout(rays_layout);
}
