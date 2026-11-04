# Native Android runtime integration

The Android build now uses the pinned XHYN R2 Test12 checkout as the native runtime source, without replacing the Universal repository's own application foundation.

CI initializes only the ReXGlue SDK's nested third-party dependency tree required by the Android CMake target, rather than recursively downloading every optional desktop, PS4, Switch, DLSS, and development-tool repository referenced by the upstream superproject.

The native build invokes the upstream Android build script, which builds the actual GTA IV recompiled engine and ReXGlue runtime for arm64-v8a, stages its SDL3/runtime shared libraries and runtime resources, then copies those outputs into the existing Universal Android app. Gradle selects the SDL Java classes from the same pinned SDL3 submodule when available, preserving an existing local SDL source fallback.

The workflow now verifies that libmain.so, librexruntime.so, and libSDL3.so are present and ARM64 before packaging. This is a build integration step, not proof of successful on-device game execution. Device Vulkan compatibility, installation of user-provided Xbox 360 game data and TU8, and real launch testing remain necessary. The 100 MiB APK limit remains enforced; if the complete runtime exceeds it, CI will fail visibly rather than silently dropping runtime components.
