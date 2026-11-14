# Unified Engine Architecture

## Product definition

BDFR Human Animation Engine is a single 64-bit-first platform for human performance capture, reconstruction, synthesis, editing, retargeting and delivery. It unifies facial animation, speech-driven performance, body/hand motion capture and synchronized multi-source capture.

## Top-level layers

1. **Application & Project** — projects, actors, sessions, takes, asset references, settings and recovery.
2. **Studio** — Qt desktop UI, viewport, timeline, capture monitor, calibration and export.
3. **Ingest** — cameras, prerecorded video, image sequences, microphones, prerecorded audio and external streams.
4. **Perception** — face landmarks, head pose, gaze, body pose, hand pose, audio features, phonemes and prosody.
5. **Solvers** — facial/FACS solver, body/SMPL-X solver, hand/MANO solver and speech-face synthesis.
6. **Canonical Performance Model** — timestamped, confidence-aware tracks for face, head, gaze, body, both hands, speech, emotion and events.
7. **Temporal Performance Engine** — clock mapping, timeline evaluation, temporal filters, coarticulation, procedural behavior, confidence arbitration and hybrid fusion.
8. **Retargeting** — skeleton and morph-target profiles independent of the source solver.
9. **Runtime & Export** — Unreal/LiveLink, MetaHuman/OpenRigLogic, ARKit-style curves, custom rigs, BVH/FBX/GLB and interchange formats.

## Operating modes

- Face Only
- Body Only
- Hybrid Face + Head + Body + Hands
- Audio Driven
- Offline Video/Audio Refine
- Live Multi-Camera

## Runtime boundaries

The core owns canonical types, timing, session/take semantics, validation, fusion and deterministic interchange. GPU-heavy or Python-first inference remains an out-of-process worker behind versioned contracts. Unreal, Android and DCC integrations remain adapters, not dependencies of the engine core.

## 64-bit platform policy

Desktop targets: Windows x86-64 and Linux x86-64. Android capture target: arm64-v8a where dependencies permit. All desktop builds reject 32-bit targets at CMake configure time and compile time. Use fixed-width integers for serialized data; never serialize native pointers, size_t or compiler-dependent struct layouts. File and wire formats specify endianness, field widths and schema versions.

## Migration policy

Original source projects are copied into read-only provenance snapshots first. Their files are then selectively promoted into the new module layout after dependency, license, API and regression review. Source projects are not deleted until the migration ledger is complete and build/test/feature gates pass.
