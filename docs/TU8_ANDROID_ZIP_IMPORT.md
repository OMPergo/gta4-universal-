# Android Title Update 8 ZIP import

The Android launcher accepts either an individual Xbox 360 STFS Title Update package or a ZIP archive containing regional TU8 packages.

For the four-region archive used during the November 2026 investigation, the launcher extracts only the package whose archive path identifies Media ID `6AC07221` (USA). It stages that package under the app-private files directory and passes the staged STFS package to the existing native installer. The original ZIP is not modified, and no Rockstar game data is bundled in the APK or repository.

The native installer remains the authority for validation. It checks the GTA IV Title ID, the expected TU8 XEXP patch hash and target version, the base executable revision, and the source executable signature before installing. ZIP filename matching is only a safe selection step; it does not bypass native validation.

## Selecting the original game

The Android runtime can mount a supported Xbox 360 `.iso` directly. Select the ISO file itself in the launcher's Base game row; it does not need to be opened as a folder or extracted first. The installer reads the disc image and copies the required files into the app's game installation directory.

The supported Android installer currently targets the USA retail base and its matching TU8 patch (Media ID `6AC07221`). Other regional packages in the ZIP are not silently substituted.
