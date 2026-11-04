# Build Failure Ledger and Regression Prevention

This file records confirmed build failures, their root causes, fixes, and checks that prevent recurrence. Add an entry whenever CI discovers a new failure. Do not erase old entries; mark them resolved and link the fixing commit.

## BL-001 — ReXGlue dependency setup omitted before Android native configure

- **First observed:** 2026-11-04
- **Failing run:** https://github.com/OMPergo/gta4-universal-/actions/runs/41846807613
- **Failing stage:** Build the real XHYN / ReXGlue GTA IV Android runtime
- **Exact error:** `Dependency source missing: tools/XenosRecomp/CMakeLists.txt`
- **Root cause:** CI initialized only `glue/rexglue-sdk-main/thirdparty`. The upstream XHYN repository has a setup helper at `tools/setup_repo.py` that initializes the complete pinned dependency graph (including XenosRecomp and its own dependencies) and applies reviewed dependency patches. Fetching one subtree is insufficient.
- **Fix:** Run `python3 vendor/XHYN-IV-recomp/tools/setup_repo.py` before native CMake configuration, then run `--check` and explicitly verify XenosRecomp and generated source manifests.
- **Prevention:** Never replace the upstream setup helper with a hand-picked submodule list unless a reviewed dependency manifest proves the list complete. CI fails early if required sources are absent.
- **Status:** Initial fix committed; follow-up hardening adds the Python command alias and uses the same NDK revision for every native build target. Awaiting CI validation.

## BL-002 — Runtime libraries not yet proven to package or launch

- **Observed:** Native runtime integration has not yet passed end-to-end CI or device validation.
- **Risk:** A launcher-only APK can compile successfully without the actual game engine. A successful Gradle build alone is not a playable-game test.
- **Prevention:** Require `libmain.so`, `libliberty_install.so`, `librexruntime.so`, `libSDL3.so`, Vulkan proxy libraries, ARM64 ELF checks, APK archive checks, and an explicit device smoke test before marking a release playable.
- **Status:** Open.

## BL-003 — APK release signing continuity

- **Risk:** Android will reject an update if the signing certificate differs from the installed APK.
- **Prevention:** Configure a stable, protected release keystore in GitHub Actions secrets before claiming install-over updates. Never commit keystores or passwords. Until then, mark automated APKs as development builds and disclose that a previous installation may need to be removed before installing a build signed with a different key.
- **Status:** Open.

## Required CI invariants

1. Checkout the exact pinned XHYN submodule commit.
2. Run the upstream dependency setup helper and its offline check before CMake.
3. Confirm required dependency source files exist before launching expensive native compilation.
4. Build the Android driver proxy before packaging its libraries.
5. Build the native engine with the documented Android script and explicit NDK version.
6. Stage and verify the complete native library set and ARM64 ELF architecture.
7. Build the APK, verify the archive, enforce the size limit, and upload the artifact.
8. Create versioned releases only from explicit `android-vX.Y.Z` tags after the full workflow succeeds.
9. Record each new failure here with its run URL, exact error, root cause, fix, and regression check.

## Release tracking

Use monotonically increasing tags such as `android-v0.1.0`, `android-v0.1.1`, and `android-v0.2.0`. Never reuse a tag for different contents. Every release should state the source commit, build status, APK SHA-256, known limitations, and whether the APK has been tested on physical hardware.

\n## BL-004 — First physical-device gameplay test terminated by Android low-memory manager\n
- **First observed:** 2026-11-04 on Samsung Galaxy A14 5G (4 GB RAM, Mali-G57).\n
- **Build under test:** Android ARM64 workflow run 41849386440, commit `783db3c2efc546e689bf0fcad4f592f2bf7c7378`.\n
- **Observed sequence:** Rockstar startup screens appeared, Nico loading screen appeared, gameplay loaded quickly, gameplay was severely choppy, then Android killed the game process.\n
- **Crash report:** `com.xhyn.ivrecomp:game`; reason `killed by the system: low memory`; RSS approximately 1,293 MB at termination. PSS was reported as 0 MB at exit and is not interpreted as peak usage.\n
- **Root cause:** Not yet established. Candidate areas include game asset streaming, host-side texture/buffer caches, Vulkan allocations, shader/pipeline compilation, and other native allocations. RSS alone cannot identify the subsystem responsible.\n
- **Baseline rule:** Preserve the current default graphics settings for controlled comparison. Do not claim that lowering resolution or texture quality fixes the underlying issue without measurements.\n
- **Next diagnostic pass:** Capture native memory samples across launch, loading, and a repeatable gameplay route; compare process RSS with renderer host categories and Vulkan heap telemetry; inspect frame-time distribution and shader compilation stalls. Use bounded profiler capture settings to avoid materially increasing memory pressure.\n
- **Status:** Open; first gameplay milestone achieved, performance and memory stability not achieved.\n
