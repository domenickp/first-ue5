// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FirstUE5 : ModuleRules
{
	public FirstUE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Lets code include headers in subfolders by path from the module root, e.g. "MusicSync/MusicDirector.h".
		PublicIncludePaths.Add(ModuleDirectory);
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Music timelines (MusicTimelines/*.csv) are read at runtime, so ship them with packaged builds.
		RuntimeDependencies.Add("$(ProjectDir)/MusicTimelines/*.csv", StagedFileType.UFS);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
