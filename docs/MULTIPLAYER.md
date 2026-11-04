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
