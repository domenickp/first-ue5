# Music Sync — Castle Fight

The whole castle fight runs on the timeline of "House of Mirrors". In this scene the game is a
music video for the song: the lyrics and the changes in the music tell the story, and the action
follows them.

> **The live timeline is [`MusicTimelines/HouseOfMirrors.csv`](../MusicTimelines/HouseOfMirrors.csv).**
> The game reads that file directly, so edit it to change the scene. The tables further down are
> the original cue sheet it was built from, kept as the design record.

## Editing the timeline
Open `MusicTimelines/HouseOfMirrors.csv` in Excel, Google Sheets or any text editor. Each row is
one **cue**:

| Column | What to write | Example |
|---|---|---|
| **Time** | When it happens: `m:ss` (or `m:ss.s`, or plain seconds) | `1:07` |
| **Kind** | `Section` (a change in the music), `Lyric`, `Event` (something happens in the game), `Checkpoint`, or `End` | `Event` |
| **Id** | A short name, one word with no spaces. Level objects react to this name. | `Wave2` |
| **Description** | Anything. Wrap it in quotes if it contains a comma. | `Wave 2: stronger minions` |
| **Intensity** | Optional, mainly for sections | `Peak` |

- Rows can be in any order; the game sorts them by time.
- Lines starting with `#` are comments.
- There must be exactly one `End` row, and nothing after it.
- Save it as **CSV**. If Excel asks, keep the CSV format.

**To see your changes:** save the file and press **Play**. The game re-reads the file every
time the scene starts, so you don't need to rebuild anything. Mistakes (like a mistyped
time) appear in red on screen and are also caught on the PR by the *Music timelines* check.

## Seeing it play
While a music-driven scene runs, development builds show (on screen):
- a status line: the song clock, the current **section** and the **next** cue;
- each cue as it fires, colour-coded: sections green, lyrics blue, events white,
  checkpoints yellow, the end purple.

In the editor, select the **Music Director** in the level to change these settings in the
Details panel:
- **Debug Start At Seconds:** start part-way through, e.g. `110` to jump straight to the
  transformation instead of waiting through the whole song.
- **Song:** the song asset (from `Content/Licensed/`). Leave it empty to run silently.
- **Show Debug Display:** turn the on-screen text off.

## Blocking out a scene with stand-ins
Until real enemies and effects exist, place **Music Cue Reactor** objects (grey cubes) to see
the choreography. Each one has a list of **Reactions**: a cue **Id** plus what to do:

| Reaction | What it does |
|---|---|
| **Show** | Appears. (Starts hidden.) |
| **Hide** | Disappears. |
| **Light On / Light Off** | Turns its light on or off (red by default, e.g. glowing windows). |
| **Rise** | Rises out of the ground. (Starts underground.) |

Example: a boss stand-in with *BossEmerge → Rise* and *BossDead → Hide*.

**Backspace** (the debug reset) also rewinds the scene, so you can replay it as often as you like.

## How to fill this in
1. Play the song with a timer (or in a free audio editor like Audacity, which shows the time).
2. Note the time each **section** starts: intro, verse, chorus, solo, breakdown and so on.
   Precise to the second is plenty for now.
3. For each section, describe the **intensity** you want (calm, building, peak, release).
4. Then add **events**: what spawns, what the boss does, any scripted moments.
5. Mark **checkpoints**, the moments the player has to have done something by.

You don't need to know any music theory. "The drums kick in hard at 0:42 and that's when the
windows burst" is exactly the kind of detail that's needed.

## Song facts
- **Length:** 3:40
- **Tempo (BPM):** 85
- **Placeholder track for the public repo:** If the track file doesn't exist, default to no music.

## Sections
| Start | Section | Intensity | Notes on how it sounds |
|---|---|---|---|
| 0:00 | Intro | Building | Instrumental intro |
| 0:11 | Synth Start | Building | Imposing Sound |
| 0:21 | Vocals Start | Peak | Sudden scream to Start the Song |
| 0:55 | First Refrain | Peak | Slow down of music to unrelenting pace |
| 1:00 | "Sharpened Blade" Lyric | Peak | Recognizable Lyric |
| 1:06 | "Welcome Home" Lyric | Peak | Recognizable Lyric |
| 1:07 | Fast drumming | Peak | Pace increase |
| 1:40 | Second Refrain | Peak | Slow down of music to unrelenting pace |
| 1:56 | "Distort Your Flesh and Bone" Lyric | Peak | Recognizable Lyric |
| 2:03 | "Welcome Home" Second Lyric | Peak | Recognizable Lyric |
| 2:04 | Grinding Guitar | Peak | Slow down of music to unrelenting pace |
| 3:20 | "Welcome Home" Third Lyric | Peak | Recognizable Lyric |
| 3:33 | Glass Breaking | Release | Sound of breaking glass, end of song |


## Events
<!-- One row per thing that happens. "Type" = spawn / boss / scripted / camera / checkpoint / other. -->
| Time | Type | Event | Notes |
|---|---|---|---|
| 0:00 | scripted | Player enters the yard; camera swings to first person |  |
| 0:08 | scripted | Weapon pickup (player has no control) |  |
| 0:11 | scripted | Castle windows and doors light up red |  |
| 0:21 | spawn | Wave 1: fierce low-level demons pour from the door and windows |  |
| 0:54 | checkpoint | Wave 1 must be cleared by here |  |
| 0:55 | boss | Hexagram portal opens; boss rises out of the ground with sword |  |
| 1:06 | scripted | Boss roars; player thrown into the fence |  |
| 1:07 | spawn | Wave 2: stronger minions |  |
| 1:08 | boss | Boss fight begins |  |
| 1:40 | checkpoint | Boss must be dead by here |  |
| 1:55 | checkpoint | Wave 2 must be cleared by here |  |
| 1:56 | scripted | Transformation begins |  |
| 2:03 | scripted | Transformation ends |  |
| 2:04 | spawn | Wave 3: minions spawn but are scared and run away |  |
| 0:55 | spawn | (Player is still fighting the last of wave 1 while the boss emerges) | from scope.md |
| 3:20 | checkpoint | Wave 3 must be cleared by here |  |
| 3:20 | scripted | Godlike demon emerges; instructs the player to keep purging corrupted souls | from scope.md |
| 3:33 | scripted | Player smashes personal artifact and pledges fealty to demon |  |

## "Keeping up": the failure rule
Decide what falling behind means. Some options (pick one, mix them, or invent your own):

- **A. Checkpoint deadlines:** at each checkpoint, if the required enemies aren't dead, the player dies.
  Simple and very clear to the player.
- **B. Overwhelm:** waves keep arriving on schedule whether or not the last one is cleared. Too many
  enemies alive at once and the player is overrun and dies. Feels natural, but is harder to tune.
- **C. Corruption meter:** a meter drains over time and fills with kills. If it empties, the evil
  weapons consume the player. This would tie the failure state into the story.

**Chosen rule:** C: Corruption meter.  It should be calculated realtive to the checkpoints.

**Does the player get any warning?** The music is the warning.

**On death:** Restart the castle fight and the song from the beginning
