#pragma once
#include <chrono>
#include <iomanip>

class Time {
private:
  std::chrono::hours hours;
  std::chrono::minutes minutes;

public:
  Time(std::chrono::hours h, std::chrono::minutes m) : hours(h), minutes(m) {}
  Time() : hours(0), minutes(0) {}

  std::chrono::minutes toMinutes() const;
  bool operator>(const Time &other) const;
  bool operator<(const Time &other) const;
  bool operator<=(const Time &other) const;
  Time operator+(std::chrono::minutes mins) const;
  friend std::ostream &operator<<(std::ostream &os, const Time &t);
};
