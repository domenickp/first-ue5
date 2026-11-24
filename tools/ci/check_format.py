#!/usr/bin/env python3
"""Checks (or fixes) C++ formatting against the repo's .clang-format.
Run from the repo root:
    python tools/ci/check_format.py          # check only (what CI runs)
    python tools/ci/check_format.py --fix    # reformat files in place

Uses clang-format 22.1.3 (the version CI pins). It's found via the CLANG_FORMAT environment
variable, then PATH, then the copies Visual Studio installs on Windows.
"""

import os
import shutil
import subprocess
import sys

EXPECTED_MAJOR = "22"
CANDIDATES = [
    r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-format.exe",
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\Llvm\x64\bin\clang-format.exe",
]
EXTENSIONS = (".h", ".hpp", ".cpp", ".inl")


def find_clang_format() -> str:
    for candidate in [os.environ.get("CLANG_FORMAT"), shutil.which("clang-format"), *CANDIDATES]:
        if candidate and os.path.isfile(candidate):
            return candidate
    sys.exit("clang-format not found. Install it (e.g. `pip install clang-format==22.1.3`) "
             "or set CLANG_FORMAT to its path.")


def main() -> int:
    fix = "--fix" in sys.argv[1:]
    exe = find_clang_format()

    version = subprocess.run([exe, "--version"], capture_output=True, text=True, check=True).stdout.strip()
    if f"version {EXPECTED_MAJOR}." not in version:
        print(f"warning: expected clang-format {EXPECTED_MAJOR}.x, found: {version}")

    out = subprocess.run(["git", "ls-files", "-z", "Source", "Plugins"], capture_output=True, check=True).stdout
    files = [p.decode("utf-8") for p in out.split(b"\0") if p and p.decode("utf-8").endswith(EXTENSIONS)]
    if not files:
        print("No C++ files to check.")
        return 0

    args = [exe, "-i"] if fix else [exe, "--dry-run", "-Werror"]
    result = subprocess.run(args + files)

    if fix:
        print(f"Formatted {len(files)} file(s).")
        return result.returncode
    if result.returncode != 0:
        print("\nC++ formatting check FAILED. Fix it locally with:  python tools/ci/check_format.py --fix")
        return 1
    print(f"C++ formatting check passed ({len(files)} files).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
