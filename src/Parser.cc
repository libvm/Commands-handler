#include "../include/Parser.hh"
#include <algorithm>
#include <sstream>

namespace {
class parser {
public:
  explicit parser(std::istream &input) : input(input) {}

  std::pair<ClubInfo, std::vector<Command>> parse() {
    ClubInfo info = parseClubInfo();
    std::vector<Command> commands = parseCommands(info.clubCapacity);
    return {info, commands};
  }

private:
  std::string getNextLine() {
    std::string line;
    std::getline(input, line);
    return line;
  }

  static std::vector<std::string> split(const std::string &s, char delim) {
    std::istringstream iss(s);
    std::vector<std::string> result;
    std::string next;
    while (std::getline(iss, next, delim)) {
      result.emplace_back(next);
    }
    return result;
  }

  // Функция для проверки временного интервала
  std::vector<std::pair<std::chrono::hours, std::chrono::minutes>>
  parseTimeInterval(const std::string &timeStr) {
    // Разделяем строку на время открытия и закрытия
    std::vector<std::string> splittedClubTime = split(timeStr, ' ');
    if (splittedClubTime.size() != 2) {
      throw Parser::FormatException();
    }

    std::pair<std::chrono::hours, std::chrono::minutes> startClubTime;
    std::pair<std::chrono::hours, std::chrono::minutes> endClubTime;
    // Проверяем формат каждого времени
    try {
      startClubTime = parseTime(splittedClubTime[0]);
      endClubTime = parseTime(splittedClubTime[1]);
    } catch (const std::exception &e) {
      throw Parser::FormatException();
    }

    std::vector<std::pair<std::chrono::hours, std::chrono::minutes>>
        timeInterval{startClubTime, endClubTime};

    return std::move(timeInterval);
  }

  // Функция для проверки формата времени
  std::pair<std::chrono::hours, std::chrono::minutes>
  parseTime(const std::string &timeStr) {
    if (timeStr.size() != 5)
      throw Parser::FormatException();
    if (timeStr[2] != ':')
      throw Parser::FormatException();

    int hours, minutes;
    if (sscanf(timeStr.c_str(), "%d:%d", &hours, &minutes) != 2)
      throw Parser::FormatException();
    if (hours < 0 || hours > 23)
      throw Parser::FormatException();
    if (minutes < 0 || minutes > 59)
      throw Parser::FormatException();
    return std::make_pair(std::chrono::hours(hours),
                          std::chrono::minutes(minutes));
  }

  ClubInfo parseClubInfo() {
    std::size_t count;
    std::string clubTime;
    std::uint64_t cost;

    std::string countStr = getNextLine();
    try {
      count = std::stoul(countStr);
    } catch (const std::exception &e) {
      throw Parser::FormatException(countStr);
    }

    clubTime = getNextLine();
    std::vector<std::pair<std::chrono::hours, std::chrono::minutes>>
        parsedTimeInterval;
    try {
      parsedTimeInterval = parseTimeInterval(clubTime);
    } catch (const std::exception &e) {
      throw Parser::FormatException(clubTime);
    }

    std::string costStr = getNextLine();
    try {
      cost = std::stoul(costStr);
    } catch (const std::exception &e) {
      throw Parser::FormatException(costStr);
    }

    return ClubInfo{count,
                    parsedTimeInterval[0].first,
                    parsedTimeInterval[0].second,
                    parsedTimeInterval[1].first,
                    parsedTimeInterval[1].second,
                    cost};
  }

  static bool checkName(const std::string &name) {
    auto pred = [](char c) {
      return c == '_' || c == '-' || (c >= '0' && c <= '9') ||
             (c >= 'a' && c <= 'z');
    };
    return all_of(name.begin(), name.end(), pred);
  }

