# Galaxy A14 low-load test preset

The in-game Settings panel offers **Performance → Galaxy A14 low-load test profile**.

The preset is opt-in. It stages only settings accepted by the pinned Android JNI settings allowlist and changes nothing until the user presses Save:

- 30 FPS frame cap
- Adaptive draw distance enabled, scale 0.60
- Sun-shadow updates disabled
- Reflection capture distance set to near
- Water and environment reflection updates every second frame
- Motion blur disabled
- Adaptive audio buffering enabled
- Android thermal guard enabled

This preset does **not** currently change output resolution, upscaler, shadow-map size, reflection resolution, queued-frame count, or display refresh. Those controls are not all exposed through the pinned runtime's JNI settings allowlist and must not be advertised as part of this preset.

The preset is reversible through the individual settings or Reset. It is a controlled low-load test configuration, not a claim that the A14 will reach 30 FPS. It does not implement Mali shader clip-distance emulation, fix native shutdown crashes, or enable internet matchmaking.

After applying it, compare frame-time percentiles, audio underruns, memory, visual correctness, and shutdown behavior against the previous report.
