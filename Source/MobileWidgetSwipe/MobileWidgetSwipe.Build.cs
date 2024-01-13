// Some copyright should be here...

using UnrealBuildTool;

public class MobileWidgetSwipe : ModuleRules
{
	public MobileWidgetSwipe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Slate",
				"SlateCore",
				"UMG", 
				
				
				
				// "DeveloperSettings",
				// "Engine",
				"InputCore",
				// "Slate",
				// "SlateCore",
				// "RenderCore",
				// "RHI",
				// "ApplicationCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		// PrivateIncludePathModuleNames.AddRange(
		// 	new string[] {
		// 		"SlateRHIRenderer",
		// 		"ImageWrapper",
		// 		"TargetPlatform",
		// 	}
		// );
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		// Add slate runtime dependencies
		if (Target.bUsesSlate)
		{
			RuntimeDependencies.Add("$(EngineDir)/Content/Slate/...*.ttf", StagedFileType.UFS);
			RuntimeDependencies.Add("$(EngineDir)/Content/Slate/...*.png", StagedFileType.UFS);
			RuntimeDependencies.Add("$(EngineDir)/Content/Slate/...*.svg", StagedFileType.UFS);
			RuntimeDependencies.Add("$(EngineDir)/Content/Slate/...*.tps", StagedFileType.UFS);
			RuntimeDependencies.Add("$(EngineDir)/Content/SlateDebug/...", StagedFileType.DebugNonUFS);

			if (Target.Platform == UnrealTargetPlatform.Win64)
			{ 
				RuntimeDependencies.Add("$(EngineDir)/Content/Slate/...*.cur", StagedFileType.NonUFS);
			}

			if (Target.ProjectFile != null)
			{
				RuntimeDependencies.Add("$(ProjectDir)/Content/Slate/...", StagedFileType.UFS);
				RuntimeDependencies.Add("$(ProjectDir)/Content/SlateDebug/...", StagedFileType.DebugNonUFS);
			}
		}
	}
}
