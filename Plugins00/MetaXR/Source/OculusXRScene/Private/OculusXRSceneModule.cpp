// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRSceneModule.h"

#if OCULUS_SCENE_SUPPORTED_PLATFORMS
#include "OculusXRHMDModule.h"

DEFINE_LOG_CATEGORY(LogOculusXRScene);

#define LOCTEXT_NAMESPACE "OculusXRScene"

//-------------------------------------------------------------------------------------------------
// FOculusXRSceneModule
//-------------------------------------------------------------------------------------------------
void FOculusXRSceneModule::StartupModule()
{
}

void FOculusXRSceneModule::ShutdownModule()
{
}

#endif // OCULUS_SCENE_SUPPORTED_PLATFORMS

IMPLEMENT_MODULE(FOculusXRSceneModule, OculusXRScene)

#undef LOCTEXT_NAMESPACE
