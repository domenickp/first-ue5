# Gameplay

The level has three phases with different controls and camera. See [story.md](story.md)
for the order of events.

## Core loop

| Phase | Camera | Loop |
|---|---|---|
| 1. The Hunt | Side-scroll | Move → platform over obstacles → fight weak enemies → advance toward the castle |
| 2. The Castle Yard | First person | Survive waves → kill the boss |
| 3. The Reversal | <!-- first person? --> | Chase fleeing demons → eliminate all of them |

## Player verbs
<!-- Tag each one [MUST] / [WANT] / [DREAM]. -->
- **Phase 1:** move, jump, basic melee attack, shoot a low-power firearm
- **Phase 2:** move, aim/look, melee with the evil weapon, shoot the evil firearm
- **Phase 3:** <!-- same as phase 2? new demon abilities? -->

## Controls
| Action | Keyboard/Mouse | Gamepad |
|---|---|---|
| Move | WASD | Left stick |
|  |  |  |

## A five-minute play session
<!-- Write out a short stretch of play in second person:
     "You wake up in ___. You see ___. You press ___ and ___ happens." -->


## Systems

### System: Camera / perspective shift
- **Purpose:** Pillar 1, "the camera tells the story"
- **How it works:** A side-view camera follows the player through phase 1. On entering the
  castle yard, it swings smoothly into the character's eyes.
- **Decided:** phase 1 is a **3D world viewed from the side** (2.5D). Movement is locked to a
  path through the 3D level. The same character, level and camera system carry straight into
  first person with no cut or loading.
- **Requirement:** the transition must be seamless. This is the game's signature moment.
- **Priority:** [MUST]

### System: Music sync (castle fight)
- **Purpose:** Pillar 3, "the music drives the climax"
- **How it works:** everything from entering the yard to the end of the level runs on the
  song's 3:40 timeline. Waves, the boss emergence, the transformation and other events fire at
  set points in the song. The level ends when the song ends.
- **Failure:** if the player doesn't keep up with the sequence, they die. See *Corruption meter*.
- **On death:** restart the castle fight and the song from the beginning.
- **Details:** see [music-sync.md](music-sync.md)
- **Priority:** [MUST]

### System: Corruption meter
- **Purpose:** the "keeping up" rule for the synced fight. It also ties failure into the story:
  fall behind and the evil weapons consume you.
- **Rule (chosen):** the meter drains over time and fills with kills. If it empties, the player
  dies. It's calculated relative to the checkpoints in [music-sync.md](music-sync.md).
- **Draft of how "relative to the checkpoints" could work** (for discussion):
  - Each stretch between checkpoints has a kill target (e.g. "all of wave 1 by 0:54").
  - The meter compares **how far through the kills** the player is against **how far through
    the time** they are. Ahead of pace = meter high; behind pace = meter falls.
  - A buffer gives some slack, so a player slightly behind isn't killed instantly.
  - Reaching a checkpoint on time refills or resets the meter for the next stretch.
- **Warning:** the music. As the meter falls, the song gets distorted or muffled, for example.
  <!-- Exact effect TBD. -->
- **Phase 3:** <!-- does the meter still apply after the transformation, maybe as "hunger"? -->
- **Priority:** [MUST]

### System: Combat, phase 1 (side-scroll)
- **How it works:** basic melee and low-power firearms against weak enemies
- **Feel:** predictable, platformer-style
- **Priority:** [MUST]

### System: Combat, phase 2 (FPS)
- **How it works:** a powerful melee weapon and firearm imbued with evil; Doom-like pace
- **Priority:** [MUST]

### System: Wave spawning
- **How it works:** enemies emerge from the castle's door and windows and charge the player.
  Wave 1 is fierce low-level enemies; wave 2 is stronger minions plus the boss.
- **Priority:** [MUST]

### System: Health & damage
- **How it works:** the player takes damage (the fence impact is scripted). <!-- what happens at 0 health? -->
- **Priority:** [MUST]

### System: Fleeing-enemy AI (phase 3)
- **How it works:** demons run away from the player instead of attacking
- **Priority:** [MUST]

## Progression
Fixed and story-driven: basic weapons → evil weapons → demon form. No upgrades or skill tree.

## Difficulty & failure
- **Castle fight:** falling behind the music sequence kills the player (corruption meter).
  On death, the castle fight and the song restart from the beginning.
- **Phase 1:** <!-- what happens on death in the side-scroller? -->


## Enemies / challenges

### Low-level enemies (phase 1)
- **Look:** only slightly evil
- **Behavior:** <!-- -->
- **Where it appears:** the landscape

### Fierce low-level demons (wave 1)
- **Behavior:** pour out of the castle door and windows and charge the player
- **Where it appears:** the castle yard

### Stronger minions (wave 2)
- **Behavior:** <!-- -->
- **Where it appears:** the castle yard, alongside the boss

### Boss demon
- **Entrance:** rises out of the ground through a hexagram portal and roars, which throws the player
- **Attacks / behavior:** <!-- -->
- **How the player beats it:** <!-- -->

### Fleeing demons (phase 3)
- **Behavior:** run away from the player
- **Where it appears:** <!-- the whole level? the yard only? -->

## UI / HUD
<!-- Health bar? Ammo? Does the HUD change between phases or after the transformation? -->

