// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

using UnrealBuildTool;

public class YetiOSEditor : ModuleRules
{
	public YetiOSEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
				{
					"Core",
					"UnrealEd",
					"ImageWrapper",
					"AssetRegistry",
					"PropertyEditor",
					"EditorStyle",
				}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
				{
					"Engine",
					"Slate",
					"SlateCore",
					"Projects",
					"LevelEditor",
					"InputCore",
					"CoreUObject",
					"ImageWrapper",
					"AssetRegistry",
					"PropertyEditor",
					"AdvancedPreviewScene",
				}
			);
	}
}
