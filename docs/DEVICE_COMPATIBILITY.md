# Device Compatibility Specification

Liberty City Universal detects hardware dynamically rather than whitelisting device model strings.

## Hardware Tiers
### 1. Minimum / Experimental Tier
- **Target Specification**: Entry-level ARM64 devices with <= 4 GB RAM.
- **Reference Target**: **Samsung Galaxy A14 5G**
  - SoC: MediaTek Dimensity 700 (2x Cortex-A76 @ 2.2GHz + 6x Cortex-A55 @ 2.0GHz)
  - GPU: ARM Mali-G57 MC2 (Valhall architecture)
  - RAM: 4 GB LPDDR4X
  - OS: Android ARM64
- **Runtime Configuration**:
  - Internal rendering: 540p (960x540) with dynamic resolution down to 480p.
  - Upscaling: AMD FSR 1.0 enabled.
  - Conservative memory budget: <= 1.5 GB total application memory, <= 384 MB texture budget.
  - Draw distance: 0.6x. Traffic & pedestrian density: 0.5x.
  - Streaming workers: 1 worker thread to avoid CPU cache thrashing.
  - Target framerate: 30 FPS.

### 2. Mainstream Tier
- **Target Specification**: Midrange ARM64 devices (6 GB - 8 GB RAM) and mid-tier GPUs (Snapdragon 7/8 Gen 1, Mali-G78/G710).
- **Runtime Configuration**:
  - Internal rendering: 720p (1280x720) with dynamic scaling.
  - Texture budget: 768 MB.
  - Draw distance: 1.0x. Traffic & pedestrian density: 0.8x.
  - Streaming workers: 2 worker threads.
  - Target framerate: 30 FPS.

### 3. High-Performance Tier
- **Target Specification**: Apple Silicon (M1/M2/M3/M4, A17 Pro/A18), modern desktop x86-64 / ARM64, flagship GPUs.
- **Runtime Configuration**:
  - Internal rendering: 1080p (1920x1080) native.
  - Texture budget: 1.5 GB+.
  - Draw distance: 1.2x. Traffic & pedestrian density: 1.0x.
  - Streaming workers: 4 worker threads.
  - Target framerate: 60 FPS.
