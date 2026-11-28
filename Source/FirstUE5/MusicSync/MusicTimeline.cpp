#include "MusicSync/MusicTimeline.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace MusicTimeline
{

bool ParseTime(const FString& Text, float& OutSeconds)
{
	const FString Trimmed = Text.TrimStartAndEnd();
	if (Trimmed.IsEmpty())
	{
		return false;
	}

	FString MinutesPart;
	FString SecondsPart;
	const bool bHasMinutes = Trimmed.Split(TEXT(":"), &MinutesPart, &SecondsPart);
	if (!bHasMinutes)
	{
		SecondsPart = Trimmed;
	}

	if (!SecondsPart.IsNumeric() || (bHasMinutes && !MinutesPart.IsNumeric()))
	{
		return false;
	}
	// IsNumeric accepts a leading '-', which a time can't have.
	if (SecondsPart.StartsWith(TEXT("-")) || MinutesPart.StartsWith(TEXT("-")) || MinutesPart.Contains(TEXT(".")))
	{
		return false;
	}

	const float Seconds = FCString::Atof(*SecondsPart);
	if (bHasMinutes && Seconds >= 60.f)
	{
		return false;
	}

	OutSeconds = (bHasMinutes ? FCString::Atoi(*MinutesPart) * 60.f : 0.f) + Seconds;
	return true;
}

FString FormatTime(float Seconds)
{
	const int32 Total = FMath::Max(0, FMath::FloorToInt(Seconds));
	return FString::Printf(TEXT("%d:%02d"), Total / 60, Total % 60);
}

bool ParseKind(const FString& Text, EMusicCueKind& OutKind)
{
	static const TPair<const TCHAR*, EMusicCueKind> Kinds[] = {
		{TEXT("Section"), EMusicCueKind::Section},
		{TEXT("Lyric"), EMusicCueKind::Lyric},
		{TEXT("Event"), EMusicCueKind::Event},
		{TEXT("Checkpoint"), EMusicCueKind::Checkpoint},
		{TEXT("End"), EMusicCueKind::End},
	};
	const FString Trimmed = Text.TrimStartAndEnd();
	for (const TPair<const TCHAR*, EMusicCueKind>& Kind : Kinds)
	{
		if (Trimmed.Equals(Kind.Key, ESearchCase::IgnoreCase))
		{
			OutKind = Kind.Value;
			return true;
		}
	}
	return false;
}

TArray<FString> SplitCsvLine(const FString& Line)
{
	TArray<FString> Fields;
	FString Current;
	bool bInQuotes = false;

	for (int32 i = 0; i < Line.Len(); ++i)
	{
		const TCHAR C = Line[i];
		if (bInQuotes)
		{
			if (C == TEXT('"'))
			{
				if (i + 1 < Line.Len() && Line[i + 1] == TEXT('"'))
				{
					Current.AppendChar(TEXT('"'));
					++i;
				}
				else
				{
					bInQuotes = false;
				}
			}
			else
			{
				Current.AppendChar(C);
			}
		}
		else if (C == TEXT('"'))
		{
			bInQuotes = true;
		}
		else if (C == TEXT(','))
		{
			Fields.Add(Current.TrimStartAndEnd());
			Current.Reset();
		}
		else
		{
			Current.AppendChar(C);
		}
	}
	Fields.Add(Current.TrimStartAndEnd());
	return Fields;
}

FMusicTimelineParseResult Parse(const FString& CsvText)
{
	FMusicTimelineParseResult Result;

	TArray<FString> Lines;
	CsvText.ParseIntoArrayLines(Lines, /*bCullEmpty*/ false);

	// Column positions, found from the header row.
	int32 TimeCol = INDEX_NONE;
	int32 KindCol = INDEX_NONE;
	int32 IdCol = INDEX_NONE;
	int32 DescriptionCol = INDEX_NONE;
	int32 IntensityCol = INDEX_NONE;
	bool bHaveHeader = false;

	for (int32 Index = 0; Index < Lines.Num(); ++Index)
	{
		const int32 LineNumber = Index + 1;
		FString Line = Lines[Index].TrimStartAndEnd();
		if (Index == 0)
		{
			Line.RemoveFromStart(TEXT("\xFEFF")); // UTF-8 byte order mark saved by some editors (e.g. Excel)
		}
		if (Line.IsEmpty() || Line.StartsWith(TEXT("#")))
		{
			continue;
		}

		const TArray<FString> Fields = SplitCsvLine(Line);

		if (!bHaveHeader)
		{
			bHaveHeader = true;
			for (int32 Col = 0; Col < Fields.Num(); ++Col)
			{
				const FString& Name = Fields[Col];
				if (Name.Equals(TEXT("Time"), ESearchCase::IgnoreCase))
				{
					TimeCol = Col;
				}
				else if (Name.Equals(TEXT("Kind"), ESearchCase::IgnoreCase))
				{
					KindCol = Col;
				}
				else if (Name.Equals(TEXT("Id"), ESearchCase::IgnoreCase))
				{
					IdCol = Col;
				}
				else if (Name.Equals(TEXT("Description"), ESearchCase::IgnoreCase))
				{
					DescriptionCol = Col;
				}
				else if (Name.Equals(TEXT("Intensity"), ESearchCase::IgnoreCase))
				{
					IntensityCol = Col;
				}
			}
			if (TimeCol == INDEX_NONE || KindCol == INDEX_NONE || IdCol == INDEX_NONE)
			{
				Result.Errors.Add(FString::Printf(
					TEXT("line %d: header must include Time, Kind and Id columns (found: %s)"), LineNumber, *Line));
				return Result;
			}
			continue;
		}

		auto Field = [&Fields](int32 Col)
		{
			return Fields.IsValidIndex(Col) ? Fields[Col] : FString();
		};

		FMusicCue Cue;
		Cue.SourceLine = LineNumber;

		if (!ParseTime(Field(TimeCol), Cue.TimeSeconds))
		{
			Result.Errors.Add(
				FString::Printf(TEXT("line %d: bad Time '%s' (use m:ss, e.g. 1:07)"), LineNumber, *Field(TimeCol)));
			continue;
		}
		if (!ParseKind(Field(KindCol), Cue.Kind))
		{
			Result.Errors.Add(
				FString::Printf(TEXT("line %d: unknown Kind '%s' (use Section, Lyric, Event, Checkpoint or End)"),
					LineNumber, *Field(KindCol)));
			continue;
		}
		const FString Id = Field(IdCol);
		if (Id.IsEmpty() || Id.Contains(TEXT(" ")))
		{
			Result.Errors.Add(
				FString::Printf(TEXT("line %d: Id '%s' must be a single word with no spaces"), LineNumber, *Id));
			continue;
		}

		Cue.Id = FName(*Id);
		Cue.Description = Field(DescriptionCol);
		Cue.Intensity = Field(IntensityCol);
		Result.Cues.Add(MoveTemp(Cue));
	}

	if (!bHaveHeader)
	{
		Result.Errors.Add(TEXT("the file is empty (expected a header row: Time,Kind,Id,Description,Intensity)"));
	}

	Result.Cues.StableSort([](const FMusicCue& A, const FMusicCue& B)
	{
		return A.TimeSeconds < B.TimeSeconds;
	});
	return Result;
}

FString GetTimelinePath(const FString& FileName)
{
	return FPaths::Combine(FPaths::ProjectDir(), TEXT("MusicTimelines"), FileName);
}

FMusicTimelineParseResult LoadFile(const FString& FilePath)
{
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *FilePath))
	{
		FMusicTimelineParseResult Result;
		Result.Errors.Add(FString::Printf(TEXT("could not read timeline file '%s'"), *FilePath));
		return Result;
	}
	return Parse(Text);
}

} // namespace MusicTimeline
