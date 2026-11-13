#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace LibertyRecomp::Network {

constexpr uint32_t kUniversalProtocolMagic = 0x4C43554D; // "LCUM" (Liberty City Universal Multiplayer)
constexpr uint16_t kUniversalProtocolVersion = 1;
// UDP port used by the Android launcher LAN discovery probe and compatible hosts.
constexpr uint16_t kUniversalDiscoveryPort = 27888;

enum class PacketType : uint8_t {
  DiscoveryBeacon = 1,
  DiscoveryResponse = 2,
  JoinRequest = 3,
  JoinAccept = 4,
  JoinReject = 5,
  PlayerStateSync = 10,
  VehicleStateSync = 11,
  GuestScriptBoundaryNotice = 20,
  Heartbeat = 30,
  Disconnect = 31
};

struct Vector3 {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

struct Quaternion {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float w = 1.0f;
};

#pragma pack(push, 1)
struct PacketHeader {
  uint32_t magic = kUniversalProtocolMagic;
  uint16_t version = kUniversalProtocolVersion;
  PacketType type;
  uint8_t peer_id = 0;
  uint32_t sequence = 0;
};

struct PlayerSyncData {
  PacketHeader header;
  uint64_t xuid = 0;
  Vector3 position;
  Vector3 velocity;
  Quaternion rotation;
  uint32_t animation_id = 0;
  uint32_t current_weapon_hash = 0;
  float health = 100.0f;
  float armor = 0.0f;
  uint8_t flags = 0; // Bit 0: is_in_vehicle, Bit 1: is_aiming, Bit 2: is_shooting
};

struct DiscoveryBeaconData {
  PacketHeader header;
  uint16_t server_port = 0;
  char session_name[32] = {0};
  uint8_t current_players = 0;
  uint8_t max_players = 32;
  uint32_t build_revision = 0;
};
#pragma pack(pop)

// Packet validation helper
inline bool ValidatePacketHeader(const PacketHeader& header, PacketType expected_type) {
  return header.magic == kUniversalProtocolMagic &&
         header.version == kUniversalProtocolVersion &&
         header.type == expected_type;
}

} // namespace LibertyRecomp::Network
