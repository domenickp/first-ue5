# first-ue5

A learning/experimental game project in **Unreal Engine 5.8**: a single action/horror level where a
demon hunter becomes the thing they hunt. It starts as a side-scroller and swings into first person
for a fight synchronized to music.

- **Design docs:** [docs/](docs/README.md), starting with the [pitch](docs/pitch.md)
- **Engine:** Unreal Engine 5.8, C++
- **Platforms:** Windows and macOS (Apple Silicon)

## Getting the project
**Full setup instructions (Windows and macOS): [docs/SETUP.md](docs/SETUP.md)**

This repo uses **Git LFS** for large binary files (Unreal assets, images, audio). Install it once
before cloning:

```bash
git lfs install
git clone https://github.com/domenickp/first-ue5.git
```

Without Git LFS, Unreal assets download as small text pointer files and won't open.

## Music
The castle sequence is synced to a commercially licensed song that is **not** in this repo.
Without it the game runs the same timeline silently. See [docs/audio.md](docs/audio.md).
