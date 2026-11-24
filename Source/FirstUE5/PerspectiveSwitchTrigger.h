// An invisible box that swings the player from side view into first person when they walk into it.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerspectiveSwitchTrigger.generated.h"

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

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
