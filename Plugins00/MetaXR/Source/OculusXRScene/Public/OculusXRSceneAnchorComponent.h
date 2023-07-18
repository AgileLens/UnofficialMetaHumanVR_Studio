/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"
#include "OculusXRAnchorComponent.h"
#include "OculusXRSceneAnchorComponent.generated.h"

UCLASS(meta = (DisplayName = "OculusXR Scene Anchor Component", BlueprintSpawnableComponent))
class UOculusXRSceneAnchorComponent : public UOculusXRAnchorComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UOculusXRSceneAnchorComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "OculusXR|Scene Anchor Component")
	TArray<FString> SemanticClassifications;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "OculusXR|Scene Anchor Component")
	FOculusXRUInt64 RoomSpaceID = 0;
};
