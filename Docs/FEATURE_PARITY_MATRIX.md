# Feature Parity Matrix

This matrix distinguishes exact source preservation from compilation, canonical integration and end-to-end validation. A copied source file is not automatically a production-ready unified feature.

| Capability | Preserved source | Superbuild compiled | Canonical engine adapter | End-to-end validated |
|---|---|---|---|---|
| FACS / facial curves / CurveMixer | Yes | Yes | Yes, curve mixer adapter | Unit/integration test only |
| Facial solver frame | Yes | Yes | Yes, face/head/gaze adapter | Unit/integration test only |
| Face camera tracking / MediaPipe | Yes | Core module yes; optional MediaPipe runtime separate | Partial | No live-camera test |
| Head pose and gaze | Yes | Yes | Yes, raw named channels | Unit/integration test only |
| Audio analysis / pitch / prosody | Yes | Yes | Yes, audio-buffer to canonical speech energy/pitch samples | Synthetic voiced-audio integration test; no microphone/hardware test |
| Prosody-driven nonverbal behavior | Yes | Yes | Yes, pause/emphasis cues to canonical blink, head-nod, breath and emphasis tracks | Synthetic segmented-audio adapter test |
| Text-driven viseme planning and facial-curve synthesis | Yes | Yes | Yes, heuristic grapheme-to-viseme planning and canonical facial-curve sampling | Synthetic text-to-lip-curve integration test |
| Timed phoneme-to-viseme adapter | Yes | Yes | Yes, provider-neutral timed phoneme frames mapped through English/Persian LanguageProfile into canonical speech and facial curves | Synthetic English/Persian phoneme adapter tests; no ASR provider or recorded-speech alignment test |
| ASR / forced-alignment provider contract | Partial source-side speech utilities | Yes | Provider-neutral transcribe-and-align interface feeds timed phoneme adapter; no concrete model backend bundled | Fake-provider integration test; no recorded-speech model acceptance test |
| Procedural facial behavior (blink, gaze, head motion) | Yes | Yes | Yes, BehaviorEngine to canonical face/head/gaze tracks | Synthetic adapter regression test |
| Emotion poses and instant facial events | Yes | Yes | Yes, EmotionEngine and InstantEventGenerator to canonical face/emotion tracks | Synthetic happiness and blink-event adapter tests |
| Emotion / personality-driven expression synthesis | Yes | Yes | Partial; deterministic behavior profile adapter only | No emotion-recognition or personality acceptance test |
| Multi-camera synchronization / clock mapping | Yes | Yes | Via capture fusion bridge | No hardware test |
| Domain routing and confidence fusion | Yes | Yes | Yes, FusionCore bridge | Synthetic test |
| SMPL24 / SMPL-X skeleton parsing | Yes | Yes | Yes, SkeletonPose adapter | Synthetic test |
| Body root / joint hierarchy | Yes | Yes | Parent indices and quaternion conversion | Synthetic test |
| Coordinate basis and unit conversion | Yes | Yes | Signed-axis permutation, handedness conversion and unit scaling for skeleton/hand transforms | Synthetic vector/quaternion conversion tests |
| Hand PCA channels | Yes | Yes | Preserved as named scalar channels | Synthetic test |
| Full MANO/SMPL-X hand articulation | Yes | Yes | Generic 6-component basis expansion to canonical 25-joint hands; licensed MANO/SMPL-X basis assets remain external | Synthetic basis regression test; no official model-basis acceptance test |
| OpenCV camera acquisition / calibration | Yes | Optional build target | Adapter boundary present | Not enabled in default CI |
| BVH export | Yes | Yes | Direct canonical export pending | No |
| Retarget profiles / MetaHuman mapping | Yes | Yes | Yes, adapter to existing Unreal Mannequin and MetaHuman Body profiles | Unreal Mannequin synthetic regression test; MetaHuman runtime not tested |
| FaceBuilder / eos | Yes | Stub target built; eos optional | Adapter boundary present | No photo-to-3D test |
| OpenRigLogic / MetaHuman DNA | Yes | Stub target built; OpenRigLogic optional | Adapter boundary present | No MetaHuman runtime test |
| Session / take recording and playback | Yes | Yes | Versioned codec plus streaming recorder, frame-level validation, seekable playback core | Codec tests plus recorder/player synthetic tests; UI and long-recording stress test pending |
| Qt desktop Studio | Yes | Not in unified target | Unified UI pending | No |
| Android capture app | Yes | Not part of desktop CI | Companion client integration pending | No |
| Unreal plugin / LiveLink | Yes | Not built in generic CI | Adapter boundary present | Requires Unreal toolchain |
| Model weights / licensed data | No tracked weights found | N/A | Runtime acquisition only | License/asset review pending |

## Acceptance interpretation

- **Source migration complete:** all 326 tracked files are preserved and SHA-verified.
- **Build integration complete at baseline:** both predecessor C++ modules are part of the 64-bit superbuild and have passed Windows/Linux CI.
- **Core runtime integration in progress:** canonical adapters exist for face, head, gaze, capture fusion, body skeleton, hand channels, audio energy/pitch analysis, text-planned viseme events, timed phoneme-to-viseme frames with English/Persian profiles, text-driven facial lip curves, procedural blink/gaze/head motion, emotion/instant-event tracks and prosody-driven blink/nod/breath/emphasis cues.
- **Product consolidation not complete:** the unified Studio, Android client, concrete ASR/forced-alignment model backend and recorded-speech acceptance for the provider adapter, sensor extrinsic calibration, rig-specific basis authoring and official MANO/SMPL-X hand-basis integration, runtime recorder/playback UI, Studio, Android and hardware end-to-end suite remain open.
