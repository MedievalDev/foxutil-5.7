// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class FoxUtils : ModuleRules
{
	public FoxUtils(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Foliage"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"RHI"
		});

		// Optional FoxCore integration
		string FoxCorePath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "..", "FoxCore"));
		if (Directory.Exists(FoxCorePath))
		{
			PrivateDependencyModuleNames.Add("FoxCore");
			PublicDefinitions.Add("FOX_WITH_FOXCORE=1");
		}
		else
		{
			PublicDefinitions.Add("FOX_WITH_FOXCORE=0");
		}
	}
}
