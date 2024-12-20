#pragma once
#include "ClubManager.hh"

// Конкретные команды
class ArriveEvent : public Event {
  std::string client;
  Time time;

public:
  ArriveEvent(const Time &t, const std::string &c) : client(c), time(t) {}
  void execute(ClubManager &manager) override;
};

class SitEvent : public Event {
  std::string client;
  int table;
  Time time;

public:
  SitEvent(const Time &t, const std::string &c, int tbl)
      : client(c), table(tbl), time(t) {}
  void execute(ClubManager &manager) override;
};

class WaitEvent : public Event {
  std::string client;
  Time time;

public:
  WaitEvent(const Time &t, const std::string &c) : client(c), time(t) {}
  void execute(ClubManager &manager) override;
};

class LeaveEvent : public Event {
  std::string client;
  Time time;

public:
  LeaveEvent(const Time &t, const std::string &c) : client(c), time(t) {}
  void execute(ClubManager &manager) override;
};
