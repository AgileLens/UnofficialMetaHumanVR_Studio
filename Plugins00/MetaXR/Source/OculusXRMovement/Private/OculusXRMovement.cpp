/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRMovement.h"
#include "OculusXRMovementModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"
#include "OculusXRPluginWrapper.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

namespace XRSpaceFlags
{
	const uint64 XR_SPACE_LOCATION_ORIENTATION_VALID_BIT = 0x00000001;
	const uint64 XR_SPACE_LOCATION_POSITION_VALID_BIT = 0x00000002;
} // namespace XRSpaceFlags

bool OculusXRMovement::GetBodyState(FOculusXRBodyState& outOculusXRBodyState, float WorldToMeters)
{
	static_assert(ovrpBoneId_Body_End == (int)EOculusXRBoneID::COUNT, "The size of the OVRPlugin Bone ID enum should be the same as the EOculusXRBoneID enum.");

	checkf(outOculusXRBodyState.Joints.Num() >= ovrpBoneId_Body_End, TEXT("Not enough joints in FOculusXRBodyState::Joints array. You must have at least %d joints"), ovrpBoneId_Body_End);

	ovrpBodyState OVRBodyState;
	ovrpResult OVRBodyStateResult = FOculusXRHMDModule::GetPluginWrapper().GetBodyState(ovrpStep_Render, OVRP_CURRENT_FRAMEINDEX, &OVRBodyState);
	ensureMsgf(OVRBodyStateResult != ovrpFailure_NotYetImplemented, TEXT("Body tracking is not implemented on this platform."));

	if (OVRP_SUCCESS(OVRBodyStateResult))
	{
		outOculusXRBodyState.IsActive = (OVRBodyState.IsActive == ovrpBool_True);
		outOculusXRBodyState.Confidence = OVRBodyState.Confidence;
		outOculusXRBodyState.SkeletonChangedCount = OVRBodyState.SkeletonChangedCount;
		outOculusXRBodyState.Time = static_cast<float>(OVRBodyState.Time);

		for (int i = 0; i < ovrpBoneId_Body_End; ++i)
		{
			ovrpBodyJointLocation OVRJointLocation = OVRBodyState.JointLocations[i];
			ovrpPosef OVRJointPose = OVRJointLocation.Pose;

			FOculusXRBodyJoint& OculusXRBodyJoint = outOculusXRBodyState.Joints[i];
			OculusXRBodyJoint.LocationFlags = OVRJointLocation.LocationFlags;
			OculusXRBodyJoint.bIsValid = OVRJointLocation.LocationFlags & (XRSpaceFlags::XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XRSpaceFlags::XR_SPACE_LOCATION_POSITION_VALID_BIT);
			OculusXRBodyJoint.Orientation = FRotator(OculusXRHMD::ToFQuat(OVRJointPose.Orientation));
			OculusXRBodyJoint.Position = OculusXRHMD::ToFVector(OVRJointPose.Position) * WorldToMeters;
		}

		return true;
	}

	return false;
}

