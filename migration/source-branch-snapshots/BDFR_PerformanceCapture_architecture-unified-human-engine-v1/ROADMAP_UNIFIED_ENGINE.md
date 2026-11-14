# BDFR Human Animation Engine — Roadmap

## U0 — Architecture baseline
- [x] Establish unified product scope and operating modes
- [x] Define layer boundaries and adapter policy
- [x] Define canonical performance data contract
- [x] Define non-destructive source migration/purge gates
- [ ] Review and approve coordinate convention and skeleton profile v1
- [ ] Add architecture decision records

## U1 — Canonical data foundation
- [ ] Implement versioned C++ domain types
- [ ] Implement face/head/body/hand validators
- [ ] Implement JSON debug serialization
- [ ] Implement binary take/stream serialization
- [ ] Add deterministic multi-rate timeline evaluator
- [ ] Add schema compatibility tests

## U2 — Capture and session
- [ ] Unified session/take/actor model
- [ ] Camera, video and audio source interfaces
- [ ] Clock mapping and synchronization service
- [ ] Calibration profiles and provenance
- [ ] Session recording and recovery

## U3 — Body and hand vertical slice
- [ ] EasyMocap worker protocol v1
- [ ] Worker supervisor and health monitoring
- [ ] SMPL-X/MANO normalization adapter
- [ ] Canonical skeleton and hand mapping
- [ ] Offline body/hand take import
- [ ] Golden fixture and quality metrics

## U4 — Facial vertical slice
- [ ] Import existing BDFR facial frames
- [ ] Canonical FACS/practical curve registry
- [ ] Face solver adapter and confidence mapping
- [ ] Facial temporal synthesis
- [ ] SpeechFace/viseme event tracks
- [ ] Expression and gaze layers
- [ ] Golden fixture and quality metrics

## U5 — Hybrid performance
- [ ] Shared session clock and multi-rate evaluation
- [ ] Face/head/body/hand domain fusion
- [ ] Root/head conflict policy
- [ ] Confidence-aware fallback and missing-data policy
- [ ] Hybrid regression fixtures

## U6 — Retarget and Studio
- [ ] Data-driven skeleton/morph retarget profiles
- [ ] 3D preview and canonical skeleton display
- [ ] Multi-track timeline and take review
- [ ] FBX/BVH/GLB export validation
- [ ] Unreal Engine adapter

## U7 — Production readiness
- [ ] Windows and Linux reproducible builds
- [ ] Automated regression and performance suite
- [ ] Real capture-to-output latency measurements
- [ ] Long-session soak and crash recovery
- [ ] License/provenance audit
- [ ] Migration coverage report
- [ ] Explicit user approval for source project retirement
