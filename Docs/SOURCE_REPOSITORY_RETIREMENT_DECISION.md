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


## Follow-up export completed on 2026-11-14

- A commit-metadata archive has been added at `migration/LEGACY_GIT_AND_RELEASE_ARCHIVE.json`.
- It contains 472 commits from `BDFR_FacialAnimation/main`, 82 commits from `BDFR_PerformanceCapture/main`, and 90 commits from `BDFR_PerformanceCapture/architecture/unified-human-engine-v1`. Each record retains the original SHA, parent SHA(s), full message, author/committer identity and timestamps, and source commit URL.
- Both source repositories currently return zero GitHub Issues (open or closed) and zero open/closed pull requests in the inspected repository endpoints; therefore there are no issue records to recreate.
- The two FacialAnimation Releases and all three asset names, sizes and original download URLs are recorded in the archive JSON.

## Important limitation — not a full Git mirror or Release import

The commit archive is **metadata preservation**, not a native Git object database: it does not contain commit trees/blobs and does not make original SHAs available as traversable commits in the destination repository. A true history migration requires `git clone --mirror` followed by `git push --mirror` (or a Git bundle import). That operation could not be completed in this environment because direct GitHub network access for Git transport is unavailable.

Likewise, release metadata and asset URLs are preserved, but the three binary release assets have **not** been uploaded into the destination. The available GitHub actions do not expose release creation/asset upload, and direct asset downloads were unavailable. Keep the original releases accessible until their binaries are uploaded and checksums verified in the destination.

Consequently, the source repositories are frozen for new development but are **not yet eligible for archive or deletion**. Do not represent the history or release binaries as fully migrated.


## Final migration verification — 2026-11-14

The hosted migration workflow completed successfully: [run 43596123322](https://github.com/lyingtiger88/BDFR-Human-Animation-Engine/actions/runs/43596123322).

- 326/326 tracked source files passed exact Git blob SHA comparison.
- Native commit histories imported and enumerated: 472 FacialAnimation commits, 82 PerformanceCapture main commits, and 90 PerformanceCapture architecture-branch commits (644 total).
- Two legacy release tags and three release assets imported.
- All three release assets passed SHA-256 checks before upload and after downloading from the destination.
- Both source repositories had zero Issues and zero Pull Requests in the inspected API listings.

**History rewrite note:** GitHub's built-in Actions token refused workflow-file pushes without the separate Workflows permission. To avoid a personal access token, historical `.github/workflows/` files were relocated to inert `legacy-workflows/` paths during history import. This rewrites commit SHAs while retaining the commit graph and content. Original source SHAs remain in the metadata archive. The imported workflow YAML files are not active destination workflows.

**Final status: READY FOR OWNER-MANAGED MANUAL DELETION**, provided any untracked local assets, external model weights/datasets, secrets, or organization-level settings have been separately checked. The old repositories remain unchanged and have not been deleted.
