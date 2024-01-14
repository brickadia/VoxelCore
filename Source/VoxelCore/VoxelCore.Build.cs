// Copyright Voxel Plugin, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelCore : ModuleRules
{
	public VoxelCore(ReadOnlyTargetRules Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		CppStandard = CppStandardVersion.Cpp17;
		IWYUSupport = IWYUSupport.None;
		bUseUnity = false;
		PrivatePCHHeaderFile = "Public/VoxelMinimal.h";

		if (Target.Configuration == UnrealTargetConfiguration.Debug ||
			Target.Configuration == UnrealTargetConfiguration.DebugGame)
		{
			PublicDefinitions.Add("VOXEL_DEBUG=1");
		}
		else
		{
			PublicDefinitions.Add("VOXEL_DEBUG=0");
		}

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"RHI",
				"Engine",
				"TraceLog",
				"Renderer",
				"Projects",
				"InputCore",
				"RenderCore",
				"ApplicationCore",
				"DeveloperSettings",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"zlib",
				"UElibPNG",
				"HTTP",
				"Slate",
				"SlateCore",
			}
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"UATHelper",
				}
			);
		}
	}
}