# Migration Plan and Deletion Gate

## Stage 0 — Snapshot preservation
- Copy every tracked file from both source repositories into `migration/source-snapshots/`.
- Record source repository, branch, commit SHA, path, byte size and destination blob SHA.
- Keep source paths intact beneath each snapshot root.
- Record any binary/LFS/submodule/release assets that cannot be represented by the source tree API separately.

## Stage 1 — 64-bit foundation
- Establish C++20, CMake and 64-bit-only desktop builds.
- Add Windows x64 and Linux x86-64 CI.
- Define fixed-width serialization and coordinate/time conventions.
- Add schema validation, diagnostics and test conventions.

## Stage 2 — Canonical model
- Unify facial curves, head/gaze, skeleton joints, hand articulation, speech, emotion and events in a versioned multi-track session model.
- Keep source clocks, session time, confidence, validity, provenance and coordinate-space declarations.
- Add deterministic JSON debug interchange and binary take format.

## Stage 3 — Capture and synchronization
- Migrate camera discovery, async capture, frame fan-out, clock mapping, synchronization, calibration and source-health services.
- Support prerecorded video/audio and live capture using the same session model.

## Stage 4 — Solver workers
- Facial solver worker: landmarks/observations to canonical FACS/curve tracks.
- Body/hand worker: EasyMocap/SMPL-X/MANO to canonical joint tracks.
- SpeechFace worker/services: phoneme, viseme, coarticulation, prosody and expressive layers.
- Worker processes remain isolated from the C++ core dependency graph.

## Stage 5 — Hybrid performance
- Fuse face/head/body/hands independently by domain.
- Add temporal evaluation, confidence arbitration, missing-data policies and offline refine.
- Preserve each original source track for editing and diagnostics.

## Stage 6 — Studio and delivery
- One Qt Studio: capture, calibration, project/session browser, 3D viewport, multi-track timeline, retarget setup and export.
- Unreal/MetaHuman/LiveLink and DCC adapters.
- Android capture client as a companion application using the same versioned session protocol.

## Stage 7 — Validation and source retirement
- Build Windows x64 and Linux x86-64 from a clean checkout.
- Run all migrated unit, integration and golden-fixture tests.
- Validate representative Face Only, Body Only, Audio Driven and Hybrid sessions.
- Verify all model weights, licenses, generated assets, releases and submodules are accounted for.
- Produce a signed-off migration ledger with zero unclassified required features.
- Only then may the user delete the two old repositories.

## Explicit non-goals for the first migration commit

- Do not merge the old CMake projects directly.
- Do not copy build outputs, caches, .git directories or downloaded model weights blindly.
- Do not claim all functionality is integrated just because snapshots are present.
- Do not delete either source repository as part of migration automation.
