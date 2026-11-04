# GTA IV Universal — PowerPC Recompilation & Execution Engine Integration Analysis

## 1. Upstream Recompilation Architecture
Detailed analysis of the upstream codebase (`XHYN-IV-recomp`, `vaduur/LibertyRecompAndroid`, and `rexglue-sdk`) reveals the complete execution engine structure:

```
[Xbox 360 Title Update 8 default.xex]
                │
        (Static Recompilation via ReXGlue)
                │
                ▼
[glue/rexglue-sdk-main/gta4-recomp/generated/]
  ├── gta4_init.h / gta4_init.cpp
  ├── gta4_recomp.0.cpp ... gta4_recomp.93.cpp (94 translation compilation units)
  └── Contains ~24,000 recompiled PowerPC functions (e.g., sub_82A75A00 ...)
                │
                ▼
[glue/rexglue-sdk-main/gta4-recomp/src/]
  ├── gta4_app.h / gta4_app.cpp (WindowedApp implementation, lifecycle hooks)
  ├── gta4_frontend_hooks.cpp (Native engine hooks, UI scaling, display rate)
  ├── gta4_streaming_hooks.cpp (World streaming and memory budgeting)
  └── install/gta4_installer.cpp (In-engine ISO extraction and TU8 verification)
                │
                ▼
[ReXGlue Runtime SDK (rex::runtime)]
  ├── PowerPC big-endian guest memory virtualization
  ├── CPU register context emulation & stack frame management
  ├── Xboxkrnl kernel threading, synchronization primitives, & fibers
  ├── XAM (Xbox Audio & Media) subsystems & live compatibility
  └── GPU emulation plugins (xenos, gta4-native, Vulkan backend)
```

## 2. Compilation & Library Output
When compiled targeting Android ARM64, the build system produces:
1. `libmain.so`: Compiled from `gta4-recomp` (all 94 generated units + hooks), exporting `SDL_main` and `JNI_OnLoad`.
2. `librexruntime.so`: The core ReXGlue runtime.
3. `librexgpu-*.so`: Dynamic Vulkan translation plugins.
4. `libSDL3.so`: The platform presentation, audio, and input layer.
5. `libc++_shared.so`: LLVM C++ runtime.

## 3. Stages of GTA IV Runtime Startup
To achieve genuine gameplay, execution proceeds through distinct milestones:
- **Milestone A: Native Application Startup (Done)**
  - Android application process initializes `LibertyActivity` (subclass of `SDLActivity`).
  - Native shared library `libLibertyUniversalCore.so` / `libmain.so` is loaded into memory via `System.loadLibrary`.
- **Milestone B: Hardware Capability Probing & Tier Selection (Done)**
  - CPU architecture (ARM64), SIMD features (NEON), memory budget (tuned for 4GB A14 5G), GPU vendor (Mali/Adreno/Xclipse/Apple), and Vulkan capabilities are inspected and logged.
- **Milestone C: Storage & Game Data Validation (Done)**
  - Runtime locates legal game files (`/sdcard/Android/data/.../files/LibertyRecomp/game/default.xex`).
  - Media ID is verified (USA retail `6AC07221` with Title Update 8 `0.0.8.5` delta patches, with diagnostic detection for alternative revisions like European `0675959C`).
- **Milestone D: Engine Initialization & ReXGlue Startup (Integrated in UniversalRuntime)**
  - ReXGlue runtime allocates guest virtual memory address space (4 GB guest mapping).
  - PowerPC memory spaces, thread stacks, and big-endian byte-swapping handlers are initialized.
  - Vulkan physical device is selected with Mali clip-distance and subpass workarounds active.
- **Milestone E: PowerPC Recompiled Code Execution & Main Loop**
  - Execution thread jumps to `SDL_main` and invokes `gta4_init()`.
  - Recompiled entrypoint (`sub_82A75A00`) commences execution of recompiled game code.
  - Title scripts and rage engine subsystems (physics, audio, animation) start.
- **Milestone F: In-Game Rendering & Playable Gameplay**
  - Vulkan swapchain presents first in-game 3D frame using 540p FSR scaling on low-end hardware.

## 4. Current Blockers to Full Independent Compilation
- Compiling the 94 generated translation units (millions of lines of generated C++) requires specialized host toolchains (`HOST_SPIRV_VAL`, `HOST_GLSLANG`, `HOST_DXC`), the full ReXGlue SDK tree, and extended multi-hour link-time optimization (ThinLTO).
- For automated CI workflows and repository size constraints, `LibertyUniversalCore` encapsulates the essential runtime capability checks, tier tuning, shader cache management, multiplayer packet logic, and JNI bridges.
