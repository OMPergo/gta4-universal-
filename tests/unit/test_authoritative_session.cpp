#include "../../LibertyRecomp/network/authoritative_session.h"
#include <cassert>
#include <iostream>

using namespace LibertyRecomp::Network;

void TestAuthoritativeSessionRules() {
  // Create host session (Niko)
  AuthoritativeSession session(true, 0x1122334455667788ULL, "NikoBellic_Host");
  assert(session.IsHost() == true);
  assert(session.GetLocalPeerId() == 0);
  assert(session.GetConnectedPeerCount() == 1);
  assert(session.CanPeerTriggerMissionScript(0) == true);

  // Add guest peer
  bool added = session.AddGuestPeer(1, 0xAABBCCDDEEFF0011ULL, "GuestPlayer2");
  assert(added == true);
  assert(session.GetConnectedPeerCount() == 2);

  // Guest must NOT trigger host mission scripts
  assert(session.CanPeerTriggerMissionScript(1) == false);

  // Attempt to add duplicate peer or peer ID 0 as guest
  assert(session.AddGuestPeer(0, 0x1234, "FakeHost") == false);
  assert(session.AddGuestPeer(1, 0x5678, "DupGuest") == false);

  // Update guest state
  PlayerSyncData sync{};
  sync.header.magic = kUniversalProtocolMagic;
  sync.header.version = kUniversalProtocolVersion;
  sync.header.type = PacketType::PlayerStateSync;
  sync.header.peer_id = 1;
  sync.position = {100.0f, 200.0f, 15.0f};
  sync.health = 95.0f;

  assert(session.UpdatePeerState(1, sync) == true);
  auto peer = session.GetPeer(1);
  assert(peer.has_value());
  assert(peer->last_state.position.x == 100.0f);
  assert(peer->last_state.health == 95.0f);

  // Validate packet header helper
  assert(ValidatePacketHeader(sync.header, PacketType::PlayerStateSync) == true);
  assert(ValidatePacketHeader(sync.header, PacketType::VehicleStateSync) == false);

  // Remove guest
  assert(session.RemovePeer(1) == true);
  assert(session.GetConnectedPeerCount() == 1);
  assert(session.RemovePeer(0) == false); // Host cannot remove self

  std::cout << "[PASS] Authoritative Session & Guest Script Isolation" << std::endl;
}

int main() {
  std::cout << "Running Authoritative Session Unit Tests..." << std::endl;
  TestAuthoritativeSessionRules();
  std::cout << "All Authoritative Session Unit Tests Passed Successfully!" << std::endl;
  return 0;
}
