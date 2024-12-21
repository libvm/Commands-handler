#pragma once
#include "Event.hh"
#include "Table.hh"
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// Менеджер клуба
class ClubManager {
private:
  int tableCount, price;
  int peopleInQueue;
  Time openTime, closeTime;
  std::vector<Table> tables;
  std::queue<std::string> waitQueue;
  std::unordered_multiset<std::string> leftFromQueue;
  std::unordered_map<std::string, bool> clientsState;
  std::vector<std::string> output;
  void getStats();

public:
  bool dayEnded;
  ClubManager(int tCount, const Time &open, const Time &close, int p)
      : tableCount(tCount), openTime(open), closeTime(close), price(p),
        tables(tCount) {}

  void addEvent(std::unique_ptr<Event> command);

  // Методы для команд
  void clientArrive(const std::string &client, const Time &time);
  void clientSit(const std::string &client, int table, const Time &time);
  void clientWait(const std::string &client, const Time &time);
  void clientLeave(const std::string &client, const Time &time);
  void endDay();
  void printOutput();
};
