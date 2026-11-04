# Pinned XHYN R2 Android source integration

This branch keeps the existing GTA4 Universal implementation as its foundation and pins the XHYN IV R2 Test12 upstream source as an additive Git submodule.

- Upstream: https://github.com/XHYN-PH/XHYN-IV-recomp
- Pinned commit: 025986db1a24aec73ebccc60aac4c235fd820da4
- Submodule path: vendor/XHYN-IV-recomp
- Android CI initializes the direct submodule, then copies missing Java classes, assets, and resources into the app source tree with rsync --ignore-existing. Existing customized files are never overwritten by this synchronization step.

The upstream repository contains many nested third-party submodules for desktop tooling, graphics backends, platform ports, and optional features. They are deliberately not recursively fetched as a blanket operation. Native Android runtime integration must select and pin only the dependencies actually referenced by the Android target, then verify their licenses, ABI support, and build configuration.

This source synchronization is not proof of a playable game. The generated GTA IV code, ReXGlue runtime, Android Vulkan device/swapchain, game-data installation, and on-device execution remain separate required milestones.
