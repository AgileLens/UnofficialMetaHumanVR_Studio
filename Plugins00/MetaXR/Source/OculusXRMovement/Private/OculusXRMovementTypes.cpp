/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRMovementTypes.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"

FOculusXRBodyJoint::FOculusXRBodyJoint()
	: LocationFlags(0)
	, bIsValid(false)
	, Orientation(FRotator::ZeroRotator)
	, Position(FVector::ZeroVector)
{
}

FOculusXRBodyState::FOculusXRBodyState()
	: IsActive(false)
	, Confidence(0)
	, SkeletonChangedCount(0)
	, Time(0.f)
{
	Joints.SetNum(static_cast<int32>(EOculusXRBoneID::COUNT));
}

FOculusXRFaceState::FOculusXRFaceState()
	: bIsValid(false)
	, bIsEyeFollowingBlendshapesValid(false)
	, Time(0.f)
{
	ExpressionWeights.SetNum(static_cast<int32>(EOculusXRFaceExpression::COUNT));
	ExpressionWeightConfidences.SetNum(static_cast<int32>(EOculusXRFaceConfidence::COUNT));
}

FOculusXREyeGazeState::FOculusXREyeGazeState()
	: Orientation(FRotator::ZeroRotator)
	, Position(FVector::ZeroVector)
	, Confidence(0.f)
	, bIsValid(false)
{
}

FOculusXREyeGazesState::FOculusXREyeGazesState()
	: Time(0.f)
{
	EyeGazes.SetNum(static_cast<int32>(EOculusXREye::COUNT));
}
