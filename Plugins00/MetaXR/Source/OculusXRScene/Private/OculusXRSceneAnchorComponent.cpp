/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRSceneAnchorComponent.h"

#include "Engine/StaticMeshActor.h"

UOculusXRSceneAnchorComponent::UOculusXRSceneAnchorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUpdateHeadSpaceTransform = false;
}

void UOculusXRSceneAnchorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetHandle().Value == 0)
	{
		return;
	}
}
