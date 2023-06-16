#include "application.hpp"

Application ::~Application() { INFO("application destroyed"); }

void Application::Run() {
  Prepare();

  MainLoop();
}

void Application::Prepare() { VulkanApplication::Prepare(); }

void Application::MainLoop() {
  DEBUG("application main loop start");

  while (!window->ShouldClose()) {
    Update();

    Draw();
  }
}

void Application::Update() { ProcessEvents(); }

void Application::ChangeSufaceCallback() {}

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
  duration delta_time_duration = current_frame - time_info.prev_frame;
  time_info.prev_frame = current_frame;
  duration time_form_start_duration = current_frame - time_info.program_start;

  time_info.delta_time =
      chrono::duration_cast<chrono::nanoseconds>(delta_time_duration).count() /
      1000000000.0l;

  time_info.time_from_start =
      chrono::duration_cast<chrono::nanoseconds>(time_form_start_duration)
          .count() /
      1000000000.0l;

  time_info.frame_time_history.erase(time_info.frame_time_history.begin());
  time_info.frame_time_history.push_back(time_info.delta_time);

  long double total_frame_time = 0;
  for (int i = time_info.frame_time_history.size() - 20;
       i < time_info.frame_time_history.size(); i++) {
    total_frame_time += time_info.frame_time_history[i];
  }

  time_info.fps = 1.0 / (total_frame_time / 20.0);

  time_info.fps_history.erase(time_info.fps_history.begin());
  time_info.fps_history.push_back(time_info.fps);
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

  ImGui::Text("test text");

  ImGui::End();

  ImGui::Render();

  first_call = false;
}
