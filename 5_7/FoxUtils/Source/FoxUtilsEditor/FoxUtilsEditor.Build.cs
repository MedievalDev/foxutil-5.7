// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.

using UnrealBuildTool;

public class FoxUtilsEditor : ModuleRules
{
	public FoxUtilsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"FoxUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"BlueprintGraph",
			"GraphEditor",
			"KismetCompiler",
			"Slate",
			"SlateCore",
			"RHI",
			"AssetRegistry"
		});
	}
}
