#!/usr/bin/env python3
"""Checks that relative links in the repo's Markdown files point at files that exist.
Run from the repo root:  python tools/ci/check_links.py

External (http/https/mailto) links are not checked, so the result never depends on other websites.
"""

import re
import subprocess
import sys
from pathlib import PurePosixPath

LINK = re.compile(r"\[[^\]]*\]\(\s*<?([^)\s>]+)>?(?:\s+\"[^\"]*\")?\s*\)")
FENCE = re.compile(r"^(```|~~~)")
INLINE_CODE = re.compile(r"`[^`]*`")
HTML_COMMENT = re.compile(r"<!--.*?-->", re.DOTALL)
EXTERNAL = re.compile(r"^(https?:|mailto:|#)", re.IGNORECASE)


def tracked() -> list:
    out = subprocess.run(["git", "ls-files", "-z"], capture_output=True, check=True).stdout
    return [p.decode("utf-8") for p in out.split(b"\0") if p]


def main() -> int:
    files = tracked()
    existing = set(files)
    dirs = {str(parent) for f in files for parent in PurePosixPath(f).parents}
    errors = []
    checked = 0

    for md in (f for f in files if f.lower().endswith(".md")):
        with open(md, encoding="utf-8") as fh:
            text = HTML_COMMENT.sub("", fh.read())
        in_fence = False
        for lineno, line in enumerate(text.splitlines(), 1):
            if FENCE.match(line.strip()):
                in_fence = not in_fence
                continue
            if in_fence:
                continue
            for target in LINK.findall(INLINE_CODE.sub("", line)):
                if EXTERNAL.match(target):
                    continue
                path = target.split("#", 1)[0].split("?", 1)[0]
                if not path:
                    continue
                base = PurePosixPath() if path.startswith("/") else PurePosixPath(md).parent
                resolved = _normalize(base / path.lstrip("/"))
                checked += 1
                if resolved not in existing and resolved not in dirs:
                    errors.append(f"{md}:{lineno}: broken link -> {target}")

    if errors:
        print("Docs link check FAILED:\n")
        for e in errors:
            print(f"  - {e}")
        return 1

    print(f"Docs link check passed ({checked} relative links).")
    return 0


def _normalize(path: PurePosixPath) -> str:
    parts = []
    for part in path.parts:
        if part == "..":
            if parts:
                parts.pop()
        elif part not in (".", ""):
            parts.append(part)
    return "/".join(parts) or "."


if __name__ == "__main__":
    sys.exit(main())
