#include "MusicSync/MusicDirector.h"

#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"

DEFINE_LOG_CATEGORY_STATIC(LogMusicSync, Log, All);

namespace
{
// Fixed key so the status line overwrites itself instead of stacking up.
constexpr uint64 StatusMessageKey = 0x4D5553494353594EULL; // "MUSICSYN"

// If the audio clock and the game clock disagree by more than this, trust the audio.
constexpr float AudioResyncThreshold = 0.05f;

FColor ColorForKind(EMusicCueKind Kind)
{
	switch (Kind)
	{
	case EMusicCueKind::Section:
		return FColor(120, 220, 120);
	case EMusicCueKind::Lyric:
		return FColor(110, 200, 255);
	case EMusicCueKind::Checkpoint:
		return FColor(255, 210, 80);
	case EMusicCueKind::End:
		return FColor(200, 120, 255);
	case EMusicCueKind::Event:
	default:
		return FColor::White;
	}
}

const TCHAR* KindLabel(EMusicCueKind Kind)
{
	switch (Kind)
	{
	case EMusicCueKind::Section:
		return TEXT("Section");
	case EMusicCueKind::Lyric:
		return TEXT("Lyric");
	case EMusicCueKind::Checkpoint:
		return TEXT("Checkpoint");
	case EMusicCueKind::End:
		return TEXT("End");
	case EMusicCueKind::Event:
	default:
		return TEXT("Event");
	}
}
} // namespace

AMusicDirector::AMusicDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(RootComponent);
	Audio->bAutoActivate = false;
	Audio->bAllowSpatialization = false; // the song plays "in the player's head", not from a point
}

void AMusicDirector::BeginPlay()
{
	Super::BeginPlay();

	Audio->OnAudioPlaybackPercent.AddDynamic(this, &AMusicDirector::HandlePlaybackPercent);

	if (bStartOnBeginPlay)
	{
		StartTimeline();
	}
}

void AMusicDirector::StartTimeline()
{
	StopTimeline();

	// Re-read the file every time, so edits to the CSV show up on the next play without rebuilding.
	const FString Path = MusicTimeline::GetTimelinePath(TimelineFile);
	FMusicTimelineParseResult Parsed = MusicTimeline::LoadFile(Path);
	for (const FString& Error : Parsed.Errors)
	{
		UE_LOG(LogMusicSync, Error, TEXT("%s: %s"), *TimelineFile, *Error);
		ShowDebugMessage(FString::Printf(TEXT("Music timeline %s: %s"), *TimelineFile, *Error), FColor::Red, 15.f);
	}

	Cues = MoveTemp(Parsed.Cues);
	EndTime = 0.f;
	for (const FMusicCue& Cue : Cues)
	{
		if (Cue.Kind == EMusicCueKind::End)
		{
			EndTime = Cue.TimeSeconds;
			break;
		}
	}
	if (EndTime <= 0.f && Cues.Num() > 0)
	{
		EndTime = Cues.Last().TimeSeconds;
	}

	// Jump to the debug start point: skip earlier cues, but remember which section we're in.
	SongTime = DebugStartAtSeconds;
	NextCueIndex = 0;
	CurrentSectionIndex = INDEX_NONE;
	while (NextCueIndex < Cues.Num() && Cues[NextCueIndex].TimeSeconds < SongTime)
	{
		if (Cues[NextCueIndex].Kind == EMusicCueKind::Section)
		{
			CurrentSectionIndex = NextCueIndex;
		}
		++NextCueIndex;
	}

	USoundBase* SongAsset = Song.IsNull() ? nullptr : Song.LoadSynchronous();
	bHasSong = SongAsset != nullptr;
	if (bHasSong)
	{
		Audio->SetSound(SongAsset);
		Audio->Play(SongTime);
	}
	else
	{
		UE_LOG(LogMusicSync, Log, TEXT("%s: no song asset found; running the timeline silently."), *TimelineFile);
		ShowDebugMessage(TEXT("Music timeline: no song found, running silently"), FColor(180, 180, 180), 5.f);
	}

	bPlaying = Cues.Num() > 0;
	UE_LOG(LogMusicSync, Log, TEXT("%s: started at %s with %d cues (ends %s)."), *TimelineFile,
		*MusicTimeline::FormatTime(SongTime), Cues.Num(), *MusicTimeline::FormatTime(EndTime));
}

