# Setup

How to get **first-ue5** building on a new machine.

| | Version used |
|---|---|
| Unreal Engine | **5.8.4** |
| Visual Studio (Windows) | **2026** (18.x) with the MSVC **14.50** C++ toolset |
| Windows SDK | **10.0.22621** |
| Git LFS | 3.x |

---

## Windows

### 1. Git and Git LFS
1. Install **Git for Windows**: https://git-scm.com/download/win. It includes Git LFS.
2. Turn on Git LFS once per computer:
   ```bash
   git lfs install
   ```

### 2. Unreal Engine 5.8.4
1. Install the **Epic Games Launcher**: https://www.unrealengine.com/download
2. In the launcher: **Unreal Engine → Library → +** next to *Engine Versions*, choose **5.8.4**, then
   click **Install**. The default location is `C:\Program Files\Epic Games\UE_5.8`.

### 3. Visual Studio 2026
Unreal needs Visual Studio's C++ compiler, even if you never write code in Visual Studio.

1. Download **Visual Studio 2026 Community** (free): https://visualstudio.microsoft.com/downloads/
2. **Easiest way:** clone the repo first (step 4), then in the **Visual Studio Installer** click
   **More → Import configuration** and select the repo's **`.vsconfig`** file. That selects every
   required component.
3. **Or pick the components by hand:**
   - **Workloads:** *Desktop development with C++*, *Game development with C++* (with *IDE support
     for Unreal Engine* and *Unreal Engine debugger* checked), *.NET desktop development*
   - **Individual components:** *MSVC v14.50 – VS 2026 C++ x64/x86 build tools*, *Windows 11 SDK
     (10.0.22621.0)*, *.NET Framework 4.6.2 targeting pack*, *C++ Clang Compiler for Windows*

> **Compiler version note:** Unreal 5.8.4 blocks some early 14.50 compiler versions (up to
> 14.50.35722) because of a crash bug. The installer folder may be *named* `14.50.35717` even
> though the compiler inside is newer (e.g. 14.50.35740), which is fine. The build log shows the
> real version on its `Using Visual Studio ... toolchain` line.

### 4. Get the project
```bash
git clone https://github.com/domenickp/first-ue5.git
```
If Unreal assets ever show up as tiny text files, Git LFS wasn't on when cloning. Fix it with:
```bash
git lfs pull
```

### 5. Build and open
**Option A, simplest:** double-click **`FirstUE5.uproject`**. Unreal says the project's modules
are missing and asks whether to rebuild them. Click **Yes**. The first build takes a few minutes.

**Option B, command line** (shows full errors if something goes wrong):
```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" FirstUE5Editor Win64 Development -Project="%CD%\FirstUE5.uproject" -WaitMutex
```
Run it from the repo folder with the Unreal Editor closed. It should end with `Result: Succeeded`.

**To edit C++ in Visual Studio:** right-click `FirstUE5.uproject` → **Generate Visual Studio
project files**, then open `FirstUE5.sln`. (The `.sln` is generated and not committed.)

---

## macOS (Apple Silicon)

> Not yet tested on this project. Expect to update this section after the first Mac build.

1. **Xcode:** install it from the Mac App Store, open it once so it can install its components, then
   run:
   ```bash
   sudo xcode-select -s /Applications/Xcode.app
   ```
   Check Epic's [hardware and software requirements](https://dev.epicgames.com/documentation/unreal-engine/hardware-and-software-specifications-for-unreal-engine)
   for the Xcode version that UE 5.8 needs.
2. **Git LFS:** `brew install git-lfs && git lfs install` (needs [Homebrew](https://brew.sh)).
3. **Unreal Engine 5.8.4:** install it through the Epic Games Launcher for Mac. The default
   location is `/Users/Shared/Epic Games/UE_5.8`.
4. **Clone** the repo (same as Windows, step 4).
5. **Build:**
   ```bash
   "/Users/Shared/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh" FirstUE5Editor Mac Development -Project="$PWD/FirstUE5.uproject" -WaitMutex
   ```
   Or double-click `FirstUE5.uproject` and let Unreal rebuild the modules.

---

## PR checks
Every pull request runs three automatic checks on GitHub (`.github/workflows/checks.yml`). They
**don't compile the game**, because GitHub's servers don't have Unreal Engine, so still build
locally before opening a PR.

| Check | Catches | Run it locally |
|---|---|---|
| **Repo hygiene** | Generated folders (`Binaries/`, `Intermediate/`...), anything in `Content/Licensed/`, assets that skipped Git LFS, large non-LFS files | `python tools/ci/check_repo.py` |
| **Docs links** | Broken relative links in Markdown files | `python tools/ci/check_links.py` |
| **C++ formatting** | Code that doesn't match `.clang-format` (Unreal style) | `python tools/ci/check_format.py`. Add `--fix` to reformat. |

The scripts check the files currently staged in git, so run `git add` first. The format check
uses the clang-format that Visual Studio installs (version 22.1.3, the same version CI uses).

## Licensed music
The castle sequence is synced to "House of Mirrors" by Arch Enemy, which is **not** in the repo.
If you own a copy and want to use it locally, import it into **`Content/Licensed/`** in the
editor. That folder is gitignored, so it can never be committed. Without it, the game runs the
same timeline silently.

## Troubleshooting
| Problem | Fix |
|---|---|
| Unreal assets are tiny text files / won't open | `git lfs install`, then `git lfs pull` |
| "Modules are missing or built with a different engine version" | Click **Yes** to rebuild. If that fails, use the command-line build (Option B) to see the error |
| The project in Unreal's *Recent Projects* list won't open | The folder moved. Double-click `FirstUE5.uproject` in the repo instead |
| Build can't find a compiler or SDK | Re-import `.vsconfig` in the Visual Studio Installer (Windows step 3) |
