# Source Repository Retirement Decision

Date: 2026-11-14
Destination: `lyingtiger88/BDFR-Human-Animation-Engine`

## Decision

- Make `BDFR-Human-Animation-Engine` the sole active development/integration repository.
- Freeze feature development in `BDFR_FacialAnimation` and `BDFR_PerformanceCapture`.
- Do **not** delete or archive either source repository yet.
- Preserve their current state, release artifacts and branch content until the retirement gates below are closed.
- No source repository has been deleted, archived, or modified by this decision.

## Verified inventory

| Source repository | Tracked snapshot in destination | Other repository content found |
|---|---:|---|
| `BDFR_FacialAnimation` | 244 files | Two GitHub Releases with three downloadable artifacts; main branch only; no open issues or PRs; no repository license declared |
| `BDFR_PerformanceCapture` | 82 files | Additional branch `architecture/unified-human-engine-v1` is 8 commits ahead of main; no open issues or PRs; no GitHub Releases; no repository license declared |

The two main-branch source snapshots account for 326 files and are recorded in `migration/SNAPSHOT_MANIFEST.json`. The source main HEAD commits match the pinned source revisions in that manifest. This is a file snapshot, not a migration of Git history.

## Additional content requiring preservation

### BDFR_FacialAnimation release assets

- [BDFR Studio Windows test build](https://github.com/lyingtiger88/BDFR_FacialAnimation/releases/download/studio-test-latest/BDFR_Studio_Windows.zip) — 23,023,626 bytes.
- [BDFR Android hardware test APK](https://github.com/lyingtiger88/BDFR_FacialAnimation/releases/download/hardware-test-latest/BDFR_Android_Hardware_Test.apk) — 54,221,734 bytes.
- [BDFR Windows hardware test kit](https://github.com/lyingtiger88/BDFR_FacialAnimation/releases/download/hardware-test-latest/BDFR_Windows_Hardware_Test_Kit.zip) — 177,544 bytes.

These release binaries are not part of the 326-file source snapshot. Keep the original releases available until each artifact is either copied, checksum-verified and retained in the destination, or explicitly declared obsolete.

### BDFR_PerformanceCapture architecture branch

The branch `architecture/unified-human-engine-v1` is 8 commits ahead of `main` and contains eight changed files: architecture and migration design documents, a canonical performance data contract, a preliminary `PerformanceFrame.h` contract, its test, and a CMake change. These are not included in the 326-file main-branch snapshot. Their exact file contents and original blob SHAs are preserved under:

`migration/source-branch-snapshots/BDFR_PerformanceCapture_architecture-unified-human-engine-v1/`

This is a source-preservation copy only. It is not automatically approved as production code or compiled into the engine.

## Required retirement gates

1. Review and disposition the eight commits on the additional PerformanceCapture branch.
2. Export and retain Git bundles or equivalent full-history backups for both source repositories, including all branches and tags.
3. Copy/checksum/retain the three FacialAnimation release artifacts, or explicitly approve their obsolescence.
4. Inventory model weights, datasets, local/untracked files, workflows, secrets/configuration and third-party notices.
5. Complete license and redistribution review; neither source repository currently declares a GitHub repository license.
6. Complete product parity and hardware acceptance against representative face/body/hand/audio workflows.
7. Re-run the source snapshot verification and approved CI matrix on the final destination release commit.
8. Obtain explicit owner approval for archive/delete.

## Operational status

**Active repository:** `BDFR-Human-Animation-Engine`

**Frozen source repositories:**
- `BDFR_FacialAnimation`
- `BDFR_PerformanceCapture`

**Retirement status:** NOT CLEARED FOR ARCHIVE OR DELETION.

Recommended final action, after all gates pass: archive both old repositories first, retain them through an agreed rollback period, and only consider permanent deletion after a separate explicit approval and verified backups.