bool OculusXRMovement::IsBodyTrackingEnabled()
{
	bool bResult = false;

	ovrpBool IsEnabled = ovrpBool_False;
	ovrpResult TrackingEnabledResult = FOculusXRHMDModule::GetPluginWrapper().GetBodyTrackingEnabled(&IsEnabled);

	if (OVRP_SUCCESS(TrackingEnabledResult))
	{
		bResult = (IsEnabled == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::IsBodyTrackingSupported()
{
	bool bResult = false;

	ovrpBool IsSupported = ovrpBool_False;
	ovrpResult TrackingSupportedResult = FOculusXRHMDModule::GetPluginWrapper().GetBodyTrackingSupported(&IsSupported);

	if (OVRP_SUCCESS(TrackingSupportedResult))
	{
		bResult = (IsSupported == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::StartBodyTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StartBodyTracking());
}

bool OculusXRMovement::StopBodyTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StopBodyTracking());
}

bool OculusXRMovement::GetFaceState(FOculusXRFaceState& outOculusXRFaceState)
{
	static_assert(ovrpFaceExpression_Max == (int)EOculusXRFaceExpression::COUNT, "The size of the OVRPlugin Face Expression enum should be the same as the EOculusXRFaceExpression enum.");

	checkf(outOculusXRFaceState.ExpressionWeightConfidences.Num() >= ovrpFaceConfidence_Max, TEXT("Not enough expression weight confidences in FOculusXRFaceState::ExpressionWeightConfidences. Requires %d available elements in the array."), ovrpFaceConfidence_Max);
	checkf(outOculusXRFaceState.ExpressionWeights.Num() >= ovrpFaceExpression_Max, TEXT("Not enough expression weights in FOculusXRFaceState::ExpressionWeights. Requires %d available elements in the array."), ovrpFaceExpression_Max);

	ovrpFaceState OVRFaceState;
	ovrpResult OVRFaceStateResult = FOculusXRHMDModule::GetPluginWrapper().GetFaceState(ovrpStep_Render, OVRP_CURRENT_FRAMEINDEX, &OVRFaceState);
	ensureMsgf(OVRFaceStateResult != ovrpFailure_NotYetImplemented, TEXT("Face tracking is not implemented on this platform."));

	if (OVRP_SUCCESS(OVRFaceStateResult))
	{
		outOculusXRFaceState.bIsValid = (OVRFaceState.Status.IsValid == ovrpBool_True);
		outOculusXRFaceState.bIsEyeFollowingBlendshapesValid = (OVRFaceState.Status.IsEyeFollowingBlendshapesValid == ovrpBool_True);
		outOculusXRFaceState.Time = static_cast<float>(OVRFaceState.Time);

		for (int i = 0; i < ovrpFaceExpression_Max; ++i)
		{
			outOculusXRFaceState.ExpressionWeights[i] = OVRFaceState.ExpressionWeights[i];
		}

		for (int i = 0; i < ovrpFaceConfidence_Max; ++i)
		{
			outOculusXRFaceState.ExpressionWeightConfidences[i] = OVRFaceState.ExpressionWeightConfidences[i];
		}

		return true;
	}

	return false;
}

bool OculusXRMovement::IsFaceTrackingEnabled()
{
	bool bResult = false;

	ovrpBool IsEnabled = ovrpBool_False;
	ovrpResult TrackingEnabledResult = FOculusXRHMDModule::GetPluginWrapper().GetFaceTrackingEnabled(&IsEnabled);

	if (OVRP_SUCCESS(TrackingEnabledResult))
	{
		bResult = (IsEnabled == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::IsFaceTrackingSupported()
{
	bool bResult = false;

	ovrpBool IsSupported = ovrpBool_False;
	ovrpResult TrackingSupportedResult = FOculusXRHMDModule::GetPluginWrapper().GetFaceTrackingSupported(&IsSupported);

	if (OVRP_SUCCESS(TrackingSupportedResult))
	{
		bResult = (IsSupported == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::StartFaceTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StartFaceTracking());
}

bool OculusXRMovement::StopFaceTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StopFaceTracking());
}

bool OculusXRMovement::GetEyeGazesState(FOculusXREyeGazesState& outOculusXREyeGazesState, float WorldToMeters)
{
	static_assert(ovrpEye_Count == (int)EOculusXREye::COUNT, "The size of the OVRPlugin Eye enum should be the same as the EOculusXREye enum.");

	checkf(outOculusXREyeGazesState.EyeGazes.Num() >= ovrpEye_Count, TEXT("Not enough eye gaze states in FOculusXREyeGazesState::EyeGazes. Requires %d available elements in the array."), ovrpEye_Count);

	ovrpEyeGazesState OVREyeGazesState;
	ovrpResult OVREyeGazesStateResult = FOculusXRHMDModule::GetPluginWrapper().GetEyeGazesState(ovrpStep_Render, OVRP_CURRENT_FRAMEINDEX, &OVREyeGazesState);
	ensureMsgf(OVREyeGazesStateResult != ovrpFailure_NotYetImplemented, TEXT("Eye tracking is not implemented on this platform."));

	if (OVRP_SUCCESS(OVREyeGazesStateResult))
	{
		outOculusXREyeGazesState.Time = static_cast<float>(OVREyeGazesState.Time);
		for (int i = 0; i < ovrpEye_Count; ++i)
		{
			const auto& EyeGazePose = OVREyeGazesState.EyeGazes[i].Pose;
			outOculusXREyeGazesState.EyeGazes[i].Orientation = FRotator(OculusXRHMD::ToFQuat(EyeGazePose.Orientation));
			outOculusXREyeGazesState.EyeGazes[i].Position = OculusXRHMD::ToFVector(EyeGazePose.Position) * WorldToMeters;
			outOculusXREyeGazesState.EyeGazes[i].bIsValid = (OVREyeGazesState.EyeGazes[i].IsValid == ovrpBool_True);
			outOculusXREyeGazesState.EyeGazes[i].Confidence = OVREyeGazesState.EyeGazes[i].Confidence;
		}

		return true;
	}

	return false;
}

bool OculusXRMovement::IsEyeTrackingEnabled()
{
	bool bResult = false;

	ovrpBool IsEnabled = ovrpBool_False;
	ovrpResult TrackingEnabledResult = FOculusXRHMDModule::GetPluginWrapper().GetEyeTrackingEnabled(&IsEnabled);

	if (OVRP_SUCCESS(TrackingEnabledResult))
	{
		bResult = (IsEnabled == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::IsEyeTrackingSupported()
{
	bool bResult = false;

	ovrpBool IsSupported = ovrpBool_False;
	ovrpResult TrackingSupportedResult = FOculusXRHMDModule::GetPluginWrapper().GetEyeTrackingSupported(&IsSupported);

	if (OVRP_SUCCESS(TrackingSupportedResult))
	{
		bResult = (IsSupported == ovrpBool_True);
	}

	return bResult;
}

bool OculusXRMovement::StartEyeTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StartEyeTracking());
}

bool OculusXRMovement::StopEyeTracking()
{
	return OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().StopEyeTracking());
}

#undef LOCTEXT_NAMESPACE
