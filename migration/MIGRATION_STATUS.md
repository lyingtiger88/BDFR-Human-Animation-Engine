# Migration Status

Destination: `lyingtiger88/BDFR-Human-Animation-Engine`

Sources pinned by source Git tree:
- `lyingtiger88/BDFR_FacialAnimation`, branch `main`, tree `c090e6ab4612373e2c701f7ace39d69b68f242a5`
- `lyingtiger88/BDFR_PerformanceCapture`, branch `main`, tree `4c36eeb06402cd16d635aa46bf9b5fce12aec832`

## Exact source snapshot migration
- [x] All 244 tracked Facial Animation files copied to `migration/source-snapshots/BDFR_FacialAnimation/`.
- [x] All 82 tracked Performance Capture files copied to `migration/source-snapshots/BDFR_PerformanceCapture/`.
- [x] All 326 destination paths compared with source trees: no missing paths.
- [x] All 326 Git blob SHA values compared: exact match, no mismatches.
- [x] Machine-readable manifest: `migration/SNAPSHOT_MANIFEST.json`.
- [x] CI verification script: `migration/verify_snapshot.py`.
- [x] CI snapshot verification step passed on Linux x86-64.

The snapshots preserve the original tracked source files and paths. This does not include untracked local files or the source repositories' Git commit history. The source trees reported no Git submodules or LFS pointer files.

## 64-bit superbuild
- [x] C++20 / CMake 64-bit-only engine foundation.
- [x] Windows x64 and Linux x86-64 CI matrix.
- [x] Facial source module added to the unified CMake superbuild.
- [x] Performance Capture source module added to the unified CMake superbuild.
- [x] Both legacy modules compile successfully on Linux in the initial superbuild run.
- [ ] Current unified-runtime bridge build and tests must pass on both CI platforms.

## Canonical runtime integration
- [x] Versioned canonical multi-domain session schema.
- [x] Timestamp and confidence validation foundation.
- [x] Named-channel preservation for face, head, body and hands.
- [x] Capture FusionCore to canonical frame bridge.
- [x] Facial CurveMixer integration helper.
- [ ] Complete coordinate conversion, retarget profiles and lossless session serialization.
- [ ] Full live camera, audio, speech, face and body end-to-end tests.
- [ ] Unified Qt Studio and Android capture application.

## Retirement gate
Do not delete either source repository yet. The source snapshot copy is complete and SHA-verified, but source history is not mirrored and full application-level parity has not been demonstrated. Retire the originals only after Windows/Linux CI passes on the current runtime integration, model and third-party license review is complete, representative end-to-end tests pass, and a final parity checklist is approved.
