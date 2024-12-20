#include "../include/Events.hh"

void ArriveEvent::execute(ClubManager &manager) {
  manager.clientArrive(client, time);
}

void SitEvent::execute(ClubManager &manager) {
  manager.clientSit(client, table, time);
}

void WaitEvent::execute(ClubManager &manager) {
  manager.clientWait(client, time);
}

void LeaveEvent::execute(ClubManager &manager) {
  manager.clientLeave(client, time);
}
