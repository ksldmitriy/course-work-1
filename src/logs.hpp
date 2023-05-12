#pragma once
#include <iostream>
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>

#pragma GCC diagnostic pop

#define TRACE(...) logger->trace(__VA_ARGS__)
#define DEBUG(...) logger->debug(__VA_ARGS__)
#define INFO(...) logger->info(__VA_ARGS__)
#define WARN(...) logger->warn(__VA_ARGS__)
#define ERROR(...) logger->error(__VA_ARGS__)
#define FATAL(...) logger->critical(__VA_ARGS__)

using namespace std;

extern shared_ptr<spdlog::logger> logger;

void setup_logs();
