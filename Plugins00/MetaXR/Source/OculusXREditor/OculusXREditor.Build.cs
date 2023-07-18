// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OculusXREditor : ModuleRules
{
	public OculusXREditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"EngineSettings",
				"AndroidRuntimeSettings",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"Core",
				"OculusXRHMD",
				"OculusXRMovement",
				"OculusXRPassthrough",
				"OVRPluginXR",
				"HTTP",
				"DesktopPlatform",
				"LauncherServices",
				"GameProjectGeneration",
				"SharedSettingsWidgets",
			}
		);

		PrivateIncludePaths.AddRange(
				new string[] {
					// Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
					"OculusXREditor/Private",
					"OculusXRHMD/Private",
				});

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"Settings",
			}
			);
	}
}
