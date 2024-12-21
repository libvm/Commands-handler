#include "../include/ClubManager.hh"
#include <algorithm>
#include <iostream>

void ClubManager::addEvent(std::unique_ptr<Event> command) {
  command->execute(*this);
}

// Метод для завершения дня
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
    std::stringstream result;
    result << closeTime << " 11 " << client;
    output.push_back(result.str());
  }
}

// Методы для команд

// Метод для ID 1
void ClubManager::clientArrive(const std::string &client, const Time &time) {
  // Входящая команда
  std::stringstream inCommand;
  if (time > closeTime) {
    endDay();
  }
  inCommand << time << " 1 " << client;
  output.push_back(inCommand.str());

  // Исходящая команда
  std::stringstream outCommand;
  if (time < openTime || time > closeTime) {
    outCommand << time << " 13 NotOpenYet";
  } else if (clientsState.find(client) != clientsState.end()) {
    outCommand << time << " 13 YouShallNotPass";
  } else {
    clientsState[client] = false;
  }
  if (!outCommand.str().empty())
    output.push_back(outCommand.str());
}

// Метод для ID 2
void ClubManager::clientSit(const std::string &client, int table,
                            const Time &time) {
  // Входящая команда
  std::stringstream inCommand;
  if (time > closeTime) {
    endDay();
  }
  inCommand << time << " 2 " << client << " " << table;
  output.push_back(inCommand.str());

  // Исходящая команда
  std::stringstream outCommand;
  if (clientsState.find(client) == clientsState.end()) {
    outCommand << time << " 13 ClientUnknown";
  } else if (tables[table - 1].isOccupied) {
    outCommand << time << " 13 PlaceIsBusy";
  } else {
    tables[table - 1].occupy(client, time);
  }
  if (!outCommand.str().empty())
    output.push_back(outCommand.str());
}

// Метод для ID 3 и ID 11
void ClubManager::clientWait(const std::string &client, const Time &time) {
  // Входящая команда
  std::stringstream inCommand;
  if (clientsState.find(client) != clientsState.end())
    clientsState[client] = true;

  if (time > closeTime) {
    endDay();
  }
  inCommand << time << " 3 " << client;
  output.push_back(inCommand.str());

  // Исходящая команда
  std::stringstream outCommand;
  if (clientsState.find(client) == clientsState.end()) {
    outCommand << time << " 13 ClientUnknown";
  } else if (any_of(tables.begin(), tables.end(),
                    [](const Table &t) { return !t.isOccupied; })) {
    outCommand << time << " 13 ICanWaitNoLonger!";
  } else if (waitQueue.size() - leftFromQueue.size() >= tableCount) {
    outCommand << time << " 11 " << client;
    clientsState.erase(client);
  } else {
    waitQueue.push(client);
  }
  if (!outCommand.str().empty())
    output.push_back(outCommand.str());
}

// Метод для ID 4 и ID 12
void ClubManager::clientLeave(const std::string &client, const Time &time) {
  // Входящая команда
  std::stringstream inCommand;
  if (time > closeTime) {
    endDay();
  }
  if (!dayEnded) {
    inCommand << time << " 4 " << client;
    output.push_back(inCommand.str());
  }

  // Исходящая команда
  std::stringstream outCommand;
  if (clientsState.find(client) == clientsState.end()) {
    outCommand << time << " 13 ClientUnknown";
  }
  for (auto &table : tables) {
    if (table.isOccupied && table.client == client) {
      table.release(time, price);
      if (!dayEnded && !waitQueue.empty()) {
        std::string nextClient = waitQueue.front();
        waitQueue.pop();
        table.occupy(nextClient, time);
        outCommand << time << " 12 " << nextClient << " "
                   << (&table - &tables[0] + 1);
      }
      break;
    }
  }
  // Если ожидает в очереди
  if (clientsState[client]) {
    leftFromQueue.insert(client);
  }
  clientsState.erase(client);
  if (!outCommand.str().empty())
    output.push_back(outCommand.str());
}

// Вывод статистики за день
void ClubManager::getStats() {
  std::stringstream stats;
  stats << closeTime;
  for (size_t i = 0; i < tables.size(); ++i) {
    stats << i + 1 << " " << tables[i].revenue << " " << std::setfill('0')
          << std::setw(2) << tables[i].totalTime / 60 << ":"
          << std::setfill('0') << std::setw(2) << tables[i].totalTime % 60;
  }
  output.push_back(stats.str());
}

void ClubManager::printOutput() {
  for (const auto &str : output) {
    std::cout << str << std::endl;
  }
}
