#include "authoritative_session.h"

namespace LibertyRecomp::Network {

AuthoritativeSession::AuthoritativeSession(bool is_host, uint64_t local_xuid, std::string local_gamertag)
    : is_host_(is_host), local_peer_id_(is_host ? 0 : 1), local_xuid_(local_xuid), local_gamertag_(std::move(local_gamertag)) {
  ConnectedPeer local_peer;
  local_peer.peer_id = local_peer_id_;
  local_peer.xuid = local_xuid_;
  local_peer.gamertag = local_gamertag_;
  local_peer.role = is_host ? PlayerRole::HostStoryPlayer : PlayerRole::GuestWorldPlayer;
  local_peer.is_script_isolated = !is_host; // Host is NOT script-isolated (controls story)

  peers_[local_peer_id_] = local_peer;
}

bool AuthoritativeSession::AddGuestPeer(uint8_t peer_id, uint64_t xuid, const std::string& gamertag) {
  std::scoped_lock lock(mutex_);
  if (peer_id == 0) {
    // Peer ID 0 is strictly reserved for the Host (Niko)
    return false;
  }
  if (peers_.find(peer_id) != peers_.end()) {
    return false; // Already present
  }

  ConnectedPeer peer;
  peer.peer_id = peer_id;
  peer.xuid = xuid;
  peer.gamertag = gamertag;
  peer.role = PlayerRole::GuestWorldPlayer;
  peer.is_script_isolated = true; // Guests are strictly isolated from host mission progression

  peers_[peer_id] = peer;
  return true;
}

bool AuthoritativeSession::RemovePeer(uint8_t peer_id) {
  std::scoped_lock lock(mutex_);
  if (peer_id == local_peer_id_) {
    return false; // Cannot remove self
  }
  return peers_.erase(peer_id) > 0;
}

bool AuthoritativeSession::UpdatePeerState(uint8_t peer_id, const PlayerSyncData& state) {
  std::scoped_lock lock(mutex_);
  auto it = peers_.find(peer_id);
  if (it == peers_.end()) {
    return false;
  }
  it->second.last_state = state;
  return true;
}

std::optional<ConnectedPeer> AuthoritativeSession::GetPeer(uint8_t peer_id) const {
  std::scoped_lock lock(mutex_);
  auto it = peers_.find(peer_id);
  if (it == peers_.end()) {
    return std::nullopt;
  }
  return it->second;
}

size_t AuthoritativeSession::GetConnectedPeerCount() const {
  std::scoped_lock lock(mutex_);
  return peers_.size();
}

bool AuthoritativeSession::CanPeerTriggerMissionScript(uint8_t peer_id) const {
  std::scoped_lock lock(mutex_);
  auto it = peers_.find(peer_id);
  if (it == peers_.end()) {
    return false;
  }
  // Only the Host (Niko) has permission to trigger story missions
  return it->second.role == PlayerRole::HostStoryPlayer && !it->second.is_script_isolated;
}

} // namespace LibertyRecomp::Network
