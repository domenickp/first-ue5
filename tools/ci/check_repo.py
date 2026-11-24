#!/usr/bin/env python3
"""Repository hygiene checks. Run from the repo root:  python tools/ci/check_repo.py

Fails if:
  1. Generated or private folders are committed (Binaries/, Intermediate/, Saved/,
     DerivedDataCache/, Content/Developers/, IDE files).
  2. Anything under Content/Licensed/ is committed (copyrighted music lives there).
  3. A file that should be in Git LFS (see .gitattributes) was committed as a raw binary.
  4. A large file (> 1 MB) was committed outside Git LFS.

Reads committed content from git's object database, so it works whether or not the LFS
files have been downloaded.
"""

import re
import subprocess
import sys

LARGE_FILE_BYTES = 1 * 1024 * 1024
LFS_POINTER_PREFIX = b"version https://git-lfs.github.com/spec/v1"

FORBIDDEN = [
    (re.compile(r"^(Binaries|Intermediate|Saved|DerivedDataCache)/"), "generated Unreal folder"),
    (re.compile(r"^Plugins/.+/(Binaries|Intermediate)/"), "generated plugin folder"),
    (re.compile(r"^Content/Developers/"), "per-user editor folder"),
    (re.compile(r"^Content/Licensed/"), "licensed content (must never be published)"),
    (re.compile(r"(^|/)\.vs/"), "Visual Studio cache"),
    (re.compile(r"\.(sln|slnx|suo|VC\.db|VC\.opendb)$"), "generated IDE file"),
]


def git(*args: str, data: bytes = None) -> bytes:
    return subprocess.run(["git", *args], input=data, capture_output=True, check=True).stdout


def tracked_files():
    """Yield (blob_sha, path) for every file in the index."""
    for entry in git("ls-files", "-s", "-z").split(b"\0"):
        if not entry:
            continue
        meta, path = entry.split(b"\t", 1)
        mode, sha, _stage = meta.split(b" ")
        if mode == b"160000":  # submodule
            continue
        yield sha.decode(), path.decode("utf-8")


def lfs_paths(paths):
    """Return the set of paths whose 'filter' attribute is 'lfs'."""
    out = git("check-attr", "-z", "--stdin", "filter", data="\0".join(paths).encode("utf-8") + b"\0")
    parts = out.split(b"\0")
    result = set()
    for i in range(0, len(parts) - 2, 3):
        path, _attr, value = parts[i], parts[i + 1], parts[i + 2]
        if value == b"lfs":
            result.add(path.decode("utf-8"))
    return result


def blob_sizes_and_heads(shas):
    """Return {sha: (size, first_bytes)} using one `git cat-file --batch` call."""
    proc = subprocess.run(["git", "cat-file", "--batch"], input="\n".join(shas).encode() + b"\n",
                          capture_output=True, check=True)
    out = proc.stdout
    result = {}
    pos = 0
    while pos < len(out):
        header_end = out.index(b"\n", pos)
        sha, _kind, size = out[pos:header_end].split(b" ")
        size = int(size)
        body_start = header_end + 1
        result[sha.decode()] = (size, out[body_start:body_start + len(LFS_POINTER_PREFIX)])
        pos = body_start + size + 1  # skip body and trailing newline
    return result


def main() -> int:
    files = list(tracked_files())
    paths = [p for _, p in files]
    errors = []

    for path in paths:
        for pattern, reason in FORBIDDEN:
            if pattern.search(path):
                errors.append(f"{path}: {reason} must not be committed")
                break

    lfs = lfs_paths(paths)
    blobs = blob_sizes_and_heads(sorted({sha for sha, _ in files}))
    for sha, path in files:
        size, head = blobs[sha]
        is_pointer = head.startswith(LFS_POINTER_PREFIX)
        if path in lfs and not is_pointer:
            errors.append(f"{path}: should be stored in Git LFS but was committed as a regular file "
                          f"(run `git lfs install`, then `git rm --cached` and re-add it)")
        elif path not in lfs and size > LARGE_FILE_BYTES:
            errors.append(f"{path}: {size / 1024 / 1024:.1f} MB committed outside Git LFS "
                          f"(add its extension to .gitattributes)")

    if errors:
        print("Repo hygiene check FAILED:\n")
        for e in errors:
            print(f"  - {e}")
        return 1

    print(f"Repo hygiene check passed ({len(paths)} files, {len(lfs)} in Git LFS).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
