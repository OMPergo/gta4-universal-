# GTA IV Xbox 360 Base-Game and TU8 Blueprint

## Purpose

This document records the verified input layout and runtime dependencies for the Xbox 360 GTA IV build targeted by GTA4 Universal. It is a technical blueprint only: it does not contain or redistribute Rockstar game assets. Each user must supply files from their own legally obtained copy.

## Evidence repositories

- TU8 package and device diagnostics: [OMPergo/gta-4-title-8-update](https://github.com/OMPergo/gta-4-title-8-update)
- Runtime, installer, and platform builds: [OMPergo/gta4-universal-](https://github.com/OMPergo/gta4-universal-)

The evidence repository currently contains:
- USA TU8 STFS package: `TU_1A581VI_000000K000000.0000000000205` (3,715,072 bytes in GitHub; expected USA media ID `6AC07221`).
- Android diagnostic archive: `XHYN-IV-debug-20261106-053947-653.log.zip` (137,915 bytes).
- README placeholder.

The diagnostic archive is registered as evidence but must be unpacked and parsed before drawing conclusions about the latest device run.

## Verified Xbox 360 input layout

The upstream ReXGlue GTA IV setup documentation describes the following staged layout:

```text
game/
  default.xex
  xbox360.rpf
  common.rpf
  audio.rpf
  common/
  xbox360/
  $SystemUpdate/        # disc/system update data; not the GTA IV TU8 payload

tu8/
  default.xex           # copy of game/default.xex
  default.xexp          # TU8 executable delta

update/
  data/
  shaders/
  text/
  update.img
```

The extracted disc root is reported as approximately 6.5 GB and roughly 170 files by the upstream setup guide. This is a reference estimate, not a verified inventory of the user's own disc.

## Executable and title-update relationship

- `game/default.xex` is the base Xbox 360 executable.
- TU8 supplies `default.xexp`, a delta patch; it is not a standalone replacement executable.
- The ReXGlue manifest stages a copy of the base executable at `tu8/default.xex` beside `tu8/default.xexp`, then applies the delta during code generation/runtime preparation.
- The USA TU8 package is associated with title ID `545407F2` and media ID `6AC07221`. The media ID must match the base disc.
- The remaining TU8 payload (including updated data, shader, text resources, and `update.img`) is staged separately and mounted as the runtime `update:` device.

## What we know vs. what remains unknown

| Item | Status |
|---|---|
| USA TU8 package is present in the evidence repository | Confirmed by repository listing and file size |
| TU8 package title/media identifiers | Public upstream references identify title ID `545407F2`, USA media ID `6AC07221` |
| Base disc root structure | Documented by upstream setup guide; not yet verified against user's ISO |
| Exact base-disc file inventory and per-file hashes | Not yet obtained |
| Exact `default.xex` build/version/hash for user's disc | Not yet obtained |
| Whether the user's disc matches USA TU8 media ID | Must be verified from the user's own disc/XEX metadata |
| Latest Android diagnostic findings | Archive uploaded; detailed parsing pending |

## Next audit steps

1. Parse the uploaded Android diagnostic ZIP and correlate timestamps, runtime logs, hitch CSVs, launch/exit properties, memory, GPU, and crash data.
2. Inspect the TU8 STFS package metadata and internal file table; verify the expected title ID, media ID, package signature, and payload inventory.
3. Compare the Android installer/runtime's actual file probes and mount paths against the staged layout above.
4. Add a machine-readable manifest for known paths, expected metadata, provenance, and verification status. Do not invent hashes or fill unknown sizes.
5. Determine the minimum required user-supplied base-game files and identify which failures are caused by missing content versus runtime/renderer defects.

## Upstream technical references

- ReXGlue GTA IV manifest: https://github.com/luisxl15/GTA-IV-RECOMP-XBOX-360/blob/main/gta4_manifest.toml
- ReXGlue setup guide: https://github.com/luisxl15/GTA-IV-RECOMP-XBOX-360/blob/main/docs/SETUP.md
- ReXGlue project README: https://github.com/luisxl15/GTA-IV-RECOMP-XBOX-360

These references document a compatible research/build workflow; they do not prove that every file requirement is already implemented in GTA4 Universal.
