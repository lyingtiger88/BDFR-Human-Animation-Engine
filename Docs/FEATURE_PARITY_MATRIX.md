# Feature Parity Matrix

This matrix distinguishes exact source preservation from compilation, canonical integration and end-to-end validation. A copied source file is not automatically a production-ready unified feature.

| Capability | Preserved source | Superbuild compiled | Canonical engine adapter | End-to-end validated |
|---|---|---|---|---|
| FACS / facial curves / CurveMixer | Yes | Yes | Yes, curve mixer adapter | Unit/integration test only |
| Facial solver frame | Yes | Yes | Yes, face/head/gaze adapter | Unit/integration test only |
| Face camera tracking / MediaPipe | Yes | Core module yes; optional MediaPipe runtime separate | Partial | No live-camera test |
| Head pose and gaze | Yes | Yes | Yes, raw named channels | Unit/integration test only |
| Audio analysis / pitch / prosody | Yes | Yes | Not yet | No |
| Speech / phoneme / viseme / coarticulation | Yes | Yes | Not yet | No |
| Emotion / personality / procedural behavior | Yes | Yes | Not yet | No |
| Multi-camera synchronization / clock mapping | Yes | Yes | Via capture fusion bridge | No hardware test |
| Domain routing and confidence fusion | Yes | Yes | Yes, FusionCore bridge | Synthetic test |
| SMPL24 / SMPL-X skeleton parsing | Yes | Yes | Yes, SkeletonPose adapter | Synthetic test |
| Body root / joint hierarchy | Yes | Yes | Parent indices and quaternion conversion | Synthetic test |
| Hand PCA channels | Yes | Yes | Preserved as named scalar channels | Synthetic test |
| Full MANO/SMPL-X hand articulation | Yes | Yes | PCA preserved; expansion not complete | No |
| OpenCV camera acquisition / calibration | Yes | Optional build target | Adapter boundary present | Not enabled in default CI |
| BVH export | Yes | Yes | Direct canonical export pending | No |
| Retarget profiles / MetaHuman mapping | Yes | Core source yes; proprietary runtime optional | Canonical profile layer pending | No |
| FaceBuilder / eos | Yes | Stub target built; eos optional | Adapter boundary present | No photo-to-3D test |
| OpenRigLogic / MetaHuman DNA | Yes | Stub target built; OpenRigLogic optional | Adapter boundary present | No MetaHuman runtime test |
| Session / take recording and playback | Yes | Yes | Canonical serialization bridge pending | Legacy tests only |
| Qt desktop Studio | Yes | Not in unified target | Unified UI pending | No |
| Android capture app | Yes | Not part of desktop CI | Companion client integration pending | No |
| Unreal plugin / LiveLink | Yes | Not built in generic CI | Adapter boundary present | Requires Unreal toolchain |
| Model weights / licensed data | No tracked weights found | N/A | Runtime acquisition only | License/asset review pending |

## Acceptance interpretation

- **Source migration complete:** all 326 tracked files are preserved and SHA-verified.
- **Build integration complete at baseline:** both predecessor C++ modules are part of the 64-bit superbuild and have passed Windows/Linux CI.
- **Core runtime integration in progress:** canonical adapters exist for face, head, gaze, capture fusion, body skeleton and hand channels.
- **Product consolidation not complete:** the unified Studio, Android client, full audio-driven path, coordinate calibration, canonical take format, complete retargeting and hardware end-to-end suite remain open.
