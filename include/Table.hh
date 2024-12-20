#pragma once
#include "Time.hh"

// Стол
struct Table {
  bool isOccupied = false;
  std::string client;
  int revenue = 0, totalTime = 0;
  Time startTime;

  void occupy(const std::string &clientName, const Time &time);
  void release(const Time &time, int price);
};
