#include "../include/Events.hh"
#include "../include/Parser.hh"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return 1;
  }

  std::ifstream inputFile(argv[1]);
  if (!inputFile.is_open()) {
    return 1;
  }
  std::pair<ClubInfo, std::vector<Command>> data;
  try {
    data = Parser::parse(inputFile);
  } catch (const Parser::FormatException &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  ClubInfo club = data.first;
  std::vector<Command> commands = data.second;
  Time openTime(club.startTimeHours, club.startTimeMinutes);
  Time closeTime(club.closeTimeHours, club.closeTimeMinutes);

  std::cout << openTime << std::endl;

  ClubManager manager(club.clubCapacity, openTime, closeTime, club.cost);

  for (auto const &cmd : commands) {
    Time time(cmd.hours, cmd.minutes);
    if (cmd.type == CommandID::IN_CLIENT_ARRIVE) {
      manager.addEvent(std::make_unique<ArriveEvent>(time, cmd.clientName));
    } else if (cmd.type == CommandID::IN_CLIENT_LOCK_TABLE) {
      manager.addEvent(
          std::make_unique<SitEvent>(time, cmd.clientName, cmd.seatID));
    } else if (cmd.type == CommandID::IN_CLIENT_WAITING) {
      manager.addEvent(std::make_unique<WaitEvent>(time, cmd.clientName));
    } else if (cmd.type == CommandID::IN_CLIENT_LEFT) {
      manager.addEvent(std::make_unique<LeaveEvent>(time, cmd.clientName));
    }
  }
  if (!manager.dayEnded) {
    manager.endDay();
  }
  manager.printStats();
  return 0;
}
