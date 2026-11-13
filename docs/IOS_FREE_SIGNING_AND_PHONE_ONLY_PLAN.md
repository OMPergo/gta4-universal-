# iOS build, signing, and installation plan (free-account / phone-only constraint)

**Target:** GTA IV Universal on iPhone 12 and iPhone 13.
**Current constraint:** no local computer, no paid Apple Developer Program membership.
**Status:** engineering plan and CI guardrails; this document does not claim a complete iOS app or installable IPA exists.

## 1. Non-negotiable distinction

An iOS ARM64 static library is not an iOS application. A successful CMake build of
`LibertyUniversalCore` does not create an `.app`, an `.ipa`, or a package that
can be installed on an iPhone.

The current Universal repository is a small CMake core/scaffold. It does not yet
contain the complete Theft4/LibertyRecomp game runtime, generated GTA IV code,
iOS UIKit application target, Metal renderer integration, or all required
runtime/build assets. The iOS workflow therefore remains a core compile check.

The CI artifact is deliberately named and documented as **core-only / unsigned /
not an IPA**. Do not rename it to `.ipa`, wrap it in a fake app bundle, or claim
that it is installable.

## 2. Source integration sequence

1. Pin the exact Theft4 and XHYN Android commit SHAs and their submodule SHAs.
2. Compare the LibertyRecomp and ReXGlue ancestry and generated-game-code
   provenance. Select one shared runtime baseline rather than copying two
   divergent full runtimes.
3. Import the required shared runtime and generated code with license notices
   and a source manifest.
4. Adapt Theft4's iOS app shell, lifecycle, file access, Metal backend, audio,
   and input integration as platform-specific code.
5. Keep XHYN's Android shell and input implementation platform-specific while
   defining shared settings semantics and platform-neutral game actions.
6. Add an actual iOS app target that links the selected runtime and all required
   resources. Add packaging and bundle-content validation only after that target
   exists.
7. Build the full app on macOS CI, then inspect the app bundle and IPA. A
   simulator build and a device-arm64 build are separate checks.

Do not copy game data, title updates, encryption keys, private signing material,
device identifiers, user saves, or private diagnostic logs into Git or CI.

## 3. Free Apple Account: what it does and does not provide

Apple's free Personal Team signing is managed through Xcode. Apple's current
account documentation states that Personal Team provisioning supports up to
three registered devices and three installed apps per device; App IDs, devices,
and provisioning profiles expire after seven days, requiring reprovisioning,
rebuilding, and reinstalling.

This is a development/testing path, not TestFlight distribution. TestFlight and
App Store Connect require Apple Developer Program membership.

Because the user has no computer, they cannot presently perform the normal
first-time Personal Team setup or connect/register their iPhones through Xcode.
A GitHub-hosted macOS runner can compile source, but it does not automatically
create the user's Personal Team, register the physical iPhones, or grant a
permanent signing identity. Do not put an Apple Account password or private key
in repository files or ordinary workflow logs.

A remote signing workflow may be investigated later only if the user can
legitimately establish/export the required signing assets and securely supply
them to GitHub Actions. It must not be represented as available until tested
end-to-end on a physical device.

## 4. Practical release gates

- **Core-compiled:** the current CMake core builds and host tests pass.
- **Runtime-integrated:** the selected full game runtime and generated code are
  linked into the iOS target.
- **App-packaged:** a real iOS app bundle contains the executable, frameworks,
  resources, metadata, and required assets.
- **Install-signed:** Apple accepts the signature and provisioning profile for
  the target device.
- **Install-validated:** the app installs and opens on a physical iPhone.
- **Gameplay-validated:** user-supplied compatible game files pass validation
  and GTA IV reaches a controllable 3D scene.
- **Multiplayer-validated:** two physical devices join and synchronize actual
  in-game player state in both directions.

Never advance a release label based only on an earlier gate.

## 5. Immediate phone-only actions

1. Use GitHub on the Android phone to inspect the integration branch and its
   Actions run.
2. Download the CI artifact only for engineering inspection; it is not an app.
3. Continue source integration and compile validation through GitHub Actions.
4. Before the first physical-device install, arrange a legitimate signing path:
   temporary access to a Mac with Xcode for free Personal Team provisioning,
   or later Apple Developer Program enrollment and TestFlight distribution.
5. Keep the iPhone 12 and iPhone 13 as separate physical-device validation
   targets. Do not claim either is supported until it has installed and launched.

## 6. Current limitation / honest user-facing status

With no computer and a free Apple Account, a complete build can be prepared in
cloud CI, but there is currently no established way to sign and install that
build on the user's iPhones. The immediate engineering work can proceed; physical
installation remains blocked until a valid Apple signing/provisioning route is
available.

Official Apple references:
- https://developer.apple.com/help/account/basics/about-your-developer-account/
- https://developer.apple.com/documentation/xcode/distributing-your-app-to-registered-devices
- https://developer.apple.com/documentation/xcode/signing-and-capabilities
