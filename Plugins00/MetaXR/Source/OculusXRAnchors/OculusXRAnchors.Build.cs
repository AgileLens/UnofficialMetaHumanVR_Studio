// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class OculusXRAnchors : ModuleRules
	{
		public OculusXRAnchors(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"OculusXRHMD",
					"OVRPluginXR",
				});

			PrivateIncludePaths.AddRange(
				new string[] {
					// Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
					"OculusXRHMD/Private",
				});

			PublicIncludePaths.AddRange(
				new string[] {
					"Runtime/Engine/Classes/Components",
				});
		}
	}
}
