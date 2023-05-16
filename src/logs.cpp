#include "logs.hpp"
#include <memory>

shared_ptr<spdlog::logger> logger;

shared_ptr<spdlog::sinks::basic_file_sink_mt> info_file_logger_sink;
shared_ptr<spdlog::sinks::basic_file_sink_mt> debug_file_logger_sink;
shared_ptr<spdlog::sinks::stdout_color_sink_mt> terminal_logger_sink;

void setup_logs() {
  debug_file_logger_sink = make_shared<spdlog::sinks::basic_file_sink_mt>("full-logs");
  debug_file_logger_sink->set_pattern("[%I:%M:%S %o] [%^%l%$]: %v");
  debug_file_logger_sink->set_level(spdlog::level::trace);

  info_file_logger_sink = make_shared<spdlog::sinks::basic_file_sink_mt>("debug-logs");
  info_file_logger_sink->set_pattern("[%I:%M:%S %o] [%^%l%$]: %v");
  info_file_logger_sink->set_level(spdlog::level::debug);
  
  terminal_logger_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
  terminal_logger_sink->set_pattern("[%o] [%^%l%$]: %v");
  terminal_logger_sink->set_level(spdlog::level::debug);

  vector<spdlog::sink_ptr> sinks = {info_file_logger_sink, debug_file_logger_sink, terminal_logger_sink};

  logger = make_unique<spdlog::logger>("main logger", begin(sinks), end(sinks));
  logger->set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::level_enum::trace);

  DEBUG("logger setuped");
}
