# A14 Adaptive Resource Management — Alpha implementation boundary

- Adds deterministic memory-budget planning with a 20% safety reserve and independent texture, streaming, and decode ceilings.
- Applies the computed ceilings to startup graphics settings.
- Adds a thread-safe reservation counter and host unit tests.
- This is not yet wired into the runtime's real texture, streaming, or audio allocators. It therefore does not guarantee an RSS reduction or prevent out-of-memory termination by itself.
- Mali clip-distance emulation remains disabled until a verified shader-side implementation exists.
