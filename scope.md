# Scope & Priorities

## Team & resources
- **Who's working on it:**
- **Hours per week, roughly:**
- **Skills available:** <!-- programming, 3D, animation, audio, writing... -->
- **Budget for assets/tools:**

## Platforms
- **Windows PC:** primary development and test machine.
- **macOS:** also a target. Tested on an M5 MacBook Pro.
  - Mac builds must be packaged on the Mac (requires Xcode).
  - Do a Mac test build at each milestone, starting with the camera-transition prototype, so
    Mac-specific problems (graphics features, performance) show up early.
  - Lumen and Nanite support on Mac is more limited than on Windows. Check what the chosen
    engine version supports before the art direction depends on it.
  - Unsigned builds shared with others trigger macOS security warnings. That's fine for testing
    (right-click → Open). Code signing ($99/yr Apple Developer account) only matters for a public
    release.

## Vertical slice
<!-- The first real goal: one short, polished chunk that proves the core loop and the look. -->
- **Which level / area:**
- **Length:** <!-- e.g. 10–15 minutes of play -->
- **Systems it must include:**
- **What "done" looks like:**

## Feature priority list
| Feature | Priority | Notes |
|---|---|---|
|  | MUST |  |
|  | WANT |  |
|  | DREAM |  |

## Out of scope (for now)
<!-- Things you're deliberately NOT doing. Writing these down protects the project. -->
-

## Open questions
<!-- Anything you're unsure about. These become discussion topics. -->
- ~~2D or 3D side view?~~ **3D world viewed from the side.** The seamless transition is essential.
- ~~What's synced to the song?~~ **The whole castle fight.** It ends with the song; falling behind = death.
- ~~What does "keeping up" mean?~~ **Corruption meter**, relative to the checkpoints. Draft in gameplay.md.
- ~~What happens on death in the castle?~~ Restart the castle fight and the song.
- ~~Music in phase 3?~~ The song continues; the whole level ends with it.
- Can the player act while the boss emerges (0:55–1:06)?
  - Yes, the player should be kept busy fighting the last of wave 1.
- Does phase 3 stay in first person, and does the corruption meter still apply there?
  - Phase 3 should be first person.
- Where does phase 3 happen: the yard, or back across the whole level? (A 76-second window suggests the yard.)
  - The yard.
- What happens between 3:20 (wave 3 deadline) and 3:33 (artifact smash)?
  - Emergence of a god like demon that instructs the player to continue to purge corrupted souls.
- What is the personal artifact, and where is it introduced?
  - The artifact should be referenced in the introduction text, being gifted as a sign of allegience to the ruler.
- What happens on death in the side-scroller?
  - Restart about one screen back, or possibly at one of a few set checkpoints.
