using UnrealBuildTool;
public class WfcppRuntime : ModuleRules
{
	public WfcppRuntime(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] {
		});
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject",
			"Engine", "InputCore"
		});
		
		PublicIncludePaths.AddRange(new [] {
			"WfcppRuntime/Public"
		});
		PrivateIncludePaths.AddRange(new [] {
			"WfcppRuntime/Private",
			"../../WFC++",
			"../../WFC++/Tiled3D"
		});
		
		PrivateDefinitions.Add("WFC_STATIC=1");
	}
}