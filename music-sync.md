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
- **Length:** <!-- m:ss -->
- **Tempo (BPM):** <!-- optional: tap along at a site like "tap BPM", or leave blank -->
- **Placeholder track for the public repo:** <!-- the real song isn't committed; see audio.md -->

## Sections
| Start | Section | Intensity | Notes on how it sounds |
|---|---|---|---|
| 0:00 | Intro |  |  |
|  |  |  |  |

## Events
<!-- One row per thing that happens. "Type" = spawn / boss / scripted / camera / checkpoint / other. -->
| Time | Type | Event | Notes |
|---|---|---|---|
| 0:00 | scripted | Player enters the yard; camera swings to first person |  |
|  | scripted | Weapon pickup (player has no control) |  |
|  | spawn | Wave 1: fierce low-level demons pour from the door and windows |  |
|  | checkpoint | Wave 1 must be cleared by here |  |
|  | spawn | Wave 2: stronger minions |  |
|  | boss | Hexagram portal opens; boss rises out of the ground |  |
|  | scripted | Boss roars; player thrown into the fence |  |
|  | boss | Boss fight begins |  |
|  | checkpoint | Boss must be dead by here |  |
| *end* | scripted | Song ends; transformation begins |  |

## "Keeping up": the failure rule
Decide what falling behind means. Some options (pick one, mix them, or invent your own):

- **A. Checkpoint deadlines:** at each checkpoint, if the required enemies aren't dead, the player dies.
  Simple and very clear to the player.
- **B. Overwhelm:** waves keep arriving on schedule whether or not the last one is cleared. Too many
  enemies alive at once and the player is overrun and dies. Feels natural, but is harder to tune.
- **C. Corruption meter:** a meter drains over time and fills with kills. If it empties, the evil
  weapons consume the player. This would tie the failure state into the story.

**Chosen rule:** <!-- -->

**Does the player get any warning?** <!-- e.g. the music distorts, the screen darkens, a sound cue -->

**On death:** <!-- restart the castle fight and the song from the beginning? -->
