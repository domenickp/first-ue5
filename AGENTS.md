# Agent Instructions — first-ue5

A learning/experimental Unreal Engine 5 project. The repo is **public** (a friend follows along).

## Git workflow
- **Never push directly to `main`.** Make every change on a branch and open a pull request.
- Branch names: short and descriptive, e.g. `docs/story-answers`, `feat/camera-shift`.
- Open PRs with `gh pr create`. The owner reviews and merges them.
- Start new work from an up-to-date `main` (`git switch main && git pull`).

## Platforms
- Targets are **Windows and macOS (Apple Silicon)**. Keep all code cross-platform: use Unreal's
  platform-independent APIs and avoid Windows-only headers, libraries or path conventions.
- Mac builds are packaged on the owner's Mac, not cross-compiled from Windows.

## Content rules
- Don't commit copyrighted audio (e.g. "House of Mirrors" by Arch Enemy) or other unlicensed assets.
  Use placeholders in the repo.
- Licensed content lives under `Content/Licensed/`, which is gitignored. Code must handle it being
  absent (e.g. the music-sync timeline runs silently on a clock).
- Large binaries (`.uasset`, `.umap`, images, audio, 3D files) go through **Git LFS**. See
  `.gitattributes`. Never commit `Binaries/`, `Intermediate/`, `Saved/` or `DerivedDataCache/`.

## Build (Windows)
- Engine: Unreal Engine **5.8.4** at `C:\Program Files\Epic Games\UE_5.8`.
- Build the editor target from the command line (close the Unreal Editor first):
  ```
  "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" FirstUE5Editor Win64 Development -Project="<repo>\FirstUE5.uproject" -WaitMutex
  ```
- Toolchain: MSVC 14.50 (from the VS 2026 Build Tools install) + Windows SDK 10.0.22621. The
  required Visual Studio components are listed in `.vsconfig`. The Visual Studio Installer can
  import that file.
- Regenerate IDE project files: right-click `FirstUE5.uproject` → *Generate Visual Studio project files*.

## Checks before opening a PR
1. Build the editor target (above) when C++ changed. CI cannot compile Unreal code.
2. Run the Unreal automation tests (`FirstUE5.*`; command in `docs/SETUP.md`) when C++ or
   `MusicTimelines/` changed.
3. `python tools/ci/check_format.py --fix`, then stage everything and run
   `python tools/ci/check_repo.py`, `python tools/ci/check_links.py` and
   `python tools/ci/check_timelines.py`.
   CI (`.github/workflows/checks.yml`) runs the same four scripts on every PR.

## Music-driven scenes
- Timelines are CSV files in `MusicTimelines/`, authored by the owner. Treat them as content:
  don't rewrite cue times or descriptions without being asked.
- `AMusicDirector` plays the song and fires cues; the song's playback position is the clock.
  Game code reacts to cues by `Id` (bind to `OnCue`), never by hard-coded times.

## Layout
- `docs/`: design docs (`pitch.md`, `gameplay.md`, `story.md`, `music-sync.md`, ...).
  `docs/README.md` explains what each one holds.
- The Unreal project (`FirstUE5.uproject`, `Source/`, `Content/`, `Config/`) lives at the repo root.
- `MusicTimelines/`: music-scene timelines (CSV), read at runtime and shipped with packaged builds.
- `tools/ci/`: the check scripts CI runs.
