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
- [x] Native facial solver frame adapter (face curves, head pose and gaze).
- [x] SMPL/SMPL-X skeleton adapter (root transform, joint hierarchy, shape/expression and hand-PCA channels).
- [x] Canonical skeleton to Unreal Mannequin / MetaHuman Body retarget-profile adapter.
- [x] Configurable capture FusionCore to canonical frame bridge.
- [x] Facial CurveMixer integration helper.
- [x] Audio-buffer adapter for windowed energy, prosody and pitch samples.
- [x] Text-driven grapheme-to-viseme planning and canonical facial-curve sampling (heuristic; not an acoustic phoneme recognizer).
- [x] Procedural blink, gaze and head-motion adapter with degree-to-radian normalization.
- [x] EmotionEngine pose mapping and instant facial-event mapping (blink, double blink, brow flick, lip twitch, nose flare, jaw clench) into canonical face/emotion tracks.
- [x] Prosody-driven audio behavior adapter mapping speech pauses and emphasis to canonical blink, head-nod, breath and emphasis cues.
- [x] Provider-neutral timed phoneme adapter mapping externally aligned English/Persian phonemes through LanguageProfile into canonical speech and facial-viseme frames.
- [x] Provider-neutral ASR/transcribe-and-align interface and adapter from provider output to canonical speech/viseme frames.
- [ ] Concrete ASR/forced-alignment backend, model packaging/licensing and recorded-speech acceptance tests.
- [x] Cross-module integration regression tests for facial, skeleton, audio, text-viseme and procedural behavior paths.
- [x] Release test targets explicitly retain assertions (NDEBUG disabled) so CI assertions execute in Release builds.
- [ ] Latest expanded integration CI run pending; see GitHub Actions for current Windows/Linux status.
- [x] Canonical signed-axis permutation, handedness-aware quaternion conversion and unit scaling for skeleton and hand samples.
- [ ] Camera/sensor extrinsic calibration, per-device calibration workflow and rig-specific basis-profile authoring.
- [x] Generic six-component hand-PCA basis expansion into canonical 25-joint hand samples; model basis remains caller-supplied.
- [x] Coordinate profile transforms for skeleton root/joints and hand palm/joints.
- [ ] Integrate authorized MANO/SMPL-X basis assets and validate against official model outputs.
- [x] Versioned canonical session serialization and deterministic round-trip/corruption/schema validation tests.
- [x] Streaming session recorder with incremental frame validation and monotonic timestamp enforcement.
- [x] Seekable session playback core with playback-rate control.
- [ ] Schema migration fixtures, long-session stress testing and UI recording/playback controls.
- [ ] Production ASR/forced alignment and speech-driven lip-sync using a licensed model backend (provider interface and coarticulation sampler are implemented).
- [ ] Full live camera/audio/face/body/hand end-to-end tests.
- [ ] Unified Qt Studio and Android capture application.

## Licensing and external assets
- [x] Third-party dependency and model-asset review register created at `Docs/THIRD_PARTY_DEPENDENCIES.md`.
- [ ] Select and add an explicit project license.
- [ ] Review upstream licenses, MetaHuman/SMPL/MANO terms and downloadable model assets.

## Repository retirement
- [x] Evidence-based cutover assessment created at `Docs/CUTOVER_READINESS.md`.
- [ ] Product parity, live hardware acceptance, legal/asset clearance and history/asset retention checks remain open.

**Decision: NOT READY to delete either original repository.** The destination is suitable as the active integration repository, but the source repositories must remain available until the blockers in `Docs/CUTOVER_READINESS.md` are closed.
