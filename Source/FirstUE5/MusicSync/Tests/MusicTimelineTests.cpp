// Automation tests for the music timeline parser.
// Run headless (see docs/SETUP.md) or in the editor: Tools -> Session Frontend -> Automation,
// filter "FirstUE5".

#include "Misc/AutomationTest.h"
#include "MusicSync/MusicTimeline.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
constexpr EAutomationTestFlags MusicTestFlags =
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMusicTimelineParseTimeTest, "FirstUE5.MusicSync.ParseTime", MusicTestFlags)
bool FMusicTimelineParseTimeTest::RunTest(const FString& Parameters)
{
	float Seconds = -1.f;
	TestTrue(TEXT("m:ss parses"), MusicTimeline::ParseTime(TEXT("1:07"), Seconds));
	TestEqual(TEXT("1:07 = 67 s"), Seconds, 67.f);

	TestTrue(TEXT("m:ss.s parses"), MusicTimeline::ParseTime(TEXT("3:33.5"), Seconds));
	TestEqual(TEXT("3:33.5 = 213.5 s"), Seconds, 213.5f);

	TestTrue(TEXT("plain seconds parse"), MusicTimeline::ParseTime(TEXT("21"), Seconds));
	TestEqual(TEXT("21 = 21 s"), Seconds, 21.f);

	TestTrue(TEXT("surrounding spaces are ignored"), MusicTimeline::ParseTime(TEXT(" 0:08 "), Seconds));
	TestEqual(TEXT("0:08 = 8 s"), Seconds, 8.f);

	TestFalse(TEXT("empty is rejected"), MusicTimeline::ParseTime(TEXT(""), Seconds));
	TestFalse(TEXT("text is rejected"), MusicTimeline::ParseTime(TEXT("soon"), Seconds));
	TestFalse(TEXT("60+ seconds after a colon is rejected"), MusicTimeline::ParseTime(TEXT("1:75"), Seconds));
	TestFalse(TEXT("negative is rejected"), MusicTimeline::ParseTime(TEXT("-5"), Seconds));
	TestFalse(TEXT("two colons are rejected"), MusicTimeline::ParseTime(TEXT("1:02:03"), Seconds));

	TestEqual(TEXT("FormatTime"), MusicTimeline::FormatTime(67.9f), FString(TEXT("1:07")));
	TestEqual(TEXT("FormatTime zero"), MusicTimeline::FormatTime(0.f), FString(TEXT("0:00")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMusicTimelineSplitCsvTest, "FirstUE5.MusicSync.SplitCsvLine", MusicTestFlags)
bool FMusicTimelineSplitCsvTest::RunTest(const FString& Parameters)
{
	const TArray<FString> Simple = MusicTimeline::SplitCsvLine(TEXT("0:21,Event,Wave1,Demons pour out,"));
	TestEqual(TEXT("trailing empty field counts"), Simple.Num(), 5);
	TestEqual(TEXT("field 3"), Simple[2], FString(TEXT("Wave1")));
	TestEqual(TEXT("empty last field"), Simple[4], FString());

	const TArray<FString> Quoted =
		MusicTimeline::SplitCsvLine(TEXT("1:06,Lyric,WelcomeHome1,\"\"\"Welcome home\"\", first time\",Peak"));
	TestEqual(TEXT("quoted field count"), Quoted.Num(), 5);
	TestEqual(TEXT("quotes and comma kept"), Quoted[3], FString(TEXT("\"Welcome home\", first time")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMusicTimelineParseTest, "FirstUE5.MusicSync.Parse", MusicTestFlags)
bool FMusicTimelineParseTest::RunTest(const FString& Parameters)
{
	const FString Csv =
		TEXT("# comment line\n") TEXT("Kind,Time,Id,Description\n") // columns in a different order, no Intensity
		TEXT("Event,0:21,Wave1,First wave\n") TEXT("\n")
			TEXT("section,0:11,SynthStart,Imposing synth\n") // lower-case kind is fine
		TEXT("Evnt,0:30,Typo,Bad kind\n") TEXT("Event,1:75,BadTime,Bad time\n") TEXT("Event,0:40,Has Space,Bad id\n")
			TEXT("End,3:40,SongEnd,Song ends\n");

	const FMusicTimelineParseResult Result = MusicTimeline::Parse(Csv);

	TestEqual(TEXT("valid cues kept"), Result.Cues.Num(), 3);
	TestEqual(TEXT("three bad rows reported"), Result.Errors.Num(), 3);
	if (Result.Cues.Num() == 3)
	{
		TestTrue(TEXT("sorted by time: first"), Result.Cues[0].Id == FName(TEXT("SynthStart")));
		TestTrue(TEXT("section kind"), Result.Cues[0].Kind == EMusicCueKind::Section);
		TestTrue(TEXT("sorted by time: second"), Result.Cues[1].Id == FName(TEXT("Wave1")));
		TestEqual(TEXT("source line recorded"), Result.Cues[1].SourceLine, 3);
		TestTrue(TEXT("end kind"), Result.Cues[2].Kind == EMusicCueKind::End);
	}

	const FMusicTimelineParseResult NoHeader = MusicTimeline::Parse(TEXT("Foo,Bar\n0:01,Event,X\n"));
	TestTrue(TEXT("missing required columns is an error"), NoHeader.HasErrors());
	TestEqual(TEXT("no cues without a valid header"), NoHeader.Cues.Num(), 0);

	TestTrue(TEXT("empty file is an error"), MusicTimeline::Parse(TEXT("")).HasErrors());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMusicTimelineProjectFilesTest, "FirstUE5.MusicSync.ProjectTimelinesAreValid", MusicTestFlags)
bool FMusicTimelineProjectFilesTest::RunTest(const FString& Parameters)
{
	const FMusicTimelineParseResult Result =
		MusicTimeline::LoadFile(MusicTimeline::GetTimelinePath(TEXT("HouseOfMirrors.csv")));
	for (const FString& Error : Result.Errors)
	{
		AddError(Error);
	}
	TestTrue(TEXT("HouseOfMirrors.csv has cues"), Result.Cues.Num() > 0);
	TestTrue(TEXT("HouseOfMirrors.csv ends with an End cue"),
		Result.Cues.Num() > 0 && Result.Cues.Last().Kind == EMusicCueKind::End);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
