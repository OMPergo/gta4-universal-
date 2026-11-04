# Liberty City Universal — System Architecture

Liberty City Universal provides a unified native runtime for Grand Theft Auto IV across mobile and desktop platforms.

```
+-------------------------------------------------------------+
|                     Grand Theft Auto IV                     |
|           Recompiled Xbox 360 Game Logic & Title Update      |
+-------------------------------------------------------------+
                              |
+-------------------------------------------------------------+
|                     LibertyRecomp Core                      |
|       PPC Instruction Translation, Big-Endian Emulation     |
+-------------------------------------------------------------+
                              |
+-------------------------------------------------------------+
|                   Unified Shared Runtime                    |
| Memory Budgets | Virtual FS | Config CVars | Error Handler  |
+-------------------------------------------------------------+
         |                        |                    |
+-----------------+      +-----------------+  +---------------+
|    Diagnostics  |      |    Graphics     |  |   Multiplayer |
| Hardware Prober |      | Backend Selector|  | Authoritative |
| Tiers (Low/High)|      | Vulkan / Metal  |  | Host & Guests |
+-----------------+      +-----------------+  +---------------+
         |                        |                    |
+-------------------------------------------------------------+
|                     Platform Adapters                       |
|   Android ARM64  |  Apple iOS/macOS  |  Linux  |  Windows   |
+-------------------------------------------------------------+
```

## Subsystem Details
1. **Diagnostics & Hardware Probing (`LibertyRecomp/diagnostics/`)**:
   - Classifies CPU architecture and SIMD instruction sets.
   - Detects physical and available system RAM, calculating conservative budgets.
   - Detects GPU vendor IDs and drivers (Adreno, Mali, Immortalis, Xclipse, Apple, Nvidia, AMD, Intel).
   - Maps detected hardware to appropriate performance tiers (`MinimumExperimental`, `Mainstream`, `HighPerformance`).

2. **Modular GPU Compatibility (`LibertyRecomp/gpu/`)**:
   - Configures hardware-specific workarounds (Mali clip-distance workarounds, subpass fallbacks).
   - Manages pipeline caches to eliminate micro-stutters during runtime shader binding.
   - Supports system Vulkan driver with optional rootless driver loading on Adreno.

3. **Multiplayer (`LibertyRecomp/network/`)**:
   - Authoritative host model: Host plays Niko Bellic and triggers story scripts.
   - Independent guest players: Join without taking control of Niko, synchronized through UDP packets.
   - Script isolation prevents guests from disrupting host cutscenes or missions.

4. **Game Installer & Validator (`installer/`)**:
   - Inspects Title ID (`0x545407F2`) and Media ID.
   - Identifies USA retail `6AC07221` for Title Update 8 delta patching.
   - Identifies alternative disc revisions (`0675959C` / `06759S9C`) with clear diagnostic guidance.
