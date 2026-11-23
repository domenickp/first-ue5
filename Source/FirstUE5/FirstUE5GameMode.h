#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FirstUE5GameMode.generated.h"

/** Default game mode: spawns AFirstUE5Character at the level's Player Start. */
UCLASS()
class FIRSTUE5_API AFirstUE5GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFirstUE5GameMode();
};
