#include "main.hpp"
#include "logs.hpp"

// implement stb image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
  setup_logs();

  try {
    Application application;
    application.Run();
	INFO("application run finished");
  } catch (IException &e) {
    ERROR("application exit afer unhandled exception: {0}", (string)e);
    return -1;
  } catch (...) {
    ERROR("application exit afer unknown exception");
    return -1;
  }

  INFO("program finished");
}
