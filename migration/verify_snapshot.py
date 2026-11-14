#!/usr/bin/env python3
"""Verify every migrated source snapshot against its original Git blob SHA."""
from __future__ import annotations
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "migration" / "SNAPSHOT_MANIFEST.json"


def git_blob_sha(path: Path) -> str:
    result = subprocess.run(
        ["git", "hash-object", "--", str(path.relative_to(ROOT))],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def main() -> int:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    checked = 0
    failures = []
    for source in data["sources"]:
        for entry in source["files"]:
            path = ROOT / entry["destination_path"]
            if not path.is_file():
                failures.append(f"MISSING: {entry['destination_path']}")
                continue
            actual = git_blob_sha(path)
            expected = entry["blob_sha"]
            checked += 1
            if actual != expected:
                failures.append(
                    f"SHA MISMATCH: {entry['destination_path']} "
                    f"expected={expected} actual={actual}"
                )
    print(f"Verified {checked} source snapshot files.")
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print("All source snapshot files match their original Git blob SHA values.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
