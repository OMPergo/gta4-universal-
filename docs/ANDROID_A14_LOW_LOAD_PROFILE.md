# Galaxy A14 low-load test preset

The in-game Settings panel now offers **Performance → Galaxy A14 low-load test profile**.

The preset is opt-in. It stages the following values in the settings editor and changes nothing until the user presses Save:

- 540p output resolution, native upscaler
- 30 FPS cap
- Adaptive draw distance with a 0.60 scale
- 256 shadow map, sun shadows disabled
- Quarter-resolution reflections, water, and mirrors
- Reflection capture distance set to near; water/environment updates every second frame
- Motion blur disabled
- Two queued frames
- Adaptive audio dropout protection enabled; low-latency audio disabled
- Android thermal guard and lower display refresh enabled

Restart-marked settings apply on the next launch. The preset is reversible through the individual settings or Reset.

This is a controlled low-load test configuration, not a claim that the A14 will reach 30 FPS. It does not implement Mali shader clip-distance emulation, fix native shutdown crashes, or enable internet matchmaking. After applying it, compare frame-time percentiles, audio underruns, memory, visual correctness, and shutdown behavior against the previous report.