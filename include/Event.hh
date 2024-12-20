#pragma once

// Команды
class ClubManager; // Forward declaration

class Event {
public:
  virtual void execute(ClubManager &manager) = 0;
  virtual ~Event() = default;
};
