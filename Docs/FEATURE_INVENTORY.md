# Source Feature Inventory

This is the initial capability inventory derived from the two source repositories' tracked README, roadmap, architecture, build files and source tree. Status describes source-repository claims and must be independently revalidated in the destination.

## Facial Animation source

- FACS Action Units and canonical facial curves
- Curve registry/catalog, curve mixing, constraints, dynamics and corrective rules
- Expression stack, region masks, priority, freeze and partial re-solve
- Timeline tracks: audio, text/dialogue, generated speech, emotion, events, mocap, gaze/head and manual overrides
- Sequence playback, filtering, cleanup, diagnostics, comparison and key reduction
- Project/session/take persistence, JSON and binary codecs, undo/redo and recovery foundations
- WAV/audio feature processing, pitch/prosody, transcript timing, dialogue markup/compiler and language profiles
- Speech providers, text-to-speech integration points, phoneme/viseme and coarticulation foundations
- Procedural behavior, emotion, personality, blink, gaze, eye darts and subtle motion
- Face observations, camera/video capture contracts, offline video processing and MediaPipe paths
- FaceBuilder/eos 3D morphable-model fitting path
- External mocap CSV and curve stream adapters
- MetaHuman/OpenRigLogic adapter, ARKit 52 mapping, generic retargeting
- Unreal Engine plugin and LiveLink receiver/runtime
- Qt Studio, face preview, MetaHuman viewport, CLI and diagnostic tools
- Android CameraX/MediaPipe capture, offline video workflow, model management and live transfer
- Windows/Linux core CI and Android CI workflows

## Performance Capture source

- N-source capture abstraction and camera device discovery
- Async camera capture, bounded frame queues and fan-out
- Multi-camera timestamp synchronization, clock offset estimation, jitter and drop telemetry
- Intrinsics, distortion, extrinsics, rig calibration and reprojection diagnostics
- Source health, timeout, recovery and preferred/fallback routing
- Face/head/body/left-hand/right-hand domain fusion
- BDFR facial UDP adapter and EasyMocap TCP/worker adapter
- SMPL24, compact SMPL-X 87 and expanded SMPL-X 165 parsing/mapping
- Canonical skeleton, kinematics and UE Mannequin/MetaHuman bone mapping
- Root/joint basis correction and retarget profiles
- Take recording, random-time reading, session indexing and binary BDFR Take v1
- BVH export for Blender/Maya
- Qt Performance Studio, calibration wizard, camera preview, 3D skeleton viewport and playback
- Diagnostics, probes, benchmarks and Windows/Linux CI

## Integration rules

- Preserve independent Face Only and Body Only paths.
- Hybrid fusion is per-domain and confidence-aware; no forced lockstep solver timestamps.
- Preserve original source timestamps and normalized session time.
- Do not vendor EasyMocap, MediaPipe, eos models, MetaHuman DNA or other third-party assets without license/provenance review.
- Mark every feature as snapshot-copied, integrated, tested, deferred, replaced or excluded in the migration ledger.
