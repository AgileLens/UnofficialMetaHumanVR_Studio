#include "OculusXRMovementLiveLink.h"

#include "IHeadMountedDisplayModule.h"
#include "OculusXRHMDModule.h"
#include "OculusXRMovementLog.h"
#include "OculusXRMovement.h"
#include "OculusXRMovementTypes.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "ILiveLinkClient.h"

#define LOCTEXT_NAMESPACE "MetaOculusXRMovement"

namespace
{
	constexpr int32 NoParent = -1;
}

namespace MetaXRMovement
{
	template <typename MetaXRDataFields>
	void InitializeRoleStaticData(FLiveLinkSkeletonStaticData& StaticData)
	{
		constexpr auto FieldsCount = static_cast<uint8>(MetaXRDataFields::COUNT);
		StaticData.BoneNames.Reserve(FieldsCount);
		for (uint8 XRBone = 0; XRBone < FieldsCount; ++XRBone)
		{
			StaticData.BoneNames.Add(UEnum::GetValueAsName(static_cast<MetaXRDataFields>(XRBone)));
			StaticData.BoneParents.Add(NoParent);
		}
	}
	template <typename MetaXRDataFields>
	void InitializeRoleStaticData(FLiveLinkBaseStaticData& StaticData)
	{
		constexpr auto FieldsCount = static_cast<uint8>(MetaXRDataFields::COUNT);
		StaticData.PropertyNames.Reserve(FieldsCount);
		for (uint8 XRProperty = 0; XRProperty < FieldsCount; ++XRProperty)
		{
			StaticData.PropertyNames.Add(UEnum::GetValueAsName(static_cast<MetaXRDataFields>(XRProperty)));
		}
	}
	template <typename MetaXRDataFields, typename MetaXRState, typename RoleTypeStaticData, typename RoleTypeFrameData, typename Role>
	FLiveLinkStaticDataStruct TSubject<MetaXRDataFields, MetaXRState, RoleTypeStaticData, RoleTypeFrameData, Role>::StaticData() const
	{
		FLiveLinkStaticDataStruct StaticDataStruct(RoleTypeStaticData::StaticStruct());
		RoleTypeStaticData& RoleStaticData(*StaticDataStruct.Cast<RoleTypeStaticData>());
		InitializeRoleStaticData<MetaXRDataFields>(RoleStaticData);
		return StaticDataStruct;
	}
	template <typename MetaXRDataFields, typename MetaXRState, typename RoleTypeStaticData, typename RoleTypeFrameData, typename Role>
	FLiveLinkFrameDataStruct TSubject<MetaXRDataFields, MetaXRState, RoleTypeStaticData, RoleTypeFrameData, Role>::FrameData()
	{
		FLiveLinkFrameDataStruct FrameDataStruct(RoleTypeFrameData::StaticStruct());
		RoleTypeFrameData& FrameData(*FrameDataStruct.Cast<RoleTypeFrameData>());
		UpdateFrame(FrameData);
		return FrameDataStruct;
	}

