#include "../include/Table.hh"

void Table::occupy(const std::string &clientName, const Time &time) {
  isOccupied = true;
  client = clientName;
  startTime = time;
}

void Table::release(const Time &time, int price) {
  int duration =
      (time.toMinutes().count() - startTime.toMinutes().count() + 59) / 60;
  revenue += duration * price;
  totalTime += time.toMinutes().count() - startTime.toMinutes().count();
  client = "";
  isOccupied = false;
}