void AMusicDirector::StopTimeline()
{
	bPlaying = false;
	if (Audio && Audio->IsPlaying())
	{
		Audio->Stop();
	}
	NextCueIndex = 0;
	CurrentSectionIndex = INDEX_NONE;
	SongTime = 0.f;

	OnTimelineReset.Broadcast();
}

bool AMusicDirector::GetCurrentSection(FMusicCue& OutSection) const
{
	if (Cues.IsValidIndex(CurrentSectionIndex))
	{
		OutSection = Cues[CurrentSectionIndex];
		return true;
	}
	return false;
}

void AMusicDirector::HandlePlaybackPercent(const USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	if (!bPlaying || !PlayingSoundWave)
	{
		return;
	}
	const float AudioTime = PlaybackPercent * PlayingSoundWave->Duration;
	if (FMath::Abs(AudioTime - SongTime) > AudioResyncThreshold)
	{
		SongTime = AudioTime;
	}
}

void AMusicDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bPlaying)
	{
		return;
	}

	// The game clock keeps time between audio updates (and is the only clock with no song).
	SongTime += DeltaSeconds;

	while (bPlaying && NextCueIndex < Cues.Num() && Cues[NextCueIndex].TimeSeconds <= SongTime)
	{
		FireCue(NextCueIndex++);
	}

	if (bPlaying && NextCueIndex >= Cues.Num() && SongTime >= EndTime)
	{
		bPlaying = false;
		UE_LOG(LogMusicSync, Log, TEXT("%s: finished."), *TimelineFile);
		OnFinished.Broadcast();
	}

	DrawDebugStatus();
}

void AMusicDirector::FireCue(int32 Index)
{
	// Copy: a listener may restart the timeline (which replaces Cues) while we broadcast.
	const FMusicCue Cue = Cues[Index];
	if (Cue.Kind == EMusicCueKind::Section)
	{
		CurrentSectionIndex = Index;
	}

	FString Line = FString::Printf(
		TEXT("[%s] %s: %s"), *MusicTimeline::FormatTime(Cue.TimeSeconds), KindLabel(Cue.Kind), *Cue.Id.ToString());
	if (!Cue.Description.IsEmpty())
	{
		Line += FString::Printf(TEXT("  %s"), *Cue.Description);
	}
	if (!Cue.Intensity.IsEmpty())
	{
		Line += FString::Printf(TEXT("  (%s)"), *Cue.Intensity);
	}
	UE_LOG(LogMusicSync, Log, TEXT("%s"), *Line);
	ShowDebugMessage(Line, ColorForKind(Cue.Kind), 6.f);

	OnCue.Broadcast(Cue);
}

void AMusicDirector::ShowDebugMessage(const FString& Message, const FColor& Color, float Seconds) const
{
#if !UE_BUILD_SHIPPING
	if (bShowDebugDisplay && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Seconds, Color, Message);
	}
#endif
}

void AMusicDirector::DrawDebugStatus() const
{
#if !UE_BUILD_SHIPPING
	if (!bShowDebugDisplay || !GEngine)
	{
		return;
	}

	FString Status = FString::Printf(TEXT("MUSIC %s / %s%s"), *MusicTimeline::FormatTime(SongTime),
		*MusicTimeline::FormatTime(EndTime), bHasSong ? TEXT("") : TEXT(" (silent)"));

	FMusicCue Section;
	if (GetCurrentSection(Section))
	{
		Status += FString::Printf(TEXT("   Section: %s"), *Section.Id.ToString());
		if (!Section.Intensity.IsEmpty())
		{
			Status += FString::Printf(TEXT(" (%s)"), *Section.Intensity);
		}
	}
	if (Cues.IsValidIndex(NextCueIndex))
	{
		const FMusicCue& Next = Cues[NextCueIndex];
		Status +=
			FString::Printf(TEXT("   Next: %s %s"), *MusicTimeline::FormatTime(Next.TimeSeconds), *Next.Id.ToString());
	}

	GEngine->AddOnScreenDebugMessage(StatusMessageKey, 0.5f, FColor(200, 160, 255), Status);
#endif
}
