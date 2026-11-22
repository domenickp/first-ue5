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
- **Open question:** is phase 1 a true 2D game, or a 3D world viewed from the side (often
  called 2.5D)? A 3D world makes the swing into first person seamless. Recommended.
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
<!-- What happens if the player dies? Restart the level? Checkpoints at each phase? -->


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

