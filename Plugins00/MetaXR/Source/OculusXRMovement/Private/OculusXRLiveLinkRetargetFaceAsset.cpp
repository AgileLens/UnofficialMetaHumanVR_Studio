/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRLiveLinkRetargetFaceAsset.h"

#include "LiveLinkTypes.h"
#include "Algo/ForEach.h"
#include "Animation/AnimCurveTypes.h"
#include "BonePose.h"
#include "OculusXRMovement.h"
#include "OculusXRMovementLog.h"

UOculusXRLiveLinkRetargetFaceAsset::UOculusXRLiveLinkRetargetFaceAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOculusXRLiveLinkRetargetFaceAsset::Initialize()
{
	LastSkeletonGuid.Invalidate();
	Algo::ForEach(RemappingForLastSkeleton, [](TArray<SmartName::UID_Type>& Arr) { Arr.Reset(); });
}

void UOculusXRLiveLinkRetargetFaceAsset::BuildPoseAndCurveFromBaseData(float DeltaTime, const FLiveLinkBaseStaticData* InBaseStaticData, const FLiveLinkBaseFrameData* InBaseFrameData, FCompactPose& OutPose, FBlendedCurve& OutCurve)
{
	check(InBaseFrameData);
	if (static_cast<int32>(EOculusXRFaceExpression::COUNT) != InBaseFrameData->PropertyValues.Num())
	{
		UE_LOG(LogOculusXRMovement, Error, TEXT("Received wrong data of live link frame. This retargeting asset must be used with Meta MovementSDK Live Link source and Face subject. (received %d face expressions, expected %d)"), InBaseFrameData->PropertyValues.Num(), static_cast<int32>(EOculusXRFaceExpression::COUNT));
		return;
	}
	const USkeleton* Skeleton = OutPose.GetBoneContainer().GetSkeletonAsset();
	if (!IsValid(Skeleton))
	{
		UE_LOG(LogOculusXRMovement, Error, TEXT("No skeleton asset for this retargeting."));
		return;
	}
	if (LastSkeletonGuid != Skeleton->GetGuid())
	{
		OnSkeletonChanged(Skeleton);
	}

	for (uint8 ExpressionId = 0; ExpressionId < static_cast<uint8>(EOculusXRFaceExpression::COUNT); ++ExpressionId)
	{
		for (const SmartName::UID_Type UID : RemappingForLastSkeleton[ExpressionId])
		{
			OutCurve.Set(UID, InBaseFrameData->PropertyValues[ExpressionId]);
		}
	}
}

void UOculusXRLiveLinkRetargetFaceAsset::OnSkeletonChanged(const USkeleton* Skeleton)
{
	Initialize();

	for (const auto& [ExpressionId, CurveMapping] : CurveRemapping)
	{
		for (const auto& CurveName : CurveMapping.CurveNames)
		{
			if (const SmartName::UID_Type UID = Skeleton->GetUIDByName(USkeleton::AnimCurveMappingName, CurveName); UID != SmartName::MaxUID)
			{
				RemappingForLastSkeleton[static_cast<uint8>(ExpressionId)].Emplace(UID);
			}
		}
	}

	LastSkeletonGuid = Skeleton->GetGuid();
}
