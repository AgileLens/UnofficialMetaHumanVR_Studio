/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/
#include "OculusXRRoomLayoutManagerComponent.h"
#include "OculusXRHMD.h"
#include "OculusXRRoomLayoutManager.h"
#include "OculusXRAnchorDelegates.h"
#include "OculusXRAnchorManager.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "OculusXRAnchorsModule.h"

UOculusXRRoomLayoutManagerComponent::UOculusXRRoomLayoutManagerComponent(const FObjectInitializer& ObjectInitializer)
{
	bWantsInitializeComponent = true; // so that InitializeComponent() gets called
}

void UOculusXRRoomLayoutManagerComponent::OnRegister()
{
	Super::OnRegister();

	FOculusXRAnchorEventDelegates::OculusSceneCaptureComplete.AddUObject(this, &UOculusXRRoomLayoutManagerComponent::OculusRoomLayoutSceneCaptureComplete_Handler);
}

void UOculusXRRoomLayoutManagerComponent::OnUnregister()
{
	Super::OnUnregister();

	FOculusXRAnchorEventDelegates::OculusSceneCaptureComplete.RemoveAll(this);
}

void UOculusXRRoomLayoutManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOculusXRRoomLayoutManagerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

bool UOculusXRRoomLayoutManagerComponent::LaunchCaptureFlow()
{
	UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Launch capture flow -- UOculusXRRoomLayoutManagerComponent"));

	uint64 OutRequest = 0;
	const bool bSuccess = OculusXRAnchors::FOculusXRRoomLayoutManager::RequestSceneCapture(OutRequest);
	if (bSuccess)
	{
		EntityRequestList.Add(OutRequest);
	}

	UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Launch capture flow -- RequestSceneCapture -- %d"), bSuccess);

	return bSuccess;
}

bool UOculusXRRoomLayoutManagerComponent::GetRoomLayout(FOculusXRUInt64 Space, FOculusXRRoomLayout& RoomLayoutOut, int32 MaxWallsCapacity)
{
	if (MaxWallsCapacity <= 0)
	{
		return false;
	}

	FOculusXRUUID OutCeilingUuid;
	FOculusXRUUID OutFloorUuid;
	TArray<FOculusXRUUID> OutWallsUuid;

	const bool bSuccess = OculusXRAnchors::FOculusXRRoomLayoutManager::GetSpaceRoomLayout(Space.Value, static_cast<uint32>(MaxWallsCapacity), OutCeilingUuid, OutFloorUuid, OutWallsUuid);

	if (bSuccess)
	{
		RoomLayoutOut.CeilingUuid = OutCeilingUuid;
		RoomLayoutOut.FloorUuid = OutFloorUuid;
		RoomLayoutOut.WallsUuid.InsertZeroed(0, OutWallsUuid.Num());

		for (int32 i = 0; i < OutWallsUuid.Num(); ++i)
		{
			RoomLayoutOut.WallsUuid[i] = OutWallsUuid[i];
		}

		TArray<FOculusXRUUID> spaceUUIDs;
		EOculusXRAnchorResult::Type result = OculusXRAnchors::FOculusXRAnchorManager::GetSpaceContainerUUIDs(Space, spaceUUIDs);

		if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result))
		{
			RoomLayoutOut.RoomObjectUUIDs = spaceUUIDs;
		}
	}

	return bSuccess;
}
