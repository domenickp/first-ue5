// Runs a music-driven scene: plays the song and fires each cue from the timeline CSV when the
// song reaches it. The song is the master clock. If the song asset is missing (it's licensed and
// never committed), the same timeline runs silently on the game clock.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicSync/MusicTimeline.h"
#include "MusicDirector.generated.h"

class UAudioComponent;
class USoundBase;
class USoundWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicCue, const FMusicCue&, Cue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMusicTimelineReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMusicTimelineFinished);

UCLASS()
class FIRSTUE5_API AMusicDirector : public AActor
{
	GENERATED_BODY()

public:
	AMusicDirector();

	/** (Re)start the timeline from the beginning (or from DebugStartAtSeconds). Re-reads the CSV. */
	UFUNCTION(BlueprintCallable, Category = "Music Sync")
	void StartTimeline();

	/** Stop the timeline and the song. Level objects return to their starting state. */
	UFUNCTION(BlueprintCallable, Category = "Music Sync")
	void StopTimeline();

	UFUNCTION(BlueprintPure, Category = "Music Sync")
	bool IsPlaying() const { return bPlaying; }

	/** Current position in the song, in seconds. */
	UFUNCTION(BlueprintPure, Category = "Music Sync")
	float GetSongTime() const { return SongTime; }

	/** The most recent Section cue (the part of the song playing now). False if none yet. */
	UFUNCTION(BlueprintPure, Category = "Music Sync")
	bool GetCurrentSection(FMusicCue& OutSection) const;

	/** Fired for every cue as the song reaches it. */
	UPROPERTY(BlueprintAssignable, Category = "Music Sync")
	FOnMusicCue OnCue;

	/** Fired when the timeline starts or stops, so reactors can return to their starting state. */
	UPROPERTY(BlueprintAssignable, Category = "Music Sync")
	FOnMusicTimelineReset OnTimelineReset;

	/** Fired once the End cue (or the last cue) has been reached. */
	UPROPERTY(BlueprintAssignable, Category = "Music Sync")
	FOnMusicTimelineFinished OnFinished;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	/** Timeline file in the project's MusicTimelines/ folder. */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	FString TimelineFile = TEXT("HouseOfMirrors.csv");

	/**
	 * The song. Licensed music lives in Content/Licensed/ (never committed). If this is empty or the
	 * asset is missing, the timeline still runs, silently.
	 */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	TSoftObjectPtr<USoundBase> Song;

	/** Start as soon as the level begins (normally the castle trigger starts it instead). */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	bool bStartOnBeginPlay = false;

	/**
	 * Authoring aid: start part-way into the song (seconds), e.g. 110 to jump to the transformation.
	 * Cues before this point are skipped.
	 */
	UPROPERTY(EditAnywhere, Category = "Music Sync|Debug", meta = (ClampMin = "0.0"))
	float DebugStartAtSeconds = 0.f;

	/** Show the song clock, current section and each cue on screen (development builds only). */
	UPROPERTY(EditAnywhere, Category = "Music Sync|Debug")
	bool bShowDebugDisplay = true;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAudioComponent> Audio;

private:
	UFUNCTION()
	void HandlePlaybackPercent(const USoundWave* PlayingSoundWave, const float PlaybackPercent);

	void FireCue(int32 Index);
	void ShowDebugMessage(const FString& Message, const FColor& Color, float Seconds) const;
	void DrawDebugStatus() const;

	TArray<FMusicCue> Cues;
	int32 NextCueIndex = 0;
	int32 CurrentSectionIndex = INDEX_NONE;
	float SongTime = 0.f;
	float EndTime = 0.f;
	bool bPlaying = false;
	bool bHasSong = false;
};
