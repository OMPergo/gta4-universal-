# Building for iOS & iPadOS

## Prerequisites
- macOS 14+ with Xcode 15+
- CMake 3.28+ and Ninja
- Apple Developer Account (for device signing)

## Generating Simulator Target
```bash
cmake -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphonesimulator \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-ios --config Release
```

## Note on Code Signing
Unsigned builds can run in the iOS Simulator. Deploying to physical iOS/iPadOS devices requires provisioning profile configuration and code signing with a valid Apple Developer identity.
