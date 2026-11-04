# Liberty City Universal

A unified, native, cross-platform Grand Theft Auto IV recompilation project using the Xbox 360 version as its foundation.

---

## GTA IV Universal — Android Download

### Latest Release
- **Status:** In Active Development / Build Milestone Phase 2A
- **Target Architecture:** Android ARM64 (`arm64-v8a`)
- **Minimum Target:** Android 10+ (API 29+), Vulkan 1.1+ support. Specifically engineered to support entry-level devices such as the **Samsung Galaxy A14 5G** (Dimensity 700, Mali-G57 MC2, 4 GB RAM) via 540p dynamic resolution scaling and FSR1.
- **Download Link:** [Download Latest Android APK](https://github.com/OMPergo/gta4-universal-/releases/latest) *(Note: Official APK releases will be published as GitHub Release assets once native recompilation staging passes CI).*

### Installation & Game Data Requirements
1. **Legally Obtained Game Data**: GTA IV Universal does **not** include any copyrighted game assets, executables, or title updates. Users must supply their own legitimate Xbox 360 GTA IV USA disc image (Media ID: `6AC07221`) and Title Update 8 (0.0.8.5).
2. **Setup Instructions**:
   - Install the APK on your ARM64 Android device.
   - On first launch, use the integrated installer screen to select your Xbox 360 GTA IV ISO image and Title Update 8.
   - Wait for validation and asset extraction to complete, then return to the main menu and launch the game.

---

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
- [Android APK Pipeline Audit](docs/ANDROID_APK_ANALYSIS.md)
- [Third-Party Research](docs/THIRD_PARTY_RESEARCH.md)
- [Third-Party Licenses](docs/THIRD_PARTY_LICENSES.md)
- [Implementation Status](docs/IMPLEMENTATION_STATUS.md)
