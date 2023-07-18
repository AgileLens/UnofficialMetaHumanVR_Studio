// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OculusXRPassthrough : ModuleRules
	{
		public OculusXRPassthrough(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"OculusXRHMD",
					"OVRPluginXR",
					"HeadMountedDisplay",
				});

			PublicIncludePaths.AddRange(new string[] {
				"Runtime/Engine/Classes/Components",
				"Runtime/Engine/Classes/Kismet",
			});

			PrivateIncludePaths.AddRange(new string[] {
                // Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
                "OculusXRHMD/Private",
			});
		}
	}
}
