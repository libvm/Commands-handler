#include "../include/Time.hh"

std::chrono::minutes Time::toMinutes() const {
  return std::chrono::duration_cast<std::chrono::minutes>(hours) + minutes;
}

bool Time::operator>(const Time &other) const {
  return toMinutes() > other.toMinutes();
}

bool Time::operator<(const Time &other) const {
  return toMinutes() < other.toMinutes();
}

bool Time::operator<=(const Time &other) const {
  return toMinutes() <= other.toMinutes();
}

Time Time::operator+(std::chrono::minutes mins) const {
  std::chrono::minutes total = toMinutes() + mins;
  return Time(std::chrono::hours(total.count() / 60),
              std::chrono::minutes(total.count() % 60));
}

std::ostream &operator<<(std::ostream &os, const Time &t) {
  os << std::setw(2) << std::setfill('0') << t.hours.count() << ":"
     << std::setw(2) << std::setfill('0') << t.minutes.count();
  return os;
}
