/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimTypes.h"
#include "LiveLinkRetargetAsset.h"
#include "Containers/StaticArray.h"
#include "OculusXRMovementTypes.h"

#include "OculusXRLiveLinkRetargetFaceAsset.generated.h"

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXRAnimCurveMapping
{
	GENERATED_BODY()

	FOculusXRAnimCurveMapping(){};

	FOculusXRAnimCurveMapping(const std::initializer_list<FName> CurveNamesList)
		: CurveNames(CurveNamesList)
	{
	}

	/**
	 * Skeleton's animation curve names
	 */
	UPROPERTY(EditAnywhere, Category = "OculusXR|Movement")
	TArray<FName> CurveNames;
};

UCLASS(Blueprintable, meta = (DisplayName = "MetaXR MovementSDK LiveLink retarget face asset"), ClassGroup = OculusXRHMD)
class OCULUSXRMOVEMENT_API UOculusXRLiveLinkRetargetFaceAsset : public ULiveLinkRetargetAsset
{
	GENERATED_UCLASS_BODY()

	virtual void Initialize() override;
	virtual void BuildPoseAndCurveFromBaseData(float DeltaTime, const FLiveLinkBaseStaticData* InBaseStaticData, const FLiveLinkBaseFrameData* InBaseFrameData, FCompactPose& OutPose, FBlendedCurve& OutCurve) override;

	/**
	 * Map face expression to Skeleton's animation curve mapping names.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "OculusXR|Movement")
	TMap<EOculusXRFaceExpression, FOculusXRAnimCurveMapping> CurveRemapping;

private:
	// Latest skeleton used to build pose
	FGuid LastSkeletonGuid;

	// Remapping used for latest used skeleton
	TStaticArray<TArray<SmartName::UID_Type>, static_cast<uint8>(EOculusXRFaceExpression::COUNT)> RemappingForLastSkeleton;

	// Recalculate skeleton dependent mappings
	void OnSkeletonChanged(const USkeleton* Skeleton);
};
