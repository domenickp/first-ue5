// A grey-box stand-in that reacts to music cues: appear, disappear, light up or rise out of the
// ground when the song reaches a cue Id. Used to block out a music-driven scene before real art,
// effects and enemies exist.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicSync/MusicTimeline.h"
#include "MusicCueReactor.generated.h"

class AMusicDirector;
class UPointLightComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EMusicCueReaction : uint8
{
	/** Become visible. (The object starts hidden if it has any Show reaction.) */
	Show,
	/** Disappear. */
	Hide,
	/** Turn the light on. */
	LightOn,
	/** Turn the light off. */
	LightOff,
	/** Rise up out of the ground. (The object starts RiseHeight below where you placed it.) */
	Rise
};

USTRUCT(BlueprintType)
struct FIRSTUE5_API FMusicCueReactionEntry
{
	GENERATED_BODY()

	/** The Id from the timeline CSV, e.g. WindowsGlow. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sync")
	FName CueId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Sync")
	EMusicCueReaction Reaction = EMusicCueReaction::Show;
};

UCLASS()
class FIRSTUE5_API AMusicCueReactor : public AActor
{
	GENERATED_BODY()

public:
	AMusicCueReactor();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** What to do at which cue. Add one row per reaction. */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	TArray<FMusicCueReactionEntry> Reactions;

	/** The director to listen to. Leave empty to use the first Music Director in the level. */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	TObjectPtr<AMusicDirector> Director;

	UPROPERTY(EditAnywhere, Category = "Music Sync|Rise", meta = (ClampMin = "0.0"))
	float RiseHeight = 400.f;

	UPROPERTY(EditAnywhere, Category = "Music Sync|Rise", meta = (ClampMin = "0.1"))
	float RiseDuration = 3.f;

	UPROPERTY(EditAnywhere, Category = "Music Sync|Light")
	FLinearColor LightColor = FLinearColor(1.f, 0.05f, 0.02f);

	UPROPERTY(EditAnywhere, Category = "Music Sync|Light", meta = (ClampMin = "0.0"))
	float LightIntensity = 50000.f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPointLightComponent> Light;

private:
	UFUNCTION()
	void HandleCue(const FMusicCue& Cue);

	UFUNCTION()
	void ResetToStart();

	void Apply(EMusicCueReaction Reaction);
	void SetShown(bool bShown);
	bool HasReaction(EMusicCueReaction Reaction) const;

	FVector PlacedLocation = FVector::ZeroVector;
	bool bRising = false;
	float RiseElapsed = 0.f;
};
