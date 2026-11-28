#include "PerspectiveSwitchTrigger.h"

#include "Components/BoxComponent.h"
#include "EngineUtils.h"
#include "FirstUE5Character.h"
#include "MusicSync/MusicDirector.h"

APerspectiveSwitchTrigger::APerspectiveSwitchTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	// Deep in Y so the player can't miss it; tall enough to catch a jumping player.
	Box->SetBoxExtent(FVector(100.f, 600.f, 400.f));
	Box->SetCollisionProfileName(TEXT("Trigger"));
	Box->ShapeColor = FColor(160, 32, 240); // purple, visible in the editor only
	Box->SetHiddenInGame(true);
}

void APerspectiveSwitchTrigger::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &APerspectiveSwitchTrigger::OnBoxBeginOverlap);
}

void APerspectiveSwitchTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirstUE5Character* Character = Cast<AFirstUE5Character>(OtherActor);
	if (!Character || Character->GetPerspective() != ECameraPerspective::SideScroll)
	{
		return;
	}

	Character->StartFirstPersonTransition();

	if (bStartMusicTimeline)
	{
		if (!MusicDirector)
		{
			for (TActorIterator<AMusicDirector> It(GetWorld()); It; ++It)
			{
				MusicDirector = *It;
				break;
			}
		}
		if (MusicDirector)
		{
			MusicDirector->StartTimeline();
		}
	}
}
