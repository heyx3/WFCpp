// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class WfcppDemoTarget : TargetRules
{
	public WfcppDemoTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		CppStandard = CppStandardVersion.Cpp17;
		ExtraModuleNames.AddRange( new string[] { "WfcppDemo", "WfcppRuntime" } );
	}
}
