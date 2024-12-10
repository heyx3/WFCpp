using UnrealBuildTool;
public class WfcppEditor : ModuleRules
{
	public WfcppEditor(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
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
			"AdvancedPreviewScene",
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
	}
}