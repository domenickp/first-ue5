#include "MusicSync/MusicCueReactor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "MusicSync/MusicDirector.h"
#include "UObject/ConstructorHelpers.h"

AMusicCueReactor::AMusicCueReactor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // only ticks while rising

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(Mesh);
	Light->SetMobility(EComponentMobility::Movable);
	Light->SetVisibility(false);
	Light->SetAttenuationRadius(1500.f);
}

void AMusicCueReactor::BeginPlay()
{
	Super::BeginPlay();

	PlacedLocation = GetActorLocation();
	Light->SetLightColor(LightColor);
	Light->SetIntensity(LightIntensity);

	if (!Director)
	{
		for (TActorIterator<AMusicDirector> It(GetWorld()); It; ++It)
		{
			Director = *It;
			break;
		}
	}
	if (Director)
	{
		Director->OnCue.AddDynamic(this, &AMusicCueReactor::HandleCue);
		Director->OnTimelineReset.AddDynamic(this, &AMusicCueReactor::ResetToStart);
	}

	ResetToStart();
}

void AMusicCueReactor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Director)
	{
		Director->OnCue.RemoveDynamic(this, &AMusicCueReactor::HandleCue);
		Director->OnTimelineReset.RemoveDynamic(this, &AMusicCueReactor::ResetToStart);
	}
	Super::EndPlay(EndPlayReason);
}

bool AMusicCueReactor::HasReaction(EMusicCueReaction Reaction) const
{
	return Reactions.ContainsByPredicate([Reaction](const FMusicCueReactionEntry& Entry)
	{
		return Entry.Reaction == Reaction;
	});
}

void AMusicCueReactor::ResetToStart()
{
	bRising = false;
	RiseElapsed = 0.f;
	SetActorTickEnabled(false);

	const bool bRises = HasReaction(EMusicCueReaction::Rise);
	SetActorLocation(bRises ? PlacedLocation - FVector(0.f, 0.f, RiseHeight) : PlacedLocation);
	SetShown(!(HasReaction(EMusicCueReaction::Show) || bRises));
	Light->SetVisibility(false);
}

void AMusicCueReactor::SetShown(bool bShown)
{
	// Hidden stand-ins shouldn't block the player like invisible walls.
	SetActorHiddenInGame(!bShown);
	SetActorEnableCollision(bShown);
}

void AMusicCueReactor::HandleCue(const FMusicCue& Cue)
{
	for (const FMusicCueReactionEntry& Entry : Reactions)
	{
		if (Entry.CueId == Cue.Id)
		{
			Apply(Entry.Reaction);
		}
	}
}

void AMusicCueReactor::Apply(EMusicCueReaction Reaction)
{
	switch (Reaction)
	{
	case EMusicCueReaction::Show:
		SetShown(true);
		break;
	case EMusicCueReaction::Hide:
		SetShown(false);
		Light->SetVisibility(false);
		break;
	case EMusicCueReaction::LightOn:
		Light->SetVisibility(true);
		break;
	case EMusicCueReaction::LightOff:
		Light->SetVisibility(false);
		break;
	case EMusicCueReaction::Rise:
		SetShown(true);
		bRising = true;
		RiseElapsed = 0.f;
		SetActorTickEnabled(true);
		break;
	}
}

void AMusicCueReactor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bRising)
	{
		SetActorTickEnabled(false);
		return;
	}

	RiseElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(RiseElapsed / RiseDuration, 0.f, 1.f);
	const float Eased = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.f);
	SetActorLocation(PlacedLocation - FVector(0.f, 0.f, RiseHeight * (1.f - Eased)));

	if (Alpha >= 1.f)
	{
		bRising = false;
		SetActorTickEnabled(false);
	}
}
