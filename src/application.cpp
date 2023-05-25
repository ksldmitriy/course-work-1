#include "application.hpp"

Application ::~Application() { INFO("application destroyed"); }

void Application::Run() {
  Prepare();

  RenderLoop();
}

void Application::Prepare() {
  VulkanApplication::Prepare();

  program_start = now();
  frame_time_history.resize(100, 0);
  fps_history.resize(100, 0);
}

void Application::RenderLoop() {
  DEBUG("render loop launched");

  while (!window->ShouldClose()) {
    Update();

    RenderUI();

	debug_renderer->DrawLine({{0, 0}, {1, 1}});
    
    Draw();

	Camera camera;
	camera.pos = {time_from_start / 5.0, 0};
	camera.scale = 1;
	
	SetCamera(camera);
  }

  DEBUG("render loop exit");
}

void Application::Update() {
  UpdateTime();

  ProcessEvents();
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

void Application::ProcessMouseMoveEvent(MouseMoveEvent event) {}

void Application::ProcessMouseButtonEvent(MouseButtonEvent event) {}

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
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImVec2 window_pos = {0, 0};
  ImVec2 window_size = {500, 800};
  ImGui::SetNextWindowPos(window_pos);
  ImGui::SetNextWindowSize(window_size);
  ImGui::Begin("##main");

  ImGui::BeginTabBar("tabs");

  if (ImGui::BeginTabItem("sprites")) {
    DrawPerformanceMenu();
    ImGui::EndTabItem();
  }
  if (ImGui::BeginTabItem("performance 2")) {
    DrawPerformanceMenu();
    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();

  ImGui::End();

  ImGui::Render();
}

void Application::DrawPerformanceMenu() {
  ImGui::PlotLines("frame time", frame_time_history.data(),
                   frame_time_history.size(), 0, nullptr, 0, 0.032, {0, 120});

  ImGui::PlotLines("fps", fps_history.data(), fps_history.size(), 0, nullptr, 0,
                   120, {0, 120});

  ImGui::Text((string("frame time: ") + to_string(delta_time)).c_str());
  ImGui::Text((string("fps: ") + to_string(fps)).c_str());
}
