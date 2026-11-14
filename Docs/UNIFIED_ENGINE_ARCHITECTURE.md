# BDFR Human Animation Engine — Unified Architecture

Status: Architecture baseline v1.0 (proposed; implementation is staged behind this branch).

## Product objective

Build one engine-independent platform that reconstructs a temporally coherent human performance from video, cameras and audio, then previews, edits, retargets and exports the result. Face, body, hands, gaze and speech are first-class domains. The product must support Face Only, Body Only and Hybrid operation without maintaining separate applications.

## Architectural rules

1. The canonical performance model is the product boundary. Trackers, neural models, solvers, DCC tools and game engines are replaceable providers.
2. Observation, reconstruction, performance synthesis, fusion, retargeting and presentation are separate stages.
3. Every sample carries a timestamp, timebase, coordinate-space declaration, schema version and confidence/provenance metadata.
4. Missing or low-confidence data is explicit. It must never silently become a confident neutral pose.
5. Offline processing may use future context; live processing must declare its latency budget and causal mode.
6. No model weights, third-party source or datasets are copied into the core without provenance and license review.
7. Existing repositories remain untouched on this branch until migration and regression gates pass.

## System layers

```text
                 BDFR Human Animation Engine
                              |
             +----------------+----------------+
             |                                 |
       Capture & Ingest                  Session / Project
   video, cameras, audio, files          metadata, takes, assets
             |                                 |
             +----------------+----------------+
                              |
                       Observation Bus
             face landmarks / body pose / hands /
             head pose / gaze / audio features
                              |
                    Reconstruction Services
             +----------------+----------------+
             |                |                |
        Facial Solver    Body Solver      Hand Solver
        FACS / curves    SMPL-X / pose    MANO / joints
             |                |                |
             +----------------+----------------+
                              |
                    Canonical Performance
                    Frame + Track Model
                              |
                 Temporal Fusion & Synthesis
          sync / confidence / filtering / constraints /
             speech / emotion / gaze / micro-motion
                              |
                     Retargeting Service
            skeleton profiles / morph profiles / units
                              |
                    Preview & Export Layer
          viewport / timeline / FBX / BVH / GLB / runtime
                              |
                 Unreal Engine / DCC / Other
```

## Core modules

- **Foundation**: versioning, time, coordinate spaces, IDs, diagnostics, plugin contracts.
- **Capture**: camera and file sources, audio input, device discovery, calibration and synchronization.
- **Perception**: face landmarks, head pose, body pose, hands, gaze and audio observations.
- **Facial**: FACS representation, facial solving, speech articulation, expression behavior and curve mixing.
- **Body**: body reconstruction, canonical skeleton mapping, kinematics and temporal stabilization.
- **Hands**: finger articulation and hand confidence; may initially be supplied by SMPL-X/MANO body solvers.
- **Performance**: canonical tracks, time alignment, confidence-aware fusion, constraints and procedural layers.
- **Retargeting**: data-driven skeleton and morph/curve mapping profiles.
- **Studio**: session browser, viewport, timeline, calibration, review and export.
- **Adapters**: isolated integration processes/plugins for EasyMocap, existing BDFR facial runtime, Unreal Engine and DCC tools.

## Process and dependency boundaries

The C++ core owns canonical data, scheduling, synchronization, fusion, retargeting and deterministic file formats. Heavy inference may run in separate worker processes using Python/CUDA or other runtimes. Worker boundaries use a versioned control protocol and a documented data protocol. The core must not import a solver's private model classes.

EasyMocap remains an external Python worker initially. Its outputs are normalized at the adapter boundary. The existing BDFR FacialAnimation implementation is treated as a source of facial capabilities and algorithms; it is not linked as an inseparable application dependency. Selected components are migrated only after API, license, test and behavior review.

## Canonical coordinate and time policy

- Internal time is monotonic session time in seconds, represented with 64-bit precision.
- Source timestamps and source clock identifiers are retained alongside normalized session time.
- Every spatial sample declares its coordinate basis, handedness, axis convention and units.
- Canonical world convention and skeleton rest-pose policy are specified in the data contract before solver migration.
- Conversion happens once at ingestion or retarget boundaries, never implicitly inside arbitrary modules.

## Operating modes

- **Face Only**: facial observations and facial synthesis can produce a complete facial take.
- **Body Only**: body and hand reconstruction can produce a body take.
- **Hybrid**: domains are fused on the shared timeline; face, head, body and each hand may have different providers.
- **Audio Driven**: audio observations generate speech articulation and optional expressive behavior; can be combined with video capture.
- **Offline Refine**: full-clip temporal analysis may revise live estimates and emit a refined take.

## Quality gates

A module is not considered integrated merely because it compiles. Each integration must provide:
- deterministic unit tests for its data contract;
- recorded sample fixtures with provenance;
- numerical validation for timestamps, coordinate conversion and confidence;
- regression comparisons against the source project's known outputs;
- latency/throughput measurements for live paths;
- a documented failure and fallback policy.

## Initial delivery slices

1. Canonical data contract and schema validation.
2. Session/take container and deterministic serialization.
3. Adapter SDK and worker lifecycle.
4. Body reconstruction adapter (EasyMocap) and facial adapter.
5. Timeline synchronization and confidence-aware domain fusion.
6. Preview, retarget profiles and export.
7. Studio workflow and Unreal integration.

## Repository strategy

This branch is an isolated architecture and migration workspace in the existing BDFR PerformanceCapture repository. The default branch and both source projects are not modified by this architecture commit. The long-term product identity is **BDFR Human Animation Engine**; repository naming can be finalized after the first vertical slice is validated.
