#include "../include/ClubManager.hh"
#include <algorithm>
#include <iostream>

void ClubManager::addEvent(std::unique_ptr<Event> command) {
  command->execute(*this);
}

void ClubManager::endDay() {
  dayEnded = true;

  // Освобождаем занятые столы и фиксируем, что клиенты покидают клуб.
  for (auto &table : tables) {
    if (table.isOccupied) {
      clientLeave(table.client, closeTime);
    }
  }

  // Генерируем событие "11" для всех клиентов, оставшихся в клубе (по
  // алфавиту).
  std::vector<std::string> remainingClients;
  for (const auto &[client, state] : clientsState) {
    remainingClients.push_back(client);
  }

  sort(remainingClients.begin(), remainingClients.end());
  for (const auto &client : remainingClients) {
    std::cout << closeTime << " 11 " << client << std::endl;
  }
}

// Методы для команд
void ClubManager::clientArrive(const std::string &client, const Time &time) {
  if (time > closeTime) {
    endDay();
  }
  std::cout << time << " 1 " << client << std::endl;
  if (time < openTime || time > closeTime) {
    std::cout << time << " 13 NotOpenYet" << std::endl;
  } else if (clientsState.find(client) != clientsState.end()) {
    std::cout << time << " 13 YouShallNotPass" << std::endl;
  } else {
    clientsState[client] = false;
  }
}

void ClubManager::clientSit(const std::string &client, int table,
                            const Time &time) {
  if (time > closeTime) {
    endDay();
  }
  std::cout << time << " 2 " << client << " " << table << std::endl;
  if (clientsState.find(client) == clientsState.end()) {
    std::cout << time << " 13 ClientUnknown" << std::endl;
  } else if (tables[table - 1].isOccupied) {
    std::cout << time << " 13 PlaceIsBusy" << std::endl;
  } else {
    tables[table - 1].occupy(client, time);
  }
}

void ClubManager::clientWait(const std::string &client, const Time &time) {
  if (clientsState.find(client) != clientsState.end())
    clientsState[client] = true;

  if (time > closeTime) {
    endDay();
  }
  std::cout << time << " 3 " << client << " " << std::endl;
  if (clientsState.find(client) == clientsState.end()) {
    std::cout << time << " 13 ClientUnknown" << std::endl;
  } else if (any_of(tables.begin(), tables.end(),
                    [](const Table &t) { return !t.isOccupied; })) {
    std::cout << time << " 13 ICanWaitNoLonger!" << std::endl;
  } else if (waitQueue.size() - leftFromQueue.size() >= tableCount) {
    std::cout << time << " 11 " << client << std::endl;
    clientsState.erase(client);
  } else {
    waitQueue.push(client);
  }
}

void ClubManager::clientLeave(const std::string &client, const Time &time) {
  if (time > closeTime) {
    endDay();
  }
  if (!dayEnded)
    std::cout << time << " 4 " << client << std::endl;
  if (clientsState.find(client) == clientsState.end()) {
    std::cout << time << " 13 ClientUnknown" << std::endl;
  }
  for (auto &table : tables) {
    if (table.isOccupied && table.client == client) {
      table.release(time, price);
      if (!dayEnded && !waitQueue.empty()) {
        std::string nextClient = waitQueue.front();
        waitQueue.pop();
        table.occupy(nextClient, time);
        std::cout << time << " 12 " << nextClient << " "
                  << (&table - &tables[0] + 1) << std::endl;
      }
      break;
    }
  }
  // Если ожидает в очереди
  if (clientsState[client]) {
    leftFromQueue.insert(client);
  }
  clientsState.erase(client);
}

void ClubManager::printStats() {
  std::cout << closeTime << std::endl;
  for (size_t i = 0; i < tables.size(); ++i) {
    std::cout << i + 1 << " " << tables[i].revenue << " " << std::setfill('0')
              << std::setw(2) << tables[i].totalTime / 60 << ":"
              << std::setfill('0') << std::setw(2) << tables[i].totalTime % 60
              << std::endl;
  }
}
