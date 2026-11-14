# ADR-0001: Create a New Unified Human Animation Engine

- Status: Accepted for implementation
- Date: 2026-11-14

## Context

Facial animation and body motion reconstruction have separate dependency stacks, release cycles and quality concerns. Maintaining them as independent applications creates duplicated operational work. Directly merging one project into the other would preserve historical architectural assumptions.

## Decision

Create BDFR Human Animation Engine as a new product architecture with a canonical, rig-independent performance model. Facial and body reconstruction remain separate internal domains connected through shared session time, explicit coordinate conventions and versioned contracts. Solver implementations may run in separate worker processes.

The existing BDFR PerformanceCapture repository is used as an isolated implementation workspace/donor foundation on branch `architecture/unified-human-engine-v1`; this does not mean the old architecture is accepted unchanged. Source projects remain untouched.

## Consequences

- Face Only, Body Only, Hybrid and Audio Driven workflows share one application/session model.
- Facial and body solvers can be upgraded independently.
- Existing code is migrated only after contract and regression review.
- Source repositories are not purged until migration gates pass and explicit approval is given.
- Coordinate and skeleton conventions must be decided before broad solver migration.