  CommandID getcommandID(int type) const {
    switch (type) {
    case static_cast<int>(CommandID::IN_CLIENT_ARRIVE):
      return CommandID::IN_CLIENT_ARRIVE;
    case static_cast<int>(CommandID::IN_CLIENT_LOCK_TABLE):
      return CommandID::IN_CLIENT_LOCK_TABLE;
    case static_cast<int>(CommandID::IN_CLIENT_LEFT):
      return CommandID::IN_CLIENT_LEFT;
    case static_cast<int>(CommandID::IN_CLIENT_WAITING):
      return CommandID::IN_CLIENT_WAITING;
    default:
      throw Parser::FormatException();
    }
  }

  Command parseCommand(
      std::string &commandStr,
      std::pair<std::chrono::hours, std::chrono::minutes> &previousTime,
      std::size_t &clubCapacity) {
    std::vector<std::string> splitted = split(commandStr, ' ');
    Command cmd{};

    // Проверка на количество элементов
    if (splitted.size() < 3 || splitted.size() > 4) {
      throw Parser::FormatException();
    }

    // Проверка на формат времени
    std::pair<std::chrono::hours, std::chrono::minutes> commandTime;
    try {
      commandTime = parseTime(splitted[0]);
    } catch (const std::exception &e) {
      throw Parser::FormatException();
    }

    // Проверка на то, что комманды выполняются последовательно по времени
    std::chrono::minutes previousTimeMinutes =
        std::chrono::duration_cast<std::chrono::minutes>(previousTime.first);
    std::chrono::minutes totalPreviousTime =
        previousTimeMinutes + previousTime.second;

    std::chrono::minutes commandTimeMinutes =
        std::chrono::duration_cast<std::chrono::minutes>(commandTime.first);
    std::chrono::minutes totalCommandTime =
        commandTimeMinutes + commandTime.second;

    if (totalCommandTime < totalPreviousTime) {
      throw Parser::FormatException();
    }

    // Проверка формата команды
    std::int32_t typeID;
    try {
      typeID = std::stoi(splitted[1]);
    } catch (std::exception &e) {
      throw Parser::FormatException();
    }

    CommandID type = getcommandID(typeID);
    if (type == CommandID::IN_CLIENT_LOCK_TABLE && splitted.size() != 4) {
      throw Parser::FormatException();
    } else if (type != CommandID::IN_CLIENT_LOCK_TABLE &&
               splitted.size() == 4) {
      throw Parser::FormatException();
    }

    // Проверка формата имени
    if (!checkName(splitted[2])) {
      throw Parser::FormatException();
    }

    std::size_t seatID;
    // Проверка формата номера кресла
    if (type == CommandID::IN_CLIENT_LOCK_TABLE) {
      try {
        seatID = std::stoul(splitted[3]);
      } catch (const std::exception &e) {
        throw Parser::FormatException();
      }
      if (seatID > clubCapacity) {
        throw Parser::FormatException();
      }
    }

    return Command{type, commandTime.first, commandTime.second, splitted[2],
                   seatID};
  }

  Command getNextCommand(
      std::pair<std::chrono::hours, std::chrono::minutes> previousTime,
      std::size_t clubCapacity) {
    Command nextCommand;
    std::string commandStr = getNextLine();
    try {
      nextCommand = parseCommand(commandStr, previousTime, clubCapacity);
    } catch (const std::exception &e) {
      throw Parser::FormatException(commandStr);
    }
    return nextCommand;
  }

  std::vector<Command> parseCommands(std::size_t clubCapacity) {
    std::vector<Command> commands;
    std::chrono::hours previousHours{0};
    std::chrono::minutes previousMinutes{0};
    std::pair<std::chrono::hours, std::chrono::minutes> previousTime{
        previousHours, previousMinutes};

    while (input.peek() != std::char_traits<char>::eof()) {
      std::pair<std::chrono::hours, std::chrono::minutes> previousTime{
          previousHours, previousMinutes};
      Command cmd = getNextCommand(previousTime, clubCapacity);
      commands.emplace_back(cmd);
      previousHours = commands.back().hours;
      previousMinutes = commands.back().minutes;
    }

    return commands;
  }

  std::istream &input;
};
} // namespace

std::pair<ClubInfo, std::vector<Command>> Parser::parse(std::istream &input) {
  return parser(input).parse();
}
