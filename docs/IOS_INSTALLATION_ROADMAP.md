# iOS installation roadmap

## Current status (November 2026)

The iOS workflow builds a universal iPhone/iPad **launcher shell** and packages an **unsigned IPA**. It is not currently a playable GTA IV port: the Android ReXGlue/SDL3 runtime has not been ported to an iOS-compatible backend, and the iOS target does not yet contain the gameplay engine.

The unsigned IPA is useful for build inspection only. It cannot be installed on an iPhone by tapping a public download link. iOS requires an appropriately signed app and a provisioning method accepted by the device.

## Recommended contributor distribution: TestFlight

For the owner and invited contributors, TestFlight is the cleanest eventual route:

1. Enroll the account owner in the Apple Developer Program (paid annual membership).
2. Create the app record in App Store Connect using the project's bundle identifier.
3. Configure an Apple Distribution certificate and provisioning through Xcode signing / App Store Connect.
4. Store signing credentials and App Store Connect API credentials as protected GitHub Actions secrets. Never commit certificates, private keys, passwords, or API keys to the repository.
5. Update the macOS workflow to archive the iOS device target with signing, export a signed App Store Connect build, and upload it to TestFlight.
6. Invite contributors by their Apple IDs/email addresses as internal or external testers, following Apple's review requirements for external testing.
7. Contributors install Apple's TestFlight app, accept the invitation, install the build, and then add the installed app to their iPhone home screen in the normal way.

TestFlight installs the app with its icon on the home screen. TestFlight is a distribution/testing channel, not a way to bypass Apple's signing or review rules.

## Alternative for a small device list

Ad Hoc distribution can install on registered devices, but each device UDID must be included in the provisioning profile and the IPA must be signed for that profile. This is less convenient when contributors change phones.

## Work that must happen before a playable iPhone build

Signing the current shell only distributes the shell. It does not make GTA IV playable. A separate engineering track is required to port or replace the native Android runtime dependencies for iOS, provide a compatible graphics backend (likely Metal or a supported Vulkan-to-Metal layer), integrate input/audio/filesystem/platform services, and validate memory/performance on real iPhones. This should be treated as a substantial port, not a packaging toggle.

## Immediate next prerequisites

- Confirm that the project owner has an active Apple Developer Program membership.
- Confirm that the owner controls the App Store Connect account for the app.
- Keep the current unsigned iOS artifact clearly labelled as non-installable until signed distribution is configured.
- Do not ask contributors to install configuration profiles or trust unknown certificates as a workaround.
