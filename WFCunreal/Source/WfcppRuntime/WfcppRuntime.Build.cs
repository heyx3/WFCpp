using System;
using System.Collections.Generic;
using Tools.DotNETCommon;
using UnrealBuildTool;
public class WfcppRuntime : ModuleRules
{
	public WfcppRuntime(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
		PublicDependencyModuleNames.AddRange(new string[] {
		});
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject",
			"Engine", "InputCore"
		});

		//Compile the WFC cpp files as part of this module.
		var wfcDir = DirectoryReference.Combine(
			DirectoryReference.FromString(ModuleDirectory).ParentDirectory.ParentDirectory.ParentDirectory,
			"WFC++"
		);
		foreach (string subFolder in new[] { "Tiled3D", "HelperSrc" })
		{
			var nestedWfcDir = DirectoryReference.Combine(wfcDir, subFolder);
			if (!ConditionalAddModuleDirectory(nestedWfcDir))
				throw new Exception("WFC++ directory not found! Expected at " + nestedWfcDir);
		}
		
		//Enable memory debugging:
		if (new HashSet<UnrealTargetConfiguration>() {
			    UnrealTargetConfiguration.Debug,
			    UnrealTargetConfiguration.DebugGame
		    }.Contains(target.Configuration))
		{
			PublicDefinitions.Add("WFCPP_CHECK_MEMORY=1");
		}
		PublicDefinitions.Add("WFC_API=WFCPPRUNTIME_API");
	}
}