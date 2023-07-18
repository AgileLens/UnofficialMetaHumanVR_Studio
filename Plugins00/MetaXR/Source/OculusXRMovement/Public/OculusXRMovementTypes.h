/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"

#include "OculusXRMovementTypes.generated.h"

UENUM(BlueprintType)
enum class EOculusXRBoneID : uint8
{
	BodyRoot = 0,
	BodyHips = 1,
	BodySpineLower = 2,
	BodySpineMiddle = 3,
	BodySpineUpper = 4,
	BodyChest = 5,
	BodyNeck = 6,
	BodyHead = 7,
	BodyLeftShoulder = 8,
	BodyLeftScapula = 9,
	BodyLeftArmUpper = 10,
	BodyLeftArmLower = 11,
	BodyLeftHandWristTwist = 12,
	BodyRightShoulder = 13,
	BodyRightScapula = 14,
	BodyRightArmUpper = 15,
	BodyRightArmLower = 16,
	BodyRightHandWristTwist = 17,
	BodyLeftHandPalm = 18,
	BodyLeftHandWrist = 19,
	BodyLeftHandThumbMetacarpal = 20,
	BodyLeftHandThumbProximal = 21,
	BodyLeftHandThumbDistal = 22,
	BodyLeftHandThumbTip = 23,
	BodyLeftHandIndexMetacarpal = 24,
	BodyLeftHandIndexProximal = 25,
	BodyLeftHandIndexIntermediate = 26,
	BodyLeftHandIndexDistal = 27,
	BodyLeftHandIndexTip = 28,
	BodyLeftHandMiddleMetacarpal = 29,
	BodyLeftHandMiddleProximal = 30,
	BodyLeftHandMiddleIntermediate = 31,
	BodyLeftHandMiddleDistal = 32,
	BodyLeftHandMiddleTip = 33,
	BodyLeftHandRingMetacarpal = 34,
	BodyLeftHandRingProximal = 35,
	BodyLeftHandRingIntermediate = 36,
	BodyLeftHandRingDistal = 37,
	BodyLeftHandRingTip = 38,
	BodyLeftHandLittleMetacarpal = 39,
	BodyLeftHandLittleProximal = 40,
	BodyLeftHandLittleIntermediate = 41,
	BodyLeftHandLittleDistal = 42,
	BodyLeftHandLittleTip = 43,
	BodyRightHandPalm = 44,
	BodyRightHandWrist = 45,
	BodyRightHandThumbMetacarpal = 46,
	BodyRightHandThumbProximal = 47,
	BodyRightHandThumbDistal = 48,
	BodyRightHandThumbTip = 49,
	BodyRightHandIndexMetacarpal = 50,
	BodyRightHandIndexProximal = 51,
	BodyRightHandIndexIntermediate = 52,
	BodyRightHandIndexDistal = 53,
	BodyRightHandIndexTip = 54,
	BodyRightHandMiddleMetacarpal = 55,
	BodyRightHandMiddleProximal = 56,
	BodyRightHandMiddleIntermediate = 57,
	BodyRightHandMiddleDistal = 58,
	BodyRightHandMiddleTip = 59,
	BodyRightHandRingMetacarpal = 60,
	BodyRightHandRingProximal = 61,
	BodyRightHandRingIntermediate = 62,
	BodyRightHandRingDistal = 63,
	BodyRightHandRingTip = 64,
	BodyRightHandLittleMetacarpal = 65,
	BodyRightHandLittleProximal = 66,
	BodyRightHandLittleIntermediate = 67,
	BodyRightHandLittleDistal = 68,
	BodyRightHandLittleTip = 69,
	COUNT = 70,
};

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXRBodyJoint
{
	GENERATED_BODY()
public:
	FOculusXRBodyJoint();

	uint64 LocationFlags;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	bool bIsValid;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	FRotator Orientation;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	FVector Position;
};

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXRBodyState
{
	GENERATED_BODY()
public:
	FOculusXRBodyState();

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	bool IsActive;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	float Confidence;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	int SkeletonChangedCount;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	float Time;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	TArray<FOculusXRBodyJoint> Joints;
};

UENUM(BlueprintType)
enum class EOculusXRFaceExpression : uint8
{
	// Removed invalid to make this supported as a uint8 enum class
	BrowLowererL = 0,
	BrowLowererR = 1,
	CheekPuffL = 2,
	CheekPuffR = 3,
	CheekRaiserL = 4,
	CheekRaiserR = 5,
	CheekSuckL = 6,
	CheekSuckR = 7,
	ChinRaiserB = 8,
	ChinRaiserT = 9,
	DimplerL = 10,
	DimplerR = 11,
	EyesClosedL = 12,
	EyesClosedR = 13,
	EyesLookDownL = 14,
	EyesLookDownR = 15,
	EyesLookLeftL = 16,
	EyesLookLeftR = 17,
	EyesLookRightL = 18,
	EyesLookRightR = 19,
	EyesLookUpL = 20,
	EyesLookUpR = 21,
	InnerBrowRaiserL = 22,
	InnerBrowRaiserR = 23,
	JawDrop = 24,
	JawSidewaysLeft = 25,
	JawSidewaysRight = 26,
	JawThrust = 27,
	LidTightenerL = 28,
	LidTightenerR = 29,
	LipCornerDepressorL = 30,
	LipCornerDepressorR = 31,
	LipCornerPullerL = 32,
	LipCornerPullerR = 33,
	LipFunnelerLB = 34,
	LipFunnelerLT = 35,
	LipFunnelerRB = 36,
	LipFunnelerRT = 37,
	LipPressorL = 38,
	LipPressorR = 39,
	LipPuckerL = 40,
	LipPuckerR = 41,
	LipStretcherL = 42,
	LipStretcherR = 43,
	LipSuckLB = 44,
	LipSuckLT = 45,
	LipSuckRB = 46,
	LipSuckRT = 47,
	LipTightenerL = 48,
	LipTightenerR = 49,
	LipsToward = 50,
	LowerLipDepressorL = 51,
	LowerLipDepressorR = 52,
	MouthLeft = 53,
	MouthRight = 54,
	NoseWrinklerL = 55,
	NoseWrinklerR = 56,
	OuterBrowRaiserL = 57,
	OuterBrowRaiserR = 58,
	UpperLidRaiserL = 59,
	UpperLidRaiserR = 60,
	UpperLipRaiserL = 61,
	UpperLipRaiserR = 62,
	COUNT = 63,
};

UENUM(BlueprintType)
enum class EOculusXRFaceConfidence : uint8
{
	Lower = 0,
	Upper = 1,
	COUNT = 2,
};

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXRFaceState
{
	GENERATED_BODY()
public:
	FOculusXRFaceState();

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	TArray<float> ExpressionWeights;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	TArray<float> ExpressionWeightConfidences;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	bool bIsValid;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	bool bIsEyeFollowingBlendshapesValid;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	float Time;
};

UENUM(BlueprintType)
enum class EOculusXREye : uint8
{
	Left = 0,
	Right = 1,
	COUNT = 2,
};

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXREyeGazeState
{
	GENERATED_BODY()
public:
	FOculusXREyeGazeState();

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	FRotator Orientation;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	FVector Position;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	float Confidence;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	bool bIsValid;
};

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXREyeGazesState
{
	GENERATED_BODY()
public:
	FOculusXREyeGazesState();

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	TArray<FOculusXREyeGazeState> EyeGazes;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Movement")
	float Time;
};
