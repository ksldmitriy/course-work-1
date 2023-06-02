#include "simulation.hpp"

Simulation::Simulation(SimulationCreateInfo &create_info) {
  map_borders = create_info.map_borders;
  road_collider = create_info.road_collider;
  const_delta_time = create_info.const_delta_time;

  SetRenderers(create_info.renderers);

  Transforn2D start_pos;
  start_pos.pos = {0, 0};
  start_pos.rot = -1.9;

  const int cars_count = 100;
  raycast_results.resize(cars_count * rays_layout.size());
  cars_controls.resize(cars_count);
  cars_rotation.resize(cars_count, 0);

  for (int i = 0; i < cars_count; i++) {
    cars_distance.push_back(0);
    cars_transforms.push_back(start_pos);
    cars_velocity.push_back({});
    cars_controls[i] = {};
  }

  CreateRaysLayout();
  CreateNeuralNerworks();
}

void Simulation::SetRenderers(SimulationRenderers &renderers) {
  cars_renderer = renderers.cars_renderer;
  debug_renderer = renderers.debug_renderer;
}

float Simulation::GetBestResult() { return last_best_result; }

void Simulation::Update() {
  time_from_start++;

  UpdateRaycast();
  CreateRaycastLines();

  UpdateControls();
  UpdateMovement();
  DeleteLosers();

  for (auto &nn : neural_networks) {
    nn->Run();
  }
}

void Simulation::UpdateControls() {
  cars_ai.ComputeControls(cars_controls, neural_networks, rays_layout.size(),
                          raycast_results, cars_transforms, cars_velocity);
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
  cars_physics.CalculatePhysics(cars_transforms, cars_velocity, cars_controls);

  for (int i = 0; i < cars_velocity.size(); i++) {
    float rot = cars_transforms[i].rot;
    glm::fvec2 dir = {-sin(rot), cos(rot)};

    float forward_speed = glm::dot(cars_velocity[i], dir) / glm::length(dir);

    cars_distance[i] += forward_speed;

    cars_rotation[i] += fabs(cars_controls[i].rotation);
  }
}

shared_ptr<NeuralNetwork> Simulation::GetChampion() {
  int best = 0;

  for (int i = 0; i < car_results.size(); i++) {
    CarResult &best_r = car_results[best];
    CarResult &next_r = car_results[i];

    if (best_r.result < next_r.result) {
      best = i;
    }
  }

  last_best_result = car_results[best].result;
  
  return car_results[best].nn;
}

void Simulation::RestartSimulation(int cars_count, float mutation) {
  while (!cars_transforms.empty()) {
    DeleteCar(0);
  }

  shared_ptr<NeuralNetwork> champion = GetChampion();
  car_results.clear();

  raycast_results.resize(cars_count * rays_layout.size());
  cars_distance.resize(cars_count);
  cars_transforms.resize(cars_count);
  cars_velocity.resize(cars_count);
  cars_controls.resize(cars_count);
  cars_rotation.resize(cars_count, 0);
  neural_networks.resize(cars_count);

  Transforn2D start_pos;
  start_pos.pos = {0, 0};
  start_pos.rot = -1.9;

  for (int i = 0; i < cars_count; i++) {
    cars_distance[i] = 0;
    cars_transforms[i] = start_pos;
    cars_velocity[i] = {0, 0};
    cars_controls[i] = {};
    neural_networks[i] = make_shared<NeuralNetwork>(*champion, mutation);
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
  create_info.layers_sizes = {(int)rays_layout.size() + 2, 10, 10, 2};

  for (int i = 0; i < cars_transforms.size(); i++) {
    shared_ptr<NeuralNetwork> nn = make_shared<NeuralNetwork>(create_info);
    neural_networks.push_back(nn);
  }
}

float Simulation::EstimateCarResults(int index) {
  float score = cars_distance[index] / time_from_start;
  // score += cars_distance[index];
  //  score += (cars_distance[index] / cars_rotation[index]);
  // score += (cars_distance[index] / cars_rotation[index] / 5);

  return score;
}

void Simulation::DeleteCar(int index) {
  CarResult car_result;
  car_result.nn = neural_networks[index];
  car_result.result = EstimateCarResults(index);
  car_results.push_back(car_result);

  cars_velocity.erase(cars_velocity.begin() + index);
  cars_transforms.erase(cars_transforms.begin() + index);
  cars_controls.erase(cars_controls.begin() + index);
  neural_networks.erase(neural_networks.begin() + index);
  cars_distance.erase(cars_distance.begin() + index);
  cars_rotation.erase(cars_rotation.begin() + index);

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

void Simulation::Render(bool draw_rays, bool draw_borders_kd_tree) {
  RenderCars();

  if (draw_rays) {
    RenderRays();
  }

  if (draw_borders_kd_tree) {
    RenderKDBorders();
  }
}

void Simulation::RenderKDBorders() {
  vector<Rect> kd_rects = map_borders->GetKDRects();

  debug_renderer->LoadRects(kd_rects);
}

void Simulation::RenderCars() { cars_renderer->LoadSprites(cars_transforms); }

void Simulation::RenderRays() { debug_renderer->LoadLines(raycast_lines); }

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
  const float step = 0.07;
  for (float i = step; i < 2; i += step) {
    angles.push_back(i);
    angles.push_back(-i);
  }

  for (auto i : angles) {
    glm::fvec2 ray = {sin(i), cos(i)};
    rays_layout.push_back(ray);
  }

  map_borders->LoadRaysLayout(rays_layout);
  map_borders->SetMaxDistance(1.8);
}
