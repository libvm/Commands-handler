#pragma once

#include "Club.hh"
#include "Command.hh"
#include <vector>

namespace Parser {
std::pair<ClubInfo, std::vector<Command>> parse(std::istream &);

class FormatException : public std::exception {
public:
  explicit FormatException(std::string _message)
      : message(std::move(_message)) {}
  explicit FormatException() : message() {}

  const char *what() const noexcept override { return message.c_str(); }

private:
  const std::string message;
};
} // namespace Parser
