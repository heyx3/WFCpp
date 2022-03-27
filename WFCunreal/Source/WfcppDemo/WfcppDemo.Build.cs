// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WfcppDemo : ModuleRules
{
	public WfcppDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PrivateIncludePaths.AddRange(new string[] {
			"../../WFC++",
			"../../WFC++/Tiled3D"
		});
		PrivateDefinitions.Add("WFC_STATIC=1");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
