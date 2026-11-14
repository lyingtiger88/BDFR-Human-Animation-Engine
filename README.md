# BDFR Human Animation Engine

A unified, 64-bit-first platform for reconstructing, synthesizing, editing and retargeting human performance across face, head, body, hands and speech.

## Migration policy

The complete tracked source trees of BDFR_FacialAnimation and BDFR_PerformanceCapture are preserved under `migration/source-snapshots/` as read-only provenance snapshots. They are not compiled directly as one monolithic target. Production modules are migrated into the new architecture through explicit contracts, regression tests and license review.

## Platform baseline

- Desktop: Windows x86-64 and Linux x86-64.
- Mobile capture: Android arm64-v8a where supported.
- C++20 engine core, CMake, Qt 6 Studio.
- Python worker processes for ML inference stacks that are unsuitable for in-process linking.
- Unreal Engine integration behind a plugin/adapter boundary.
- 64-bit pointer-size compile checks; no 32-bit desktop builds.

## Product modes

Face Only · Body Only · Hybrid · Audio Driven · Offline Refine.

## Build (initial scaffold)

```powershell
cmake -S . -B build -A x64 -DBDFR_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

On Linux x86-64:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBDFR_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

See `Docs/` and `migration/` for the architecture, feature inventory and source provenance.

## Migration integration status

The exact tracked source trees from both predecessor repositories are preserved under `migration/source-snapshots/` and checked against `migration/SNAPSHOT_MANIFEST.json` by `migration/verify_snapshot.py`.

The top-level CMake superbuild compiles the migrated facial and performance-capture libraries as separate modules while exposing a shared canonical runtime. The current integration adapters cover:
- Facial solver frames to face, head and gaze tracks.
- EasyMocap/SMPL skeleton poses to canonical joints, root transforms and shape/expression/hand-PCA channels.
- Capture FusionCore output to canonical face, head, body and hand tracks.
- Facial CurveMixer layers to canonical facial curves.

Build and test status is recorded in `migration/MIGRATION_STATUS.md`. The Qt Studio, Android capture client, full retargeting/coordinate conversion and production asset/license review remain separate migration gates.
