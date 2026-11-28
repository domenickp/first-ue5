#!/usr/bin/env python3
"""Validates the music timeline files in MusicTimelines/*.csv.
Run from the repo root:  python tools/ci/check_timelines.py

Uses the same rules as the game's parser (Source/FirstUE5/MusicSync/MusicTimeline.cpp), so a
timeline that passes here loads in the game without errors:
  - header row with at least Time, Kind and Id columns (any order, any case)
  - Time is m:ss, m:ss.s or plain seconds
  - Kind is Section, Lyric, Event, Checkpoint or End
  - Id is a single word with no spaces
  - there is an End cue, and nothing comes after it
Blank lines and lines starting with # are ignored.
"""

import csv
import io
import re
import subprocess
import sys

KINDS = {"section", "lyric", "event", "checkpoint", "end"}
TIME_MINUTES = re.compile(r"^(\d+):(\d+(?:\.\d+)?)$")
TIME_SECONDS = re.compile(r"^\d+(?:\.\d+)?$")


def parse_time(text: str):
    text = text.strip()
    m = TIME_MINUTES.match(text)
    if m:
        seconds = float(m.group(2))
        return int(m.group(1)) * 60 + seconds if seconds < 60 else None
    if TIME_SECONDS.match(text):
        return float(text)
    return None


def check_file(path: str) -> list:
    errors = []
    with open(path, encoding="utf-8-sig", newline="") as fh:
        lines = fh.read().splitlines()

    header = None
    cues = []  # (time, kind, line)
    for lineno, raw in enumerate(lines, 1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        fields = [f.strip() for f in next(csv.reader(io.StringIO(line)))]

        if header is None:
            header = {name.lower(): i for i, name in enumerate(fields)}
            missing = [c for c in ("time", "kind", "id") if c not in header]
            if missing:
                return [f"{path}:{lineno}: header is missing column(s): {', '.join(missing)}"]
            continue

        def field(name):
            i = header.get(name)
            return fields[i] if i is not None and i < len(fields) else ""

        time = parse_time(field("time"))
        if time is None:
            errors.append(f"{path}:{lineno}: bad Time '{field('time')}' (use m:ss, e.g. 1:07)")
            continue
        kind = field("kind").lower()
        if kind not in KINDS:
            errors.append(f"{path}:{lineno}: unknown Kind '{field('kind')}' "
                          f"(use Section, Lyric, Event, Checkpoint or End)")
            continue
        cue_id = field("id")
        if not cue_id or " " in cue_id:
            errors.append(f"{path}:{lineno}: Id '{cue_id}' must be a single word with no spaces")
            continue
        cues.append((time, kind, lineno))

    if header is None:
        return [f"{path}: file is empty (expected a header row: Time,Kind,Id,Description,Intensity)"]

    ends = [c for c in cues if c[1] == "end"]
    if not ends:
        errors.append(f"{path}: no End cue (add a row like: 3:40,End,SongEnd,Song ends,)")
    elif len(ends) > 1:
        errors.append(f"{path}: more than one End cue (lines {', '.join(str(e[2]) for e in ends)})")
    else:
        end_time = ends[0][0]
        for time, kind, lineno in cues:
            if time > end_time:
                errors.append(f"{path}:{lineno}: cue at {time:g}s comes after the End cue")
    return errors


def main() -> int:
    out = subprocess.run(["git", "ls-files", "-z", "MusicTimelines"], capture_output=True, check=True).stdout
    files = [p.decode("utf-8") for p in out.split(b"\0") if p and p.decode("utf-8").lower().endswith(".csv")]
    errors = [e for f in files for e in check_file(f)]

    if errors:
        print("Music timeline check FAILED:\n")
        for e in errors:
            print(f"  - {e}")
        return 1
    print(f"Music timeline check passed ({len(files)} file(s)).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
