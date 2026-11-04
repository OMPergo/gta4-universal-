# Liberty City Universal

A unified, native, cross-platform Grand Theft Auto IV recompilation project using the Xbox 360 version as its foundation.

Liberty City Universal brings native execution of GTA IV to a wide range of platforms:
- **Android ARM64** (phones and tablets, with specialized minimum-hardware profiles for targets such as Samsung Galaxy A14 5G).
- **iOS & iPadOS ARM64** (Apple Silicon native lifecycle and Metal backend).
- **Windows** (x86-64 and ARM64).
- **Linux** (x86-64 and ARM64).
- **macOS** (Apple Silicon).

## Core Architecture
- **PPC Recompilation Core**: High-fidelity static recompilation preserving Xbox 360 PPC instructions and calling conventions.
- **Hardware Capability Detection**: Runtime inspection of CPU SIMD, RAM budgets, GPU vendors (Adreno, Mali, Immortalis, Xclipse, Apple), and Vulkan/Metal extensions.
- **Low-End Optimization Tier**: Dedicated 540p / dynamic resolution profile with aggressive texture budgeting (tailored for devices like Samsung Galaxy A14 5G with Dimensity 700 / Mali-G57 MC2).
- **Authoritative LAN Multiplayer**: Host controls Niko Bellic and story missions; guests join as independent world pedestrians without mission desync or host hijack.
- **Game Data Integrity**: Rigorous validation of legal Xbox 360 disc dumps (USA Retail `6AC07221`) and identification of alternative disc revisions (`0675959C` / `06759S9C`).

For detailed documentation, see the `docs/` directory:
- [Architecture](docs/ARCHITECTURE.md)
- [Device Compatibility](docs/DEVICE_COMPATIBILITY.md)
- [Performance Engineering](docs/PERFORMANCE.md)
- [Multiplayer Design](docs/MULTIPLAYER.md)
- [Building for Android](docs/BUILDING_ANDROID.md)
- [Building for iOS](docs/BUILDING_IOS.md)
- [Third-Party Research](docs/THIRD_PARTY_RESEARCH.md)
- [Third-Party Licenses](docs/THIRD_PARTY_LICENSES.md)
- [Implementation Status](docs/IMPLEMENTATION_STATUS.md)
