// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class OculusXREyeTracker : ModuleRules
	{
		public OculusXREyeTracker(ReadOnlyTargetRules Target) : base(Target)
		{
			if (Target.Platform == UnrealTargetPlatform.Win64 ||
				Target.Platform == UnrealTargetPlatform.Android)
			{
				PrivateIncludePaths.AddRange(
					new string[] {
						// Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
						"OculusXRHMD/Private",
					});

				PublicDependencyModuleNames.AddRange(
					new string[]
					{
						"InputDevice",
						"EyeTracker",
						"OVRPluginXR",
						"OculusXRHMD",
					}
				);

				PrivateDependencyModuleNames.AddRange(
					new string[]
					{
						"Core",
						"CoreUObject",
						"Engine",
						"InputCore",
					}
				);
			}
		}
	}
}
