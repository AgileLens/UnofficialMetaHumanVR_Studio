/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"
#include "OculusXRSpatialAnchorComponent.h"
#include "OculusXRRoomLayoutManagerComponent.generated.h"

// Represents a room layout within a specific space
USTRUCT(BlueprintType)
struct OCULUSXRANCHORS_API FOculusXRRoomLayout
{
	GENERATED_USTRUCT_BODY()

	FOculusXRUUID FloorUuid;
	FOculusXRUUID CeilingUuid;
	TArray<FOculusXRUUID> WallsUuid;
	TArray<FOculusXRUUID> RoomObjectUUIDs;
};

UCLASS(meta = (DisplayName = "OculusXR Room Layout Manager Component", BlueprintSpawnableComponent))
class OCULUSXRANCHORS_API UOculusXRRoomLayoutManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOculusXRRoomLayoutManagerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOculusXRRoomLayoutSceneCaptureCompleteDelegate,
		FOculusXRUInt64, requestId,
		bool, result);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOculusXRRoomLayoutSceneCompleteNativeDelegate, FOculusXRUInt64 /*requestId*/, bool /*success*/);
	FOculusXRRoomLayoutSceneCompleteNativeDelegate OculusXRRoomLayoutSceneCaptureCompleteNative;

	UPROPERTY(BlueprintAssignable, Category = "OculusXR|Room Layout Manager")
	FOculusXRRoomLayoutSceneCaptureCompleteDelegate OculusXRRoomLayoutSceneCaptureComplete;

	// Requests to launch Capture Flow
	UFUNCTION(BlueprintCallable, Category = "OculusXR|Room Layout Manager")
	bool LaunchCaptureFlow();

	// Gets room layout for a specific space
	UFUNCTION(BlueprintCallable, Category = "OculusXR|Room Layout Manager")
	bool GetRoomLayout(FOculusXRUInt64 Space, UPARAM(ref) FOculusXRRoomLayout& RoomLayoutOut, int32 MaxWallsCapacity = 64);

protected:
	UPROPERTY(Transient)
	TSet<uint64> EntityRequestList;

	UPROPERTY(Transient)
	TMap<FOculusXRUInt64, FOculusXRRoomLayout> RoomLayouts;

private:
	UFUNCTION()
	void OculusRoomLayoutSceneCaptureComplete_Handler(FOculusXRUInt64 RequestId, bool bSuccess)
	{
		if (EntityRequestList.Find(RequestId.Value) != nullptr)
		{
			OculusXRRoomLayoutSceneCaptureComplete.Broadcast(RequestId, bSuccess);
			OculusXRRoomLayoutSceneCaptureCompleteNative.Broadcast(RequestId, bSuccess);
			EntityRequestList.Remove(RequestId.Value);
		}
	}
};
