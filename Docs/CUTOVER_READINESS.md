# Repository Cutover Readiness

Assessment date: 2026-11-14
Destination: `lyingtiger88/BDFR-Human-Animation-Engine`

## Decision

**NOT READY to delete the two source repositories.**

The source snapshot migration is complete and verified, and the current destination commit passes the Windows x64 and Linux x86-64 CI matrix. This establishes that the tracked source snapshot is preserved and the current integrated C++ targets build and pass their automated tests. It does not establish full product parity, complete runtime validation, or legal permission to redistribute all third-party components.

## Verified facts

- Facial source repository HEAD: `c090e6ab4612373e2c701f7ace39d69b68f242a5`.
- Performance Capture source repository HEAD: `4c36eeb06402cd16d635aa46bf9b5fce12aec832`.
- Both source repositories still point to those exact commits at assessment time.
- The migration manifest contains 326 tracked source files: 244 facial and 82 performance-capture files.
- The migration status records exact destination-path and Git blob SHA matching for all 326 files.
- The latest destination main commit is `334b05c6f7acbbc692a91587126bc645721d3677`.
- GitHub Actions run `43582582223` completed successfully for that commit; both Windows x64 and Linux x86-64 jobs passed their build and CTest steps.
- Neither source repository exposes a recognized repository-level license; no LICENSE/COPYING/NOTICE file was found at either source root. Do not infer permission to relicense or redistribute upstream dependencies and model assets.

## Remaining blockers before product retirement

1. **Application parity:** unified Qt Studio and Android capture client are not integrated into the destination product.
2. **Runtime completeness:** canonical session serialization/playback, full audio-to-speech/lip-sync path, full hand articulation, coordinate calibration and complete retarget-profile authoring remain incomplete.
3. **Hardware acceptance:** no complete live camera + audio + face + body + hands + target-rig end-to-end acceptance run has been recorded.
4. **Legal and asset clearance:** project licensing decision, upstream notices, model/weight permissions, SMPL/SMPL-X/MANO and MetaHuman terms require review.
5. **Repository history:** source files are preserved as snapshots, but original Git commit histories, issues, releases, pull requests, secrets, and untracked local files are not migrated.

## Safe cleanup path

Do not delete either source repository now. When the blockers above are closed:

1. Freeze both source repositories and record their final commit IDs and full tree hashes.
2. Re-run `migration/verify_snapshot.py` and the Windows/Linux CI matrix on the release commit.
3. Complete and sign off the feature parity and live hardware acceptance matrix.
4. Complete legal/asset clearance and add the chosen project license and third-party notices.
5. Export and retain source Git bundles (or archive the repositories) if their histories, issues, releases or audit trail may be needed.
6. Prefer **archive** over delete for an initial retention period. Delete only after a separate explicit owner decision and verified backups.

## Current recommendation

The new repository is suitable as the active integration/development repository. The two old repositories are not yet safe to delete as product sources or legal/audit references. They can be considered for archival only after the owner confirms that no open work, issues, releases or untracked assets remain there.
