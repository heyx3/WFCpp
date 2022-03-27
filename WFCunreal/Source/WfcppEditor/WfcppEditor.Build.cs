using UnrealBuildTool;
public class WfcppEditor : ModuleRules
{
	public WfcppEditor(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new [] {
			"Core", "CoreUObject",
			"Engine", "InputCore",
			"UnrealEd",
			"WfcppRuntime"
		});
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Json", "Slate", "SlateCore", "EditorStyle", "EditorWidgets",
			"Kismet", "KismetWidgets",
			"PropertyEditor", "WorkspaceMenuStructure", "ContentBrowser",
			"RenderCore",
			"Projects", "AssetRegistry"
		});
		
		PrivateIncludePathModuleNames.AddRange(new [] {
			"Settings", "IntroTutorials",
			"AssetTools", "LevelEditor"
		});
		DynamicallyLoadedModuleNames.AddRange(new[] {
			"AssetTools"
		});
		
		PublicIncludePaths.AddRange(new [] {
			"WfcppEditor/Public",
			"WfcppRuntime/Public"
		});
		PrivateIncludePaths.AddRange(new [] {
			"WfcppEditor/Private",
			"../../WFC++",
			"../../WFC++/Tiled3D"
		});
		
		PrivateDefinitions.Add("WFC_STATIC=1");
	}
}