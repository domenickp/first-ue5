# Audio

Music matters a lot in this game.

## Music
- **Phases 1–2 (exploration):** an adventure track on a loop. Moody and a little dark, but
  earnest.
- **Castle yard to end of level (phases 2 and 3):** everything is synchronized to "House of
  Mirrors" by Arch Enemy (3:40). The level ends with the song. Timing details:
  [music-sync.md](music-sync.md).
- **If the song file isn't present** (it's never committed to this repo): play no music, but
  run the event timeline on a clock exactly as if the song were playing.
- **Failure warning:** the music itself warns the player as the corruption meter runs low
  (see [gameplay.md](gameplay.md)).
- **Adaptive?** <!-- Does the loop change as the player nears the castle? -->
- **References:** "House of Mirrors" by Arch Enemy

> **Licensing note:** "House of Mirrors" is commercially copyrighted. Using it in a personal
> learning project on your own computer is fine, but **don't commit the audio file to this
> public repo** or distribute a build that includes it without a license. Option: build the
> synchronized sequence around a placeholder track at a similar tempo, so either song can
> drop in.

## Sound design
<!-- Overall approach. Signature sounds: the boss roar, the portal opening, the evil weapons. -->


## Ambience per location
| Location | Ambient sounds | Music mood |
|---|---|---|
| Hilly landscape |  | Moody adventure loop |
| Castle yard |  | "House of Mirrors" |

## Voice
<!-- Any voice at all? Grunts, demon vocalizations? -->

