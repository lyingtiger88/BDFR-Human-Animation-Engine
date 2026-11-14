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
- Procedural facial behavior (blink, gaze drift and head motion) to canonical face/head/gaze tracks.
- Emotion poses and instant facial events (blink, brow flick, lip twitch, nose flare and jaw clench) to canonical face/emotion tracks.
- Audio buffers to canonical speech tracks with windowed energy, prosody and pitch estimates.
- TextSpeechPlanner output to timestamped canonical speech samples and heuristic grapheme-to-viseme facial curves (not audio-recognized phonemes or speech-driven lip sync).
- Camera calibration profiles in the canonical engine core, including intrinsic/distortion/extrinsic parameters, versioned persistence, 3D-to-2D projection and reprojection RMSE evaluation.

Build and test status is recorded in `migration/MIGRATION_STATUS.md`; the exact capability-by-capability parity and remaining work are listed in `Docs/FEATURE_PARITY_MATRIX.md`. The Qt Studio, Android capture client, calibration estimation/device workflow, official model-basis integration, sensor extrinsic calibration, full retargeting and production asset/license review remain separate migration gates. The Windows/Linux Build + CTest matrix is now triggered after substantial engine changes, per owner approval.


## Unified Studio work in progress

The feature branch `feature/studio-live-camera-preview` adds a Live Capture tab with OpenCV device enumeration and a live preview, plus a Session Library tab for scanning and inspecting recorded `.bdfrtake` / CSV sessions. It also retains the camera/rig calibration wizard. This branch is development-only at present: no build was run and live hardware acceptance remains pending. See `Docs/STUDIO_WORKSPACE.md` for usage and prerequisites.
