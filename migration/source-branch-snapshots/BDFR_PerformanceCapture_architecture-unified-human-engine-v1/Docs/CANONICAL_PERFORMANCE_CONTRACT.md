# Canonical Performance Data Contract

Status: v1 design contract. The implementation must follow this contract or explicitly propose a versioned change.

## Design goals

The format must represent face, head, body, left/right hands, gaze and audio-driven articulation in one synchronized take. It must remain independent of any specific tracker, rig, solver, DCC tool or game engine.

## Required envelope

Every message/file record includes:

- `schema_id`: stable identifier, initially `bdfr.performance`
- `schema_version`: semantic version
- `session_id`, `take_id`, `actor_id`
- `sequence`: monotonically increasing sequence number within a stream
- `source_id` and `provider_id`
- `source_timestamp` and `source_clock_id`
- `session_time_seconds`: normalized monotonic session time
- `domain`: face, head, body, left_hand, right_hand, gaze, audio or event
- `coordinate_space` for spatial values
- `confidence` and validity/status metadata

A record without a valid session timestamp or schema version is rejected. Unknown optional fields are preserved where practical; unknown required major-version fields cause a compatibility error.

## Domain payloads

### Face

- named normalized curves (FACS Action Units and practical articulation curves)
- per-curve confidence and validity
- optional region confidence (brow, eyes, cheeks, nose, mouth, jaw)
- calibration/profile identifier
- optional solver diagnostics

Curve ranges are declared per curve in the registry; do not assume every curve has identical semantics merely because it is represented by a float.

### Head and gaze

- head transform in a declared coordinate space
- gaze direction or target representation with declared convention
- eye openness/blink values as named curves
- confidence and occlusion state

### Body

- skeleton profile identifier and schema version
- joint transforms or local rotations with an explicit joint-name/index map
- root transform
- per-joint confidence/validity
- rest-pose and unit metadata

### Hands

- side (left/right)
- hand schema/profile identifier
- wrist and finger joint data
- per-joint confidence/validity
- optional MANO/SMPL-X source metadata

### Audio and events

- audio segment references or features (not necessarily raw audio in the take)
- phoneme/viseme events with start/end times
- prosody observations
- emotion and authored event tracks
- source/provenance metadata

## Time semantics

- Session time is monotonic and measured in seconds.
- Frame index is never used as a substitute for time unless a constant frame rate is explicitly declared.
- Source clock, source timestamp and estimated clock offset are retained for diagnostics.
- Offline and live records share the same semantic model; live transport may use a compact binary encoding.

## Coordinate semantics

Every spatial payload must declare:
- handedness;
- axis directions;
- linear unit;
- rotation representation and order;
- whether transforms are world-space or parent/local-space;
- skeleton rest-pose/profile identity.

No implicit assumption about Unreal, Blender, OpenCV, SMPL or camera coordinate conventions is permitted.

## Confidence and missing data

Each domain and, where feasible, each element has:
- confidence in [0, 1];
- validity state (valid, inferred, occluded, missing, rejected);
- optional reason/diagnostic code.

Missing data is not encoded as a valid zero. Consumers must apply an explicit hold, decay, prediction or fallback policy.

## Track model

A take contains independently sampled tracks. Tracks may have different rates and timestamps. A timeline evaluator interpolates only where the track's interpolation policy permits it. Discrete events are not interpolated. The fusion stage emits a synchronized evaluation view without destroying original source tracks.

## Versioning and serialization

- Major version: incompatible semantic change.
- Minor version: backward-compatible optional fields or domain additions.
- Patch version: clarification or bug fix without semantic change.
- JSON is the human-readable debug/interchange representation.
- A compact binary representation is required for high-rate local streaming and production takes.
- Binary files must include magic bytes, version, bounded record lengths and recoverable/truncated-tail behavior.

## Initial acceptance tests

1. Reject invalid schema versions and non-finite values.
2. Preserve source and normalized timestamps.
3. Round-trip face, head, body and hand records through JSON and binary serializers.
4. Reject undeclared coordinate spaces for spatial payloads.
5. Keep missing/occluded values distinct from numeric zero.
6. Deterministically evaluate multi-rate tracks at requested session times.
7. Preserve unknown optional metadata through read/write where supported.
