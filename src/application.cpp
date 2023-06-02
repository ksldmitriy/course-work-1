#include "application.hpp"

Application ::~Application() { INFO("application destroyed"); }

void Application::Run() {
  Prepare();

  MainLoop();
}

void Application::Prepare() {
  VulkanApplication::Prepare();

  move_state = false;
  camera = {{0, 0}, 0.3};

  program_start = now();
  prev_frame = now();
  frame_time_history.resize(100, 0);
  fps_history.resize(100, 0);

  cars_renderer->SetSpriteSize({0.2, 0.4});

  draw_rays = false;
  draw_borders_kd_tree = false;

  mutation = 0.03;
  cars_count = 100;

  LoadMap();

  CreateSimulation();
}

void Application::CreateSimulation() {
  SimulationRenderers renderers;
  renderers.cars_renderer = cars_renderer.get();
  renderers.debug_renderer = debug_renderer.get();

  SimulationCreateInfo create_info;
  create_info.road_collider = road_collider.get();
  create_info.renderers = renderers;
  create_info.map_borders = map_borders.get();
  create_info.const_delta_time = 1.0 / 60;

  simulation = make_unique<Simulation>(create_info);
}

void Application::LoadMap() {
  MapLoader map_loader;
  map_loader.LoadMap("map-1.obj", 0.2);

  map_mesh = map_loader.GetMesh();
  outer_lines = map_loader.GetOutline();

  road_renderer->LoadMehs(map_mesh);

  map_borders = make_unique<MapBorders>(outer_lines);
  road_collider = make_unique<RoadCollider>(map_mesh);
}

void Application::MainLoop() {
  DEBUG("render loop launched");

  while (!window->ShouldClose()) {
    if (IsSurfaceChanged()) {
      ChangeSufaceCallback();
    }

    Update();

    debug_renderer->LoadLines(outer_lines);

    RenderUI();

    simulation->Render(draw_rays, draw_borders_kd_tree);

    Draw();
  }

  DEBUG("render loop exit");
}

void Application::Update() {
  UpdateTime();

  ProcessEvents();

  simulation->Update();

  SetCamera(camera);
}

void Application::ChangeSufaceCallback() {
  SimulationRenderers renderers;
  renderers.cars_renderer = cars_renderer.get();
  renderers.debug_renderer = debug_renderer.get();

  simulation->SetRenderers(renderers);

  road_renderer->LoadMehs(map_mesh);
}

void Application::ProcessEvents() {
  Window::PollEvents();

  vector<MouseMoveEvent> mouse_move_events;
  vector<MouseButtonEvent> mouse_button_events;

  window->GetEvents(mouse_button_events, mouse_move_events);

  for (auto e : mouse_move_events) {
    ProcessMouseMoveEvent(e);
  }

  for (auto e : mouse_button_events) {
    ProcessMouseButtonEvent(e);
  }
}

void Application::ProcessMouseMoveEvent(MouseMoveEvent event) {
  if (move_state) {
    glm::fvec2 camera_move_vec = event.delta;
    camera_move_vec.x *= -1;
    camera_move_vec *= 2;

    camera.pos += camera_move_vec;
  };
}

void Application::ProcessMouseButtonEvent(MouseButtonEvent event) {
  if (event.button == MouseButton::middle) {
    move_state = event.pressed;
  }
}

void Application::RestartSimulation() {
  simulation->RestartSimulation(cars_count, mutation);
}

void Application::UpdateTime() {
  time_point current_frame = now();
  duration delta_time_duration = current_frame - prev_frame;
  prev_frame = current_frame;
  duration time_form_start_duration = current_frame - program_start;

  delta_time =
      chrono::duration_cast<chrono::nanoseconds>(delta_time_duration).count() /
      1000000000.0l;

  time_from_start =
      chrono::duration_cast<chrono::nanoseconds>(time_form_start_duration)
          .count() /
      1000000000.0l;

  frame_time_history.erase(frame_time_history.begin());
  frame_time_history.push_back(delta_time);

  long double total_frame_time = 0;
  for (int i = frame_time_history.size() - 20; i < frame_time_history.size();
       i++) {
    total_frame_time += frame_time_history[i];
  }

  fps = 1.0 / (total_frame_time / 20.0);

  fps_history.erase(fps_history.begin());
  fps_history.push_back(fps);
}

void Application::RenderUI() {
  static bool first_call = true;

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (first_call) {
    ImVec2 window_pos = {0, 0};
    ImVec2 window_size = {500, 800};
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);
  }
  ImGui::Begin("##main");

  ImGui::BeginTabBar("tabs");

  if (ImGui::BeginTabItem("main")) {
    DrawMainMenu();
    ImGui::EndTabItem();
  }
  if (ImGui::BeginTabItem("performance")) {
    DrawPerformanceMenu();
    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();

  ImGui::End();

  ImGui::Render();

  first_call = false;
}

void Application::DrawMainMenu() {
  ImGui::Checkbox("draw rays", &draw_rays);
  ImGui::Checkbox("draw borders kd tree", &draw_borders_kd_tree);

  ImGui::SliderInt("cars count", &cars_count, 10, 1000);
  ImGui::SliderFloat("mutation", &mutation, 0.003, 0.06);

  if (ImGui::Button("restart simulation")) {
    RestartSimulation();
  }

  float last_best_score = simulation->GetBestResult();
  ImGui::Text(("last best score: " + to_string(last_best_score)).c_str());
}

void Application::DrawPerformanceMenu() {
  ImGui::PlotLines("frame time", frame_time_history.data(),
                   frame_time_history.size(), 0, nullptr, 0, 0.032, {0, 120});

  ImGui::PlotLines("fps", fps_history.data(), fps_history.size(), 0, nullptr, 0,
                   120, {0, 120});

  ImGui::Text((string("frame time: ") + to_string(delta_time)).c_str());
  ImGui::Text((string("fps: ") + to_string(fps)).c_str());
}
