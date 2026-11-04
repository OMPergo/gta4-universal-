# Android release process

1. Merge or select the commit that passed the full Android ARM64 workflow.
2. Create a new, never-before-used tag on that commit, for example `android-v0.1.0`.
3. Push the tag. The Android workflow builds the tagged source and publishes a GitHub Release with the APK only after every validation step passes.
4. The repository front page links directly to Releases, Actions, the integration commit history, and the failure ledger.
5. Never move or overwrite a published tag. Use a new patch/minor version for every update.

Automated APKs are development builds until a stable signing keystore is configured in GitHub Actions secrets. A different signing certificate prevents Android from installing the new APK as an update over an older one. Do not promise seamless updates until certificate continuity is verified.
