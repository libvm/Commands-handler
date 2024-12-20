#pragma once
#include <chrono>
#include <cstddef>
#include <string>

enum class CommandID {
  IN_CLIENT_ARRIVE = 1,
  IN_CLIENT_LOCK_TABLE = 2,
  IN_CLIENT_WAITING = 3,
  IN_CLIENT_LEFT = 4,

  OUT_CLIENT_LEFT = 11,
  OUT_CLIENT_LOCK_TABLE = 12,
  OUT_ERROR = 13
};

struct Command {
  CommandID type;
  std::chrono::hours hours;
  std::chrono::minutes minutes;
  std::string clientName;
  std::size_t seatID;
};
