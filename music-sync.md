# Music Sync — Castle Fight

The whole castle fight runs on the timeline of "House of Mirrors". This file is the **cue sheet**:
a timeline of the song with what happens in the game at each point. Fill in the song's sections
first, then attach events to them.

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
| 3:20 | checkpoint | Wave 3 must be cleared by here |  |
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
