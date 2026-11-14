# Migration Status

Destination: `lyingtiger88/BDFR-Human-Animation-Engine`

Sources:
- `lyingtiger88/BDFR_FacialAnimation`, branch `main`
- `lyingtiger88/BDFR_PerformanceCapture`, branch `main`

## Verified source inventory
- Facial Animation: 244 tracked files, approximately 670,087 bytes.
- Performance Capture: 82 tracked files, approximately 331,409 bytes.
- Total inventory: 326 tracked files, approximately 1,001,496 bytes.

## Snapshot copy progress
- [~] Facial Animation: 20 / 244 files copied (224 remaining).
- [~] Performance Capture: 12 / 82 files copied (70 remaining).
- [ ] Full content and SHA verification for all 326 source files.
- [ ] Git LFS, submodules, releases and external model assets reviewed.
- [ ] Source commit SHAs and licensing provenance recorded for every imported file.

Copied source snapshots retain original paths beneath `migration/source-snapshots/`. Current copied files include repository docs, build/workflow configuration, facial curve types/mixer/registry, timeline/session/schema/binary codec modules, capture synchronization/fusion, capture manager, skeleton, retargeting and binary take modules.

## Destination foundation
- [x] Destination repository initialized.
- [x] C++20 / CMake 64-bit-only platform guard.
- [x] Windows x64 and Linux x86-64 CI workflow.
- [x] Initial platform pointer-width test.
- [x] Canonical multi-domain performance session model (initial schema).
- [x] Session validation for version, IDs, timebase, timestamps and sample confidence.
- [ ] Full schema serialization, compatibility tests and golden fixtures.
- [ ] All production modules migrated into destination architecture.
- [ ] Qt Studio, Android capture and Unreal adapter integrated and validated.
- [ ] End-to-end tests for Face Only, Body Only, Audio Driven and Hybrid workflows.

## CI
The initial scaffold passed both Windows x64 and Linux x86-64 CI. The newer canonical-session commits are currently undergoing CI; check the latest GitHub Actions run before treating them as validated.

## Deletion gate
Do not delete either source repository until all 326 tracked files and any external assets are accounted for, the required production features are integrated, clean Windows/Linux builds pass, representative end-to-end tests pass, and a final migration ledger is signed off. Snapshot presence alone is not proof of integration.
