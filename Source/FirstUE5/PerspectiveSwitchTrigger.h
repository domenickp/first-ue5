// An invisible box that swings the player from side view into first person when they walk into it.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerspectiveSwitchTrigger.generated.h"

class AMusicDirector;
class UBoxComponent;

UCLASS()
class FIRSTUE5_API APerspectiveSwitchTrigger : public AActor
{
	GENERATED_BODY()

public:
	APerspectiveSwitchTrigger();

protected:
	virtual void BeginPlay() override;

	/** The trigger volume. Resize it in the editor with the Box's "Box Extent" property. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> Box;

	/** Start a music-driven scene when the swing begins (the song's 0:00 is entering the yard). */
	UPROPERTY(EditAnywhere, Category = "Music Sync")
	bool bStartMusicTimeline = true;

	/** The director to start. Leave empty to use the first Music Director in the level. */
	UPROPERTY(EditAnywhere, Category = "Music Sync", meta = (EditCondition = "bStartMusicTimeline"))
	TObjectPtr<AMusicDirector> MusicDirector;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
