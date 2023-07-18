/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "OculusXRMovementTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OculusXRMovementFunctionLibrary.generated.h"

UCLASS()
class OCULUSXRMOVEMENT_API UOculusXRMovementFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "OculusXR|Body")
	static bool TryGetBodyState(FOculusXRBodyState& outBodyState, float WorldToMeters = 100.0f);

	UFUNCTION(BlueprintPure, Category = "OculusXR|Body")
	static bool IsBodyTrackingEnabled();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Body")
	static bool IsBodyTrackingSupported();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Body")
	static bool StartBodyTracking();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Body")
	static bool StopBodyTracking();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Face")
	static bool TryGetFaceState(FOculusXRFaceState& outFaceState);

	UFUNCTION(BlueprintPure, Category = "OculusXR|Face")
	static bool IsFaceTrackingEnabled();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Face")
	static bool IsFaceTrackingSupported();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Face")
	static bool StartFaceTracking();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Face")
	static bool StopFaceTracking();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Eyes")
	static bool TryGetEyeGazesState(FOculusXREyeGazesState& outEyeGazesState, float WorldToMeters = 100.0f);

	UFUNCTION(BlueprintPure, Category = "OculusXR|Eyes")
	static bool IsEyeTrackingEnabled();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Eyes")
	static bool IsEyeTrackingSupported();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Eyes")
	static bool StartEyeTracking();

	UFUNCTION(BlueprintPure, Category = "OculusXR|Eyes")
	static bool StopEyeTracking();
};
