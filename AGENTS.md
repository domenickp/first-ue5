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

## Docs layout
Design docs are the markdown files at the repo root (`pitch.md`, `gameplay.md`, `story.md`, ...).
See `README.md` for what each one holds.
