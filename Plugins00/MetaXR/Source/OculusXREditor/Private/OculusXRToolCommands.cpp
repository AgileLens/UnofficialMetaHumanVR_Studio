// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRToolCommands.h"
#include "Framework/Docking/TabManager.h"

#define LOCTEXT_NAMESPACE "FOculusXREditorModule"

void FOculusToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Meta XR Peformance Window", "Show Meta XR Peformance Window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ToggleDeploySo, "Deploy compiled .so directly to device", "Faster deploy when we only have code changes by deploying compiled .so directly to device", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(OpenPlatWindow, "Meta XR Platform Window", "Show Meta XR Platform Window", EUserInterfaceActionType::Button, FInputChord());
}

void FOculusToolCommands::ShowOculusTool()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FOculusXREditorModule::OculusPerfTabName);
}

#undef LOCTEXT_NAMESPACE
