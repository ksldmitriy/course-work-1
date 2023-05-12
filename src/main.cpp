#include "main.hpp"
#include "logs.hpp"

int main() {
  setup_logs();

  try {
    Application application;
    application.Run();
  } catch (IException &e) {
    ERROR("application exit afer unhandled excpetion: {0}", (string)e);
    return -1;
  };

  INFO("program finished");
}
