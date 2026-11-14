# Source Migration and Retirement Plan

This plan governs the eventual consolidation of BDFR_FacialAnimation and EasyMocap into BDFR Human Animation Engine.

## Non-destructive rule

No source repository is deleted, archived, renamed or purged during initial development. The new engine must first prove feature coverage and reproducible outputs. Retirement is a separate, explicit final operation after user review.

## Source projects

### BDFR_FacialAnimation

Candidate capabilities to assess and migrate:
- versioned facial frame and curve registry;
- FACS/practical facial curve model;
- curve mixing and temporal filtering;
- face capture and landmark/head-pose observations;
- speech analysis, phoneme/viseme timing and coarticulation;
- expression layers (blink, gaze, micro-expression, emotion, idle);
- retarget profiles;
- MetaHuman/OpenRigLogic and Unreal adapters;
- Android capture workflow and Studio tools.

Migration rule: preserve behavior and tests where possible, but move stable engine-independent contracts into the new canonical model. Keep Unreal, Android and UI-specific code behind adapters/apps.

### EasyMocap

Candidate capabilities to assess:
- multi-view and single-view body reconstruction workflows;
- SMPL/SMPL-X/MANO model support;
- hand/body pose output;
- calibration and camera conventions;
- worker launch/configuration;
- inference/runtime dependencies.

Migration rule: initially integrate as a versioned external worker. Do not copy its source into the core. Replace or internalize only if there is a demonstrated technical reason and license/provenance review.

### Existing BDFR PerformanceCapture foundation

The existing orchestration repository already contains useful work in camera capture, synchronization, calibration, source health, solver adapters, fusion, take recording, skeleton mapping, BVH export and a Qt Studio. Treat this as a donor implementation, not as proof that the new architecture is complete. Each subsystem must be reviewed against the new contracts and migrated or rewritten deliberately.

## Migration sequence

1. Inventory source repositories, branches, commits, licenses, tests, models and runtime requirements.
2. Freeze a set of representative facial, body, hand and hybrid recordings as regression fixtures.
3. Implement the canonical schema and validators.
4. Build importers for existing facial frames, EasyMocap outputs and BDFR takes.
5. Add adapters and compare outputs against source behavior.
6. Migrate facial synthesis and body reconstruction in separate vertical slices.
7. Migrate retargeting, Studio and engine adapters.
8. Run quality, stability, performance and license checks.
9. Publish a migration matrix showing every source capability as migrated, replaced, intentionally excluded or unresolved.
10. Ask for explicit final approval before archiving or deleting any source repository.

## Purge gate

A source project is eligible for retirement only when:
- all required capabilities have a mapped destination;
- representative test fixtures pass;
- output quality is no worse than the agreed baseline;
- build and runtime instructions are reproducible;
- third-party license obligations are satisfied;
- rollback/export of source history is retained;
- the user explicitly approves the retirement action.

Until every gate is met, source repositories remain intact and accessible.
