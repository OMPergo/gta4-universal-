# Liberty City Universal

A unified, native, cross-platform Grand Theft Auto IV recompilation project using the Xbox 360 version as its foundation.

---

## GTA IV Universal — Android Download

### Latest Release
- **Release Version:** [v0.5.9.3-universal-alpha](https://github.com/OMPergo/gta4-universal-/releases/latest)
- **Direct APK Download:** [Download GTA IV Universal Android APK](https://github.com/OMPergo/gta4-universal-/releases/latest/download/app-debug.apk)
- **Target Architecture:** Android ARM64 (`arm64-v8a`)
- **Minimum Target:** Android 10+ (API 29+), Vulkan 1.1+ support. Specifically engineered to support entry-level devices such as the **Samsung Galaxy A14 5G** (Dimensity 700, Mali-G57 MC2, 4 GB RAM) via 540p dynamic resolution scaling and FSR1.
- **Build Status:** Android ARM64 application module and JNI runtime bridge verified in GitHub Actions CI.

### Forensic APK Audit & Runtime Status
For detailed forensic analysis explaining the composition of the Android build and comparing the 2.4 MB build against upstream's 67 MB bundled release, see:
- [Complete Project Forensic Audit & APK Analysis Report](docs/PROJECT_FORENSIC_AUDIT_REPORT.md)
- [PowerPC Recompilation & Execution Roadmap](docs/RECOMPILATION_INTEGRATION_ANALYSIS.md)

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
- [Forensic Project & APK Audit](docs/PROJECT_FORENSIC_AUDIT_REPORT.md)
- [PowerPC Recompilation Architecture](docs/RECOMPILATION_INTEGRATION_ANALYSIS.md)
- [Architecture Overview](docs/ARCHITECTURE.md)
- [Device Compatibility](docs/DEVICE_COMPATIBILITY.md)
- [Performance Engineering](docs/PERFORMANCE.md)
- [Multiplayer Design](docs/MULTIPLAYER.md)
- [Building for Android](docs/BUILDING_ANDROID.md)
- [Building for iOS](docs/BUILDING_IOS.md)
- [Third-Party Research](docs/THIRD_PARTY_RESEARCH.md)
- [Third-Party Licenses](docs/THIRD_PARTY_LICENSES.md)
- [Implementation Status](docs/IMPLEMENTATION_STATUS.md)