	template <>
	FEyeSubject::TSubject()
		: Name(TEXT("Eye"))
		, bLastFrameIsValid(false)
		, bStarted(false)
	{
	}
	template <>
	FFaceSubject::TSubject()
		: Name(TEXT("Face"))
		, bLastFrameIsValid(false)
		, bStarted(false)
	{
	}
	template <>
	FBodySubject::TSubject()
		: Name(TEXT("Body"))
		, bLastFrameIsValid(false)
		, bStarted(false)
	{
	}
	template <>
	bool FEyeSubject::Start()
	{
		if (!bStarted)
		{
			bStarted = OculusXRMovement::StartEyeTracking();
		}
		return bStarted;
	}
	template <>
	bool FEyeSubject::Stop()
	{
		if (bStarted)
		{
			bStarted = !OculusXRMovement::StopEyeTracking();
		}
		return !bStarted;
	}
	template <>
	bool FFaceSubject::Start()
	{
		if (!bStarted)
		{
			bStarted = OculusXRMovement::StartFaceTracking();
		}
		return bStarted;
	}
	template <>
	bool FFaceSubject::Stop()
	{
		if (bStarted)
		{
			bStarted = !OculusXRMovement::StopFaceTracking();
		}
		return !bStarted;
	}
	template <>
	bool FBodySubject::Start()
	{
		if (!bStarted)
		{
			bStarted = OculusXRMovement::StartBodyTracking();
		}
		return bStarted;
	}
	template <>
	bool FBodySubject::Stop()
	{
		if (bStarted)
		{
			bStarted = !OculusXRMovement::StopBodyTracking();
		}
		return !bStarted;
	}
	template <>
	bool FEyeSubject::IsSupported()
	{
		return OculusXRMovement::IsEyeTrackingSupported();
	}
	template <>
	bool FFaceSubject::IsSupported()
	{
		return OculusXRMovement::IsFaceTrackingSupported();
	}
	template <>
	bool FBodySubject::IsSupported()
	{
		return OculusXRMovement::IsBodyTrackingSupported();
	}
	template <>
	void FEyeSubject::UpdateFrame(FLiveLinkAnimationFrameData& FrameData)
	{
		bLastFrameIsValid = OculusXRMovement::GetEyeGazesState(LastState, 1.f);
		if (bLastFrameIsValid && (LastState.EyeGazes[0].bIsValid || LastState.EyeGazes[1].bIsValid))
		{
			constexpr auto FieldsCount = static_cast<uint8>(EOculusXREye::COUNT);
			FrameData.Transforms.Reserve(FieldsCount);
			for (uint8 i = 0u; i < FieldsCount; ++i)
			{
				const auto& EyeGaze = LastState.EyeGazes[i];
				FrameData.Transforms.Emplace(EyeGaze.Orientation, EyeGaze.Position);
			}
			FrameData.WorldTime = FPlatformTime::Seconds();
		}
	}
	template <>
	void FFaceSubject::UpdateFrame(FLiveLinkBaseFrameData& FrameData)
	{
		bLastFrameIsValid = OculusXRMovement::GetFaceState(LastState);
		if (bLastFrameIsValid && (LastState.bIsValid))
		{
			constexpr auto FieldsCount = static_cast<uint8>(EOculusXRFaceExpression::COUNT);
			FrameData.PropertyValues.Reserve(FieldsCount);
			for (uint8 i = 0u; i < FieldsCount; ++i)
			{
				FrameData.PropertyValues.Emplace(LastState.ExpressionWeights[i]);
			}
			FrameData.WorldTime = FPlatformTime::Seconds();
		}
	}
	template <>
	void FBodySubject::UpdateFrame(FLiveLinkAnimationFrameData& FrameData)
	{
		bLastFrameIsValid = OculusXRMovement::GetBodyState(LastState, 1.f);
		if (bLastFrameIsValid && (LastState.IsActive) && (LastState.SkeletonChangedCount > 0))
		{
			constexpr auto FieldsCount = static_cast<uint8>(EOculusXRBoneID::COUNT);
			FrameData.Transforms.Reserve(FieldsCount);
			for (uint8 i = 0u; i < FieldsCount; ++i)
			{
				const auto& Joint = LastState.Joints[i];
				FrameData.Transforms.Emplace(Joint.Orientation, Joint.Position);
			}
			FrameData.WorldTime = FPlatformTime::Seconds();
		}
	}

	LiveLinkSource::LiveLinkSource()
		: bAnySupported(FEyeSubject::IsSupported() || FFaceSubject::IsSupported() || FBodySubject::IsSupported())
	{
	}

	void LiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
	{
		Client = InClient;
		SourceGuid = InSourceGuid;

		InitializeMovementSubjects();
		UpdateMovementSubjects();
	}

	bool LiveLinkSource::IsSourceStillValid() const
	{
		return Client != nullptr;
	}

