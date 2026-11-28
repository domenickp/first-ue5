// Music timeline data: the list of cues (sections, lyrics, events, checkpoints) that a
// music-driven scene follows, loaded from a CSV file in the project's MusicTimelines/ folder.
//
// CSV format (first row is the header; column order doesn't matter):
//   Time,Kind,Id,Description,Intensity
//   0:21,Section,VocalsStart,Sudden scream starts the song,Peak
//   0:21,Event,Wave1,Wave 1 pours out of the castle,
//
//   Time        m:ss, m:ss.s or plain seconds (e.g. 1:07, 1:07.5, 67)
//   Kind        Section | Lyric | Event | Checkpoint | End
//   Id          short name with no spaces; level objects react to cues by this Id
//   Description free text (quote it if it contains a comma)
//   Intensity   optional; mainly for sections (e.g. Building, Peak, Release)
// Blank lines and lines starting with # are ignored.

#pragma once

#include "CoreMinimal.h"
#include "MusicTimeline.generated.h"

UENUM(BlueprintType)
enum class EMusicCueKind : uint8
{
	/** A change in the music (a new part of the song). Sets the "current section". */
	Section,
	/** A recognisable lyric. */
	Lyric,
	/** Something that happens in the game. */
	Event,
	/** A moment the player must have achieved something by. */
	Checkpoint,
	/** The end of the timeline. */
	End
};

USTRUCT(BlueprintType)
struct FIRSTUE5_API FMusicCue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	float TimeSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	EMusicCueKind Kind = EMusicCueKind::Event;

	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	FString Intensity;

	/** Line number in the CSV file, for error messages. */
	UPROPERTY(BlueprintReadOnly, Category = "Music Sync")
	int32 SourceLine = 0;
};

struct FIRSTUE5_API FMusicTimelineParseResult
{
	/** Valid cues, sorted by time. Cues with the same time keep their order from the file. */
	TArray<FMusicCue> Cues;

	/** Problems found while parsing, e.g. "line 7: unknown Kind 'Evnt'". Bad rows are skipped. */
	TArray<FString> Errors;

	bool HasErrors() const { return Errors.Num() > 0; }
};

namespace MusicTimeline
{
/** Parses "m:ss", "m:ss.s" or plain seconds. Returns false for anything else (or negative times). */
FIRSTUE5_API bool ParseTime(const FString& Text, float& OutSeconds);

/** Formats seconds as m:ss (e.g. 67.4 -> "1:07"). */
FIRSTUE5_API FString FormatTime(float Seconds);

/** Parses one kind name, case-insensitively. */
FIRSTUE5_API bool ParseKind(const FString& Text, EMusicCueKind& OutKind);

/** Splits one CSV line into fields. Supports "quoted, fields" and "" as an escaped quote. */
FIRSTUE5_API TArray<FString> SplitCsvLine(const FString& Line);

/** Parses a whole timeline CSV. */
FIRSTUE5_API FMusicTimelineParseResult Parse(const FString& CsvText);

/** Absolute path of a timeline file in the project's MusicTimelines/ folder. */
FIRSTUE5_API FString GetTimelinePath(const FString& FileName);

/** Loads and parses a timeline file. A missing file is reported as an error. */
FIRSTUE5_API FMusicTimelineParseResult LoadFile(const FString& FilePath);
} // namespace MusicTimeline
