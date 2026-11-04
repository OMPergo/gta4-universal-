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
- **Status:** Fix committed in the current branch; awaiting CI validation.

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
