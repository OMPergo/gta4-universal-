#pragma once

#include "multiplayer_protocol.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>

namespace LibertyRecomp::Network {

enum class PlayerRole {
  HostStoryPlayer,    // Host controls Niko Bellic and story progression
  GuestWorldPlayer    // Guest plays as an independent multiplayer character/pedestrian
};

struct ConnectedPeer {
  uint8_t peer_id = 0;
  uint64_t xuid = 0;
  std::string gamertag;
  PlayerRole role = PlayerRole::GuestWorldPlayer;
  PlayerSyncData last_state;
  uint64_t last_heartbeat_time_ms = 0;
  bool is_script_isolated = true; // Prevents guest from triggering host mission triggers
};

class AuthoritativeSession {
public:
  AuthoritativeSession(bool is_host, uint64_t local_xuid, std::string local_gamertag);

  bool IsHost() const { return is_host_; }
  uint8_t GetLocalPeerId() const { return local_peer_id_; }

  bool AddGuestPeer(uint8_t peer_id, uint64_t xuid, const std::string& gamertag);
  bool RemovePeer(uint8_t peer_id);
  bool UpdatePeerState(uint8_t peer_id, const PlayerSyncData& state);

  std::optional<ConnectedPeer> GetPeer(uint8_t peer_id) const;
  size_t GetConnectedPeerCount() const;

  // Mission & Script Isolation
  bool CanPeerTriggerMissionScript(uint8_t peer_id) const;

private:
  bool is_host_ = false;
  uint8_t local_peer_id_ = 0;
  uint64_t local_xuid_ = 0;
  std::string local_gamertag_;

  mutable std::mutex mutex_;
  std::unordered_map<uint8_t, ConnectedPeer> peers_;
};

} // namespace LibertyRecomp::Network
