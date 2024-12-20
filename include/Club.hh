#pragma once
#include <chrono>
#include <cstddef>

struct ClubInfo {
  std::size_t clubCapacity;
  std::chrono::hours startTimeHours;
  std::chrono::minutes startTimeMinutes;
  std::chrono::hours closeTimeHours;
  std::chrono::minutes closeTimeMinutes;
  std::size_t cost;
};
