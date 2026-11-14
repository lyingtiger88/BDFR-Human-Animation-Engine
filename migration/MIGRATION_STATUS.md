# Migration Status

Destination: `lyingtiger88/BDFR-Human-Animation-Engine`

Pinned source trees:
- `lyingtiger88/BDFR_FacialAnimation`, branch `main`, tree `c090e6ab4612373e2c701f7ace39d69b68f242a5`
- `lyingtiger88/BDFR_PerformanceCapture`, branch `main`, tree `4c36eeb06402cd16d635aa46bf9b5fce12aec832`

## Exact source snapshot migration
- [x] All 244 tracked Facial Animation files copied.
- [x] All 82 tracked Performance Capture files copied.
- [x] All 326 destination paths compared with source trees: no missing paths.
- [x] All 326 Git blob SHA values compared: exact match, no mismatches.
- [x] Machine-readable manifest: `migration/SNAPSHOT_MANIFEST.json`.
- [x] Verification script: `migration/verify_snapshot.py`.
- [x] Snapshot verification passed in Linux and Windows CI on the baseline run.

The snapshots preserve all tracked source files and paths. They do not mirror source Git history or untracked local files. The source trees reported no Git submodules or Git LFS pointer files.

## 64-bit superbuild and migrated modules
- [x] C++20 / CMake 64-bit-only engine foundation.
- [x] Windows x64 and Linux x86-64 CI matrix.
- [x] Facial source module added to the unified CMake superbuild.
- [x] Performance Capture source module added to the unified CMake superbuild.
- [x] Both modules compiled successfully on Linux and Windows in the baseline integration CI run.
- [x] Legacy facial and capture test suites passed in that baseline run.

## Canonical runtime integration
- [x] Versioned canonical multi-domain session schema.
- [x] Validation for session metadata, timestamps, confidence, face curves, head/body/hand channels, joints, speech and emotion.
- [x] Named-channel preservation for face, head, body and both hands.
- [x] Configurable capture FusionCore to canonical frame bridge.
- [x] Facial CurveMixer integration helper.
- [x] Cross-module integration regression test added.
- [x] Current integration build and all 12 CTest cases pass on Linux x86-64 (run `43581980673`).
- [ ] The matching Windows x64 build and CTest job is still running.
- [ ] Full coordinate-system conversion and named retarget-profile implementation.
- [ ] Lossless canonical session serialization and schema migration tests.
- [ ] Full live camera/audio/face/body/hand end-to-end tests.
- [ ] Unified Qt Studio and Android capture application.

## Licensing and external assets
- [x] Third-party dependency and model-asset review register created at `Docs/THIRD_PARTY_DEPENDENCIES.md`.
- [ ] Select and add an explicit project license.
- [ ] Review upstream licenses, MetaHuman/SMPL/MANO terms and downloadable model assets.

## Repository retirement
The source snapshot copy is complete and SHA-verified, but this is not yet proof of full application-level parity. Do not delete either original repository until current Windows/Linux CI passes, the licensing/asset review is complete, representative end-to-end tests pass, and the final feature parity checklist is approved.
