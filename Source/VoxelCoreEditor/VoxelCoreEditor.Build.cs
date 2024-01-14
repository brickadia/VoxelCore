// Copyright Voxel Plugin, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VoxelCoreEditor : ModuleRules
{
    public VoxelCoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		CppStandard = CppStandardVersion.Cpp17;
		IWYUSupport = IWYUSupport.None;
		bUseUnity = false;
		PrivatePCHHeaderFile = "Public/VoxelEditorMinimal.h";

		// For FDetailCategoryBuilderImpl
		PrivateIncludePaths.Add(Path.Combine(EngineDirectory, "Source/Editor/PropertyEditor/Private"));

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
				"VoxelCore",
				"Core",
				"CoreUObject",
				"RHI",
				"Engine",
				"InputCore",
				"RenderCore",
				"DeveloperSettings",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"PropertyEditor",
                "EditorFramework",
                "AdvancedPreviewScene",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "PlacementMode",
                "MessageLog",
                "WorkspaceMenuStructure",
                "DetailCustomizations",
                "BlueprintGraph",
                "Landscape",
                "ToolMenus",
                "SceneOutliner",
            });
    }
}