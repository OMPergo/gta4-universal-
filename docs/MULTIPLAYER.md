# Multiplayer Architecture & Protocol

## Overview
Liberty City Universal implements an authoritative host multiplayer model tailored for cooperative world roaming and LAN play without requiring third-party cloud accounts.

## Host and Guest Roles
- **Host (Peer 0)**:
  - Controls Niko Bellic and story progression.
  - Executes mission scripts and world event triggers.
  - Holds authoritative state for world entities and vehicles.
- **Guest Players (Peer 1..31)**:
  - Join session as independent multiplayer pedestrian entities.
  - Cannot trigger, hijack, or cancel host missions (`is_script_isolated = true`).
  - Synchronize position, rotation, velocity, health, and animations via UDP state packets.

## Protocol Specification
- Magic: `0x4C43554D` ("LCUM")
- Header: Packet Type, Peer ID, Sequence Number
- Packet Types:
  - `DiscoveryBeacon` (1) / `DiscoveryResponse` (2)
  - `JoinRequest` (3) / `JoinAccept` (4) / `JoinReject` (5)
  - `PlayerStateSync` (10)
  - `VehicleStateSync` (11)
  - `GuestScriptBoundaryNotice` (20)


## Android startup multiplayer hub (Alpha 1.0.1)

The Android launcher exposes a **Multiplayer** page beside the game launch action. While the page is open it refreshes network status and performs a bounded LAN discovery scan every five seconds.

- UDP discovery port: `27888` (`kUniversalDiscoveryPort`).
- The launcher broadcasts a 12-byte LCUM v1 discovery probe and accepts only valid v1 `DiscoveryResponse` packets with the expected 52-byte beacon layout.
- The page reports the active network, local Wi-Fi address and subnet broadcast when Android exposes them.
- The page distinguishes LAN results, online directory status, direct-connect status, and the user's current session.
- Online directory, direct-IP joining, hosting, and in-game peer synchronization are explicitly marked unavailable until their server directory and transport/session services are implemented and connected to the runtime.

**Important:** the launcher discovery client is a UI/network-discovery foundation, not a working multiplayer service by itself. A compatible host must listen on UDP 27888 and return the documented discovery response. A discovered beacon does not mean a join or synchronized gameplay has succeeded.