	bool LiveLinkSource::RequestSourceShutdown()
	{
		Client = nullptr;
		SourceGuid.Invalidate();

		if (!(Body.Stop() && Face.Stop() && Eye.Stop()))
		{
			UE_LOG(LogOculusXRMovement, Error, TEXT("At least one of the trackers cannot stop."));
		}
		return true;
	}

	FText LiveLinkSource::GetSourceType() const
	{
		return LOCTEXT("MetaOculusXRMovementLiveLinkSourceType", "MetaXR MovementSDK");
	}

	FText LiveLinkSource::GetSourceMachineName() const
	{
		if (IHeadMountedDisplayModule::IsAvailable())
		{
			const FString DeviceName = IHeadMountedDisplayModule::Get().GetDeviceSystemName();
			return FText::FromString(DeviceName);
		}
		return LOCTEXT("MetaOculusXRMovementLiveLinkMachineName", "MetaXR Device");
	}

	FText LiveLinkSource::GetSourceStatus() const
	{
		if (bAnySupported)
		{
			return LOCTEXT("MetaOculusXRMovementLiveLinkStatusSupported", "Active");
		}
		return LOCTEXT("MetaOculusXRMovementLiveLinkStatusNotSupported", "Not Supported");
	}

	void LiveLinkSource::Tick(float DeltaTime)
	{
		UpdateMovementSubjects();
	}

	template <typename SubjectT>
	void LiveLinkSource::InitializeMovementSubject(TOptional<FLiveLinkSubjectKey>& Key, SubjectT& Subject)
	{
		if (Key)
		{
			if (Key->Source.IsValid())
			{ // If the key was already in use. Remove it.
				Client->RemoveSubject_AnyThread(*Key);
			}
			Key.Reset();
		}
		if (Subject.IsSupported())
		{
			Key = FLiveLinkSubjectKey(SourceGuid, Subject.Name);
			if (!Subject.Start())
			{
				UE_LOG(LogOculusXRMovement, Error, TEXT("Tracker for LiveLink subject %s cannot start."), *Subject.Name.ToString());
			}
			using Role = typename std::remove_reference_t<decltype(Subject)>::Role;
			Client->PushSubjectStaticData_AnyThread(*Key, Role::StaticClass(), Subject.StaticData());
		}
		else
		{
			UE_LOG(LogOculusXRMovement, Log, TEXT("LiveLink subject %s is not supported."), *Subject.Name.ToString());
		}
	}
	void LiveLinkSource::InitializeMovementSubjects()
	{
		check(IsInGameThread());
		InitializeMovementSubject(KeyEye, Eye);
		InitializeMovementSubject(KeyFace, Face);
		InitializeMovementSubject(KeyBody, Body);
	}
	template <typename SubjectT>
	void LiveLinkSource::UpdateMovementSubject(const TOptional<FLiveLinkSubjectKey>& Key, SubjectT& Subject)
	{
		if (Key)
		{
			const bool bPreviousFrameValid = Subject.IsLastFrameValid();
			auto FrameData = Subject.FrameData();
			const bool bFrameValid = Subject.IsLastFrameValid();
			if (bPreviousFrameValid != bFrameValid)
			{
				UE_LOG(LogOculusXRMovement, Log, TEXT("LiveLink subject %s became %s."), *Subject.Name.ToString(), bFrameValid ? TEXT("valid") : TEXT("invalid"));
			}
			if (bFrameValid)
			{
				Client->PushSubjectFrameData_AnyThread(*Key, MoveTemp(FrameData));
			}
		}
	}

	void LiveLinkSource::UpdateMovementSubjects()
	{
		check(IsInGameThread());
		if (IsSourceStillValid())
		{
			UpdateMovementSubject(KeyEye, Eye);
			UpdateMovementSubject(KeyFace, Face);
			UpdateMovementSubject(KeyBody, Body);
		}
	}
} // namespace MetaXRMovement
#undef LOCTEXT_NAMESPACE
