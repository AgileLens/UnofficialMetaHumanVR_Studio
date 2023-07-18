/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRAnchorManager.h"

#include <vector>

#include "OculusXRHMD.h"
#include "OculusXRAnchorsModule.h"
#include "OculusXRAnchorDelegates.h"
#include "OculusXRAnchorBPFunctionLibrary.h"

namespace OculusXRAnchors
{
	OculusXRHMD::FOculusXRHMD* GetHMD(bool& OutSuccessful)
	{
		OculusXRHMD::FOculusXRHMD* OutHMD = OculusXRHMD::FOculusXRHMD::GetOculusXRHMD();
		if (!OutHMD)
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("Unable to retrieve OculusXRHMD"));
			OutSuccessful = false;
		}

		OutSuccessful = true;

		return OutHMD;
	}

	ovrpUuid ConvertFOculusXRUUIDtoOvrpUuid(const FOculusXRUUID& UUID)
	{
		ovrpUuid Result;
		FMemory::Memcpy(Result.data, UUID.UUIDBytes);

		return Result;
	}

	ovrpSpaceQueryInfo ConvertToOVRPSpaceQueryInfo(const FOculusXRSpaceQueryInfo& UEQueryInfo)
	{
		static const int32 MaxIdsInFilter = 1024;
		static const int32 MaxComponentTypesInFilter = 16;

		ovrpSpaceQueryInfo Result;
		Result.queryType = ovrpSpaceQueryType_Action;
		Result.actionType = ovrpSpaceQueryActionType_Load;

		Result.maxQuerySpaces = UEQueryInfo.MaxQuerySpaces;
		Result.timeout = static_cast<double>(UEQueryInfo.Timeout); // Prevent compiler warnings, though there is a possible loss of data here.

		switch (UEQueryInfo.Location)
		{
			case EOculusXRSpaceStorageLocation::Invalid:
				Result.location = ovrpSpaceStorageLocation_Invalid;
				break;
			case EOculusXRSpaceStorageLocation::Local:
				Result.location = ovrpSpaceStorageLocation_Local;
				break;
			case EOculusXRSpaceStorageLocation::Cloud:
				Result.location = ovrpSpaceStorageLocation_Cloud;
				break;
		}

		switch (UEQueryInfo.FilterType)
		{
			case EOculusXRSpaceQueryFilterType::None:
				Result.filterType = ovrpSpaceQueryFilterType_None;
				break;
			case EOculusXRSpaceQueryFilterType::FilterByIds:
				Result.filterType = ovrpSpaceQueryFilterType_Ids;
				break;
			case EOculusXRSpaceQueryFilterType::FilterByComponentType:
				Result.filterType = ovrpSpaceQueryFilterType_Components;
				break;
		}

		Result.IdInfo.numIds = FMath::Min(MaxIdsInFilter, UEQueryInfo.IDFilter.Num());
		for (int i = 0; i < Result.IdInfo.numIds; ++i)
		{
			ovrpUuid OvrUuid = ConvertFOculusXRUUIDtoOvrpUuid(UEQueryInfo.IDFilter[i]);
			Result.IdInfo.ids[i] = OvrUuid;
		}

		Result.componentsInfo.numComponents = FMath::Min(MaxComponentTypesInFilter, UEQueryInfo.ComponentFilter.Num());
		for (int i = 0; i < Result.componentsInfo.numComponents; ++i)
		{
			ovrpSpaceComponentType componentType = ovrpSpaceComponentType::ovrpSpaceComponentType_Max;

			switch (UEQueryInfo.ComponentFilter[i])
			{
				case EOculusXRSpaceComponentType::Locatable:
					componentType = ovrpSpaceComponentType_Locatable;
					break;
				case EOculusXRSpaceComponentType::Sharable:
					componentType = ovrpSpaceComponentType_Sharable;
					break;
				case EOculusXRSpaceComponentType::Storable:
					componentType = ovrpSpaceComponentType_Storable;
					break;
				case EOculusXRSpaceComponentType::ScenePlane:
					componentType = ovrpSpaceComponentType_Bounded2D;
					break;
				case EOculusXRSpaceComponentType::SceneVolume:
					componentType = ovrpSpaceComponentType_Bounded3D;
					break;
				case EOculusXRSpaceComponentType::SemanticClassification:
					componentType = ovrpSpaceComponentType_SemanticLabels;
					break;
				case EOculusXRSpaceComponentType::RoomLayout:
					componentType = ovrpSpaceComponentType_RoomLayout;
					break;
				case EOculusXRSpaceComponentType::SpaceContainer:
					componentType = ovrpSpaceComponentType_SpaceContainer;
					break;
			}

			Result.componentsInfo.components[i] = componentType;
		}

		return Result;
	}

	template <typename T>
	void GetEventData(ovrpEventDataBuffer& Buffer, T& OutEventData)
	{
		unsigned char* BufData = Buffer.EventData;
		BufData -= sizeof(uint64); //correct offset

		memcpy(&OutEventData, BufData, sizeof(T));
	}

	ovrpSpaceComponentType ConvertToOvrpComponentType(const EOculusXRSpaceComponentType ComponentType)
	{
		ovrpSpaceComponentType ovrpType = ovrpSpaceComponentType_Max;
		switch (ComponentType)
		{
			case EOculusXRSpaceComponentType::Locatable:
				ovrpType = ovrpSpaceComponentType_Locatable;
				break;
			case EOculusXRSpaceComponentType::Sharable:
				ovrpType = ovrpSpaceComponentType_Sharable;
				break;
			case EOculusXRSpaceComponentType::Storable:
				ovrpType = ovrpSpaceComponentType_Storable;
				break;
			case EOculusXRSpaceComponentType::ScenePlane:
				ovrpType = ovrpSpaceComponentType_Bounded2D;
				break;
			case EOculusXRSpaceComponentType::SceneVolume:
				ovrpType = ovrpSpaceComponentType_Bounded3D;
				break;
			case EOculusXRSpaceComponentType::SemanticClassification:
				ovrpType = ovrpSpaceComponentType_SemanticLabels;
				break;
			case EOculusXRSpaceComponentType::RoomLayout:
				ovrpType = ovrpSpaceComponentType_RoomLayout;
				break;
			case EOculusXRSpaceComponentType::SpaceContainer:
				ovrpType = ovrpSpaceComponentType_SpaceContainer;
				break;
			default:;
		}

		return ovrpType;
	}

	EOculusXRSpaceComponentType ConvertToUe4ComponentType(const ovrpSpaceComponentType ComponentType)
	{
		EOculusXRSpaceComponentType ue4ComponentType = EOculusXRSpaceComponentType::Undefined;
		switch (ComponentType)
		{
			case ovrpSpaceComponentType_Locatable:
				ue4ComponentType = EOculusXRSpaceComponentType::Locatable;
				break;
			case ovrpSpaceComponentType_Sharable:
				ue4ComponentType = EOculusXRSpaceComponentType::Sharable;
				break;
			case ovrpSpaceComponentType_Storable:
				ue4ComponentType = EOculusXRSpaceComponentType::Storable;
				break;
			case ovrpSpaceComponentType_Bounded2D:
				ue4ComponentType = EOculusXRSpaceComponentType::ScenePlane;
				break;
			case ovrpSpaceComponentType_Bounded3D:
				ue4ComponentType = EOculusXRSpaceComponentType::SceneVolume;
				break;
			case ovrpSpaceComponentType_SemanticLabels:
				ue4ComponentType = EOculusXRSpaceComponentType::SemanticClassification;
				break;
			case ovrpSpaceComponentType_RoomLayout:
				ue4ComponentType = EOculusXRSpaceComponentType::RoomLayout;
				break;
			case ovrpSpaceComponentType_SpaceContainer:
				ue4ComponentType = EOculusXRSpaceComponentType::SpaceContainer;
				break;
			default:;
		}

		return ue4ComponentType;
	}

	void FOculusXRAnchorManager::OnPollEvent(ovrpEventDataBuffer* EventDataBuffer, bool& EventPollResult)
	{
		ovrpEventDataBuffer& buf = *EventDataBuffer;

		switch (buf.EventType)
		{
			case ovrpEventType_None:
				break;
			case ovrpEventType_SpatialAnchorCreateComplete:
			{
				ovrpEventDataSpatialAnchorCreateComplete AnchorCreateEvent;
				GetEventData(buf, AnchorCreateEvent);

				const FOculusXRUInt64 RequestId(AnchorCreateEvent.requestId);
				const FOculusXRUInt64 Space(AnchorCreateEvent.space);
				const FOculusXRUUID BPUUID(AnchorCreateEvent.uuid.data);

				FOculusXRAnchorEventDelegates::OculusSpatialAnchorCreateComplete.Broadcast(RequestId, AnchorCreateEvent.result, Space, BPUUID);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpatialAnchorCreateComplete Request ID: %llu  --  Space: %llu  --  UUID: %s  --  Result: %d"),
					RequestId.GetValue(),
					Space.GetValue(),
					*BPUUID.ToString(),
					AnchorCreateEvent.result);

				break;
			}
			case ovrpEventType_SpaceSetComponentStatusComplete:
			{
				ovrpEventDataSpaceSetStatusComplete SetStatusEvent;
				GetEventData(buf, SetStatusEvent);

				//translate to BP types
				const FOculusXRUInt64 RequestId(SetStatusEvent.requestId);
				const FOculusXRUInt64 Space(SetStatusEvent.space);
				EOculusXRSpaceComponentType BPSpaceComponentType = ConvertToUe4ComponentType(SetStatusEvent.componentType);
				const FOculusXRUUID BPUUID(SetStatusEvent.uuid.data);
				const bool bEnabled = (SetStatusEvent.enabled == ovrpBool_True);

				FOculusXRAnchorEventDelegates::OculusSpaceSetComponentStatusComplete.Broadcast(
					RequestId,
					SetStatusEvent.result,
					Space,
					BPUUID,
					BPSpaceComponentType,
					bEnabled);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceSetComponentStatusComplete Request ID: %llu  --  Type: %d  --  Enabled: %d  --  Space: %llu  --  Result: %d"),
					SetStatusEvent.requestId,
					SetStatusEvent.componentType,
					SetStatusEvent.enabled,
					SetStatusEvent.space,
					SetStatusEvent.result);

				break;
			}
			case ovrpEventType_SpaceQueryResults:
			{
				ovrpEventSpaceQueryResults QueryEvent;
				GetEventData(buf, QueryEvent);

				const FOculusXRUInt64 RequestId(QueryEvent.requestId);

				FOculusXRAnchorEventDelegates::OculusSpaceQueryResults.Broadcast(RequestId);

				ovrpUInt32 ovrpOutCapacity = 0;

				//first get capacity
				const bool bGetCapacityResult = FOculusXRHMDModule::GetPluginWrapper().GetInitialized() && OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().RetrieveSpaceQueryResults(&QueryEvent.requestId, 0, &ovrpOutCapacity, nullptr));

				UE_LOG(LogOculusXRAnchors, Log, TEXT("ovrpEventType_SpaceQueryResults Request ID: %llu  --  Capacity: %d  --  Result: %d"), QueryEvent.requestId, ovrpOutCapacity, bGetCapacityResult);

				std::vector<ovrpSpaceQueryResult> ovrpResults(ovrpOutCapacity);

				// Get Query Data
				const bool bGetQueryDataResult = FOculusXRHMDModule::GetPluginWrapper().GetInitialized() && OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().RetrieveSpaceQueryResults(&QueryEvent.requestId, ovrpResults.size(), &ovrpOutCapacity, ovrpResults.data()));

				for (auto queryResultElement : ovrpResults)
				{
					UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceQueryResult Space: %llu  --  Result: %d"), queryResultElement.space, bGetQueryDataResult);

					//translate types
					FOculusXRUInt64 Space(queryResultElement.space);
					FOculusXRUUID BPUUID(queryResultElement.uuid.data);
					FOculusXRAnchorEventDelegates::OculusSpaceQueryResult.Broadcast(RequestId, Space, BPUUID);
				}

				break;
			}
			case ovrpEventType_SpaceQueryComplete:
			{
				ovrpEventSpaceQueryComplete QueryCompleteEvent;
				GetEventData(buf, QueryCompleteEvent);

				//translate to BP types
				const FOculusXRUInt64 RequestId(QueryCompleteEvent.requestId);
				const bool bSucceeded = QueryCompleteEvent.result >= 0;

				FOculusXRAnchorEventDelegates::OculusSpaceQueryComplete.Broadcast(RequestId, QueryCompleteEvent.result);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceQueryComplete Request ID: %llu  --  Result: %d"), QueryCompleteEvent.requestId, QueryCompleteEvent.result);

				break;
			}
			case ovrpEventType_SpaceSaveComplete:
			{
				ovrpEventSpaceStorageSaveResult StorageResult;
				GetEventData(buf, StorageResult);

				//translate to BP types
				const FOculusXRUUID uuid(StorageResult.uuid.data);
				const FOculusXRUInt64 FSpace(StorageResult.space);
				const FOculusXRUInt64 FRequest(StorageResult.requestId);
				const bool bResult = StorageResult.result >= 0;

				FOculusXRAnchorEventDelegates::OculusSpaceSaveComplete.Broadcast(FRequest, FSpace, bResult, StorageResult.result, uuid);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceSaveComplete  Request ID: %llu  --  Space: %llu  --  Result: %d"), StorageResult.requestId, StorageResult.space, StorageResult.result);

				break;
			}
			case ovrpEventType_SpaceListSaveResult:
			{
				ovrpEventSpaceListSaveResult SpaceListSaveResult;
				GetEventData(buf, SpaceListSaveResult);

				FOculusXRUInt64 RequestId(SpaceListSaveResult.requestId);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceListSaveResult  Request ID: %llu  --  Result: %d"), SpaceListSaveResult.requestId, SpaceListSaveResult.result);
				FOculusXRAnchorEventDelegates::OculusSpaceListSaveComplete.Broadcast(RequestId, SpaceListSaveResult.result);

				break;
			}
			case ovrpEventType_SpaceEraseComplete:
			{
				ovrpEventSpaceStorageEraseResult SpaceEraseEvent;
				GetEventData(buf, SpaceEraseEvent);

				//translate to BP types
				const FOculusXRUUID uuid(SpaceEraseEvent.uuid.data);
				const FOculusXRUInt64 FRequestId(SpaceEraseEvent.requestId);
				const FOculusXRUInt64 FResult(SpaceEraseEvent.result);
				const EOculusXRSpaceStorageLocation BPLocation = (SpaceEraseEvent.location == ovrpSpaceStorageLocation_Local) ? EOculusXRSpaceStorageLocation::Local : EOculusXRSpaceStorageLocation::Invalid;

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceEraseComplete  Request ID: %llu  --  Result: %d  -- UUID: %s"), SpaceEraseEvent.requestId, SpaceEraseEvent.result, *UOculusXRAnchorBPFunctionLibrary::AnchorUUIDToString(SpaceEraseEvent.uuid.data));

				FOculusXRAnchorEventDelegates::OculusSpaceEraseComplete.Broadcast(FRequestId, FResult.Value, uuid, BPLocation);
				break;
			}
			case ovrpEventType_SpaceShareResult:
			{
				unsigned char* BufData = buf.EventData;
				ovrpUInt64 OvrpRequestId = 0;
				memcpy(&OvrpRequestId, BufData, sizeof(OvrpRequestId));

				ovrpEventSpaceShareResult SpaceShareSpaceResult;
				GetEventData(buf, SpaceShareSpaceResult);

				FOculusXRUInt64 RequestId(SpaceShareSpaceResult.requestId);

				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ovrpEventType_SpaceShareSpaceResult  Request ID: %llu  --  Result: %d"),
					SpaceShareSpaceResult.requestId,
					SpaceShareSpaceResult.result);

				FOculusXRAnchorEventDelegates::OculusSpaceShareComplete.Broadcast(RequestId, SpaceShareSpaceResult.result);

				break;
			}
			default:
			{
				EventPollResult = false;
				break;
			}
		}

		EventPollResult = true;
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform)
	{
		bool bValidHMD;
		OculusXRHMD::FOculusXRHMD* HMD = GetHMD(bValidHMD);
		if (!bValidHMD)
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("FOculusAnchorManager::CreateAnchor failed to retrieve HMD."));
			return EOculusXRAnchorResult::Failure;
		}

		ovrpTrackingOrigin TrackingOriginType;
		ovrpPosef Posef;
		double Time = 0;

		const FTransform TrackingToWorld = HMD->GetLastTrackingToWorld();

		// convert to tracking space
		const FQuat TrackingSpaceOrientation = TrackingToWorld.Inverse().TransformRotation(InTransform.Rotator().Quaternion());
		const FVector TrackingSpacePosition = TrackingToWorld.Inverse().TransformPosition(InTransform.GetLocation());

		const OculusXRHMD::FPose TrackingSpacePose(TrackingSpaceOrientation, TrackingSpacePosition);

#if WITH_EDITOR
		// Link only head space position update
		FVector OutHeadPosition;
		FQuat OutHeadOrientation;
		const bool bGetPose = HMD->GetCurrentPose(HMD->HMDDeviceId, OutHeadOrientation, OutHeadPosition);
		if (!bGetPose)
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("FOculusAnchorManager::CreateAnchor failed to get current headset pose."));
			return EOculusXRAnchorResult::Failure;
		}

		OculusXRHMD::FPose HeadPose(OutHeadOrientation, OutHeadPosition);

		OculusXRHMD::FPose MainCameraPose(CameraTransform.GetRotation(), CameraTransform.GetLocation());
		OculusXRHMD::FPose PoseInHeadSpace = MainCameraPose.Inverse() * TrackingSpacePose;

		// To world space pose
		OculusXRHMD::FPose WorldPose = HeadPose * PoseInHeadSpace;

		const bool bConverted = HMD->ConvertPose(WorldPose, Posef);
#else
		const bool bConverted = HMD->ConvertPose(TrackingSpacePose, Posef);
#endif

		if (!bConverted)
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("FOculusAnchorManager::CreateAnchor failed to convert pose."));
			return EOculusXRAnchorResult::Failure;
		}

		FOculusXRHMDModule::GetPluginWrapper().GetTrackingOriginType2(&TrackingOriginType);
		FOculusXRHMDModule::GetPluginWrapper().GetTimeInSeconds(&Time);

		const ovrpSpatialAnchorCreateInfo SpatialAnchorCreateInfo = {
			TrackingOriginType,
			Posef,
			Time
		};

		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().CreateSpatialAnchor(&SpatialAnchorCreateInfo, &OutRequestId);
		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("CreateAnchor Request ID: %llu"), OutRequestId);

		if (!OVRP_SUCCESS(Result))
		{
			UE_LOG(LogOculusXRAnchors, Error, TEXT("FOculusAnchorManager::CreateAnchor failed. Result: %d"), Result);
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::DestroySpace(uint64 Space)
	{
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().DestroySpace(static_cast<ovrpSpace*>(&Space));

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("DestroySpace  Space ID: %llu"), Space);

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SetSpaceComponentStatus(uint64 Space, EOculusXRSpaceComponentType SpaceComponentType, bool Enable, float Timeout, uint64& OutRequestId)
	{
		ovrpSpaceComponentType ovrpType = ConvertToOvrpComponentType(SpaceComponentType);
		ovrpUInt64 OvrpOutRequestId = 0;

		const ovrpUInt64 OVRPSpace = Space;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().SetSpaceComponentStatus(
			&OVRPSpace,
			ovrpType,
			Enable,
			Timeout,
			&OvrpOutRequestId);

		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("SetSpaceComponentStatus  Request ID: %llu"), OutRequestId);

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceComponentStatus(uint64 Space, EOculusXRSpaceComponentType SpaceComponentType, bool& OutEnabled, bool& OutChangePending)
	{
		const ovrpUInt64 OVRPSpace = Space;
		ovrpBool OutOvrpEnabled = ovrpBool_False;
		ovrpBool OutOvrpChangePending = ovrpBool_False;

		ovrpSpaceComponentType ovrpType = ConvertToOvrpComponentType(SpaceComponentType);

		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceComponentStatus(
			&OVRPSpace,
			ovrpType,
			&OutOvrpEnabled,
			&OutOvrpChangePending);

		OutEnabled = (OutOvrpEnabled == ovrpBool_True);
		OutChangePending = (OutOvrpChangePending == ovrpBool_True);

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SaveAnchor(uint64 Space,
		EOculusXRSpaceStorageLocation StorageLocation,
		EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId)
	{
		ovrpSpaceStorageLocation OvrpStorageLocation = ovrpSpaceStorageLocation_Local;
		switch (StorageLocation)
		{
			case EOculusXRSpaceStorageLocation::Invalid:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Invalid;
				break;
			case EOculusXRSpaceStorageLocation::Local:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Local;
				break;
			case EOculusXRSpaceStorageLocation::Cloud:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Cloud;
				break;
			default:
				break;
		}

		ovrpSpaceStoragePersistenceMode OvrpStoragePersistenceMode = ovrpSpaceStoragePersistenceMode_Invalid;
		switch (StoragePersistenceMode)
		{
			case EOculusXRSpaceStoragePersistenceMode::Invalid:
				OvrpStoragePersistenceMode = ovrpSpaceStoragePersistenceMode_Invalid;
				break;
			case EOculusXRSpaceStoragePersistenceMode::Indefinite:
				OvrpStoragePersistenceMode = ovrpSpaceStoragePersistenceMode_Indefinite;
				break;
			default:
				break;
		}

		ovrpUInt64 OvrpOutRequestId = 0;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().SaveSpace(&Space, OvrpStorageLocation, OvrpStoragePersistenceMode, &OvrpOutRequestId);

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Saving space with: SpaceID: %llu  --  Location: %d  --  Persistence: %d  --  OutID: %llu"), Space, OvrpStorageLocation, OvrpStoragePersistenceMode, OvrpOutRequestId);

		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		if (!OVRP_SUCCESS(Result))
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("FOculusXRHMD::SaveAnchor failed with: SpaceID: %llu  --  Location: %d  --  Persistence: %d"), Space, OvrpStorageLocation, OvrpStoragePersistenceMode);
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SaveAnchorList(const TArray<uint64>& Spaces, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
	{
		ovrpSpaceStorageLocation OvrpStorageLocation = ovrpSpaceStorageLocation_Local;
		switch (StorageLocation)
		{
			case EOculusXRSpaceStorageLocation::Invalid:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Invalid;
				break;
			case EOculusXRSpaceStorageLocation::Local:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Local;
				break;
			case EOculusXRSpaceStorageLocation::Cloud:
				OvrpStorageLocation = ovrpSpaceStorageLocation_Cloud;
				break;
			default:
				break;
		}

		ovrpUInt64 OvrpOutRequestId = 0;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().SaveSpaceList(Spaces.GetData(), Spaces.Num(), OvrpStorageLocation, &OvrpOutRequestId);

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Saving space list: Location: %d --  OutID: %llu"), OvrpStorageLocation, OvrpOutRequestId);
		for (auto& it : Spaces)
		{
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("\tSpaceID: %llu"), it);
		}

		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		if (!OVRP_SUCCESS(Result))
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("SaveSpaceList failed -- Result: %d"), Result);
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::EraseAnchor(uint64 AnchorHandle,
		EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
	{
		ovrpSpaceStorageLocation ovrpStorageLocation = ovrpSpaceStorageLocation_Local;
		switch (StorageLocation)
		{
			case EOculusXRSpaceStorageLocation::Invalid:
				ovrpStorageLocation = ovrpSpaceStorageLocation_Invalid;
				break;
			case EOculusXRSpaceStorageLocation::Local:
				ovrpStorageLocation = ovrpSpaceStorageLocation_Local;
				break;
			default:;
		}

		ovrpUInt64 OvrpOutRequestId = 0;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().EraseSpace(&AnchorHandle, ovrpStorageLocation, &OvrpOutRequestId);
		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		UE_LOG(LogOculusXRAnchors, Log, TEXT("Erasing anchor -- Handle: %llu  --  Location: %d  --  OutID: %llu"), AnchorHandle, ovrpStorageLocation, OvrpOutRequestId);

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::QuerySpaces(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId)
	{
		ovrpSpaceQueryInfo ovrQueryInfo = ConvertToOVRPSpaceQueryInfo(QueryInfo);

		ovrpUInt64 OvrpOutRequestId = 0;
		const ovrpResult QuerySpacesResult = FOculusXRHMDModule::GetPluginWrapper().QuerySpaces(&ovrQueryInfo, &OvrpOutRequestId);
		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Query Spaces\n ovrpSpaceQueryInfo:\n\tQueryType: %d\n\tMaxQuerySpaces: %d\n\tTimeout: %f\n\tLocation: %d\n\tActionType: %d\n\tFilterType: %d\n\n\tRequest ID: %llu"),
			ovrQueryInfo.queryType, ovrQueryInfo.maxQuerySpaces, (float)ovrQueryInfo.timeout, ovrQueryInfo.location, ovrQueryInfo.actionType, ovrQueryInfo.filterType, OutRequestId);

		if (QueryInfo.FilterType == EOculusXRSpaceQueryFilterType::FilterByIds)
		{
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Query contains %d UUIDs"), QueryInfo.IDFilter.Num());
			for (auto& it : QueryInfo.IDFilter)
			{
				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("UUID: %s"), *it.ToString());
			}
		}
		else if (QueryInfo.FilterType == EOculusXRSpaceQueryFilterType::FilterByComponentType)
		{
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Query contains %d Component Types"), QueryInfo.ComponentFilter.Num());
			for (auto& it : QueryInfo.ComponentFilter)
			{
				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("ComponentType: %s"), *UEnum::GetValueAsString(it));
			}
		}

		return static_cast<EOculusXRAnchorResult::Type>(QuerySpacesResult);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::ShareSpaces(const TArray<uint64>& Spaces, const TArray<uint64>& UserIds, uint64& OutRequestId)
	{
		TArray<const char*> stringStorage;
		TArray<ovrpUser> OvrpUsers;
		for (const auto& UserId : UserIds)
		{
			ovrpUser OvrUser;
			ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().CreateSpaceUser(&UserId, &OvrUser);
			if (!OVRP_SUCCESS(Result))
			{
				UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to create space user from ID  -  %llu"), UserId);
				continue;
			}

			OvrpUsers.Add(OvrUser);
		}

		ovrpUInt64 OvrpOutRequestId = 0;
		const ovrpResult ShareSpacesResult = FOculusXRHMDModule::GetPluginWrapper().ShareSpaces(Spaces.GetData(), Spaces.Num(), OvrpUsers.GetData(), OvrpUsers.Num(), &OvrpOutRequestId);

		UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Sharing space list  --  OutID: %llu"), OvrpOutRequestId);
		for (auto& User : OvrpUsers)
		{
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("\tOvrpUser: %llu"), User);
			ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().DestroySpaceUser(&User);
			if (!OVRP_SUCCESS(Result))
			{
				UE_LOG(LogOculusXRAnchors, Verbose, TEXT("Failed to destroy space user: %llu"), User);
				continue;
			}
		}

		for (auto& it : Spaces)
		{
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("\tSpaceID: %llu"), it);
		}

		memcpy(&OutRequestId, &OvrpOutRequestId, sizeof(uint64));

		return static_cast<EOculusXRAnchorResult::Type>(ShareSpacesResult);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceContainerUUIDs(uint64 Space, TArray<FOculusXRUUID>& OutUUIDs)
	{
		TArray<ovrpUuid> ovrUuidArray;

		// Get the number of elements in the container
		ovrpSpaceContainer ovrSpaceContainer;
		ovrSpaceContainer.uuidCapacityInput = 0;
		ovrSpaceContainer.uuidCountOutput = 0;
		ovrSpaceContainer.uuids = nullptr;
		ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceContainer(&Space, &ovrSpaceContainer);
		if (!OVRP_SUCCESS(result))
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to get space container %d"), result);
			return static_cast<EOculusXRAnchorResult::Type>(result);
		}

		// Retrieve the actual array of UUIDs
		ovrUuidArray.SetNum(ovrSpaceContainer.uuidCountOutput);
		ovrSpaceContainer.uuidCapacityInput = ovrSpaceContainer.uuidCountOutput;
		ovrSpaceContainer.uuids = ovrUuidArray.GetData();

		result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceContainer(&Space, &ovrSpaceContainer);
		if (!OVRP_SUCCESS(result))
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to get space container %d"), result);
			return static_cast<EOculusXRAnchorResult::Type>(result);
		}

		// Write out the remaining UUIDs
		OutUUIDs.Reserve(ovrUuidArray.Num());
		for (auto& it : ovrUuidArray)
		{
			OutUUIDs.Add(FOculusXRUUID(it.data));
		}

		return EOculusXRAnchorResult::Success;
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceScenePlane(uint64 Space, FVector& OutPos, FVector& OutSize)
	{
		OutPos.X = OutPos.Y = OutPos.Z = 0.f;
		OutSize.X = OutSize.Y = OutSize.Z = 0.f;

		ovrpRectf rect;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundingBox2D(&Space, &rect);

		if (OVRP_SUCCESS(Result))
		{
			// Convert to UE4's coordinates system
			OutPos.Y = rect.Pos.x;
			OutPos.Z = rect.Pos.y;
			OutSize.Y = rect.Size.w;
			OutSize.Z = rect.Size.h;
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceSceneVolume(uint64 Space, FVector& OutPos, FVector& OutSize)
	{
		OutPos.X = OutPos.Y = OutPos.Z = 0.f;
		OutSize.X = OutSize.Y = OutSize.Z = 0.f;

		ovrpBoundsf bounds;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundingBox3D(&Space, &bounds);

		if (OVRP_SUCCESS(Result))
		{
			// Convert to UE4's coordinates system
			OutPos.X = bounds.Pos.z;
			OutPos.Y = bounds.Pos.x;
			OutPos.Z = bounds.Pos.y;
			OutSize.X = bounds.Size.d;
			OutSize.Y = bounds.Size.w;
			OutSize.Z = bounds.Size.h;
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceSemanticClassification(uint64 Space, TArray<FString>& OutSemanticClassifications)
	{
		OutSemanticClassifications.Empty();

		const int32 maxByteSize = 1024;
		char labelsChars[maxByteSize];

		ovrpSemanticLabels labels;
		labels.byteCapacityInput = maxByteSize;
		labels.labels = labelsChars;

		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceSemanticLabels(&Space, &labels);

		if (OVRP_SUCCESS(Result))
		{
			FString labelsStr(labels.byteCountOutput, labels.labels);
			labelsStr.ParseIntoArray(OutSemanticClassifications, TEXT(","));
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSpaceContainer(uint64 Space, TArray<FOculusXRUUID>& OutContainerUuids)
	{
		OutContainerUuids.Empty();

		ovrpSpaceContainer container;

		ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceContainer(&Space, &container);

		if (OVRP_SUCCESS(Result))
		{
			TArray<ovrpUuid> uuids;
			size_t size = container.uuidCountOutput;
			uuids.InsertZeroed(0, size);
			container.uuidCapacityInput = size;
			container.uuids = uuids.GetData();
			Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceContainer(&Space, &container);
			if (OVRP_SUCCESS(Result))
			{
				OutContainerUuids.InsertZeroed(0, size);
				for (size_t i = 0; i < size; i++)
				{
					OutContainerUuids[i] = FOculusXRUUID(uuids[i].data);
				}
			}
		}

		return static_cast<EOculusXRAnchorResult::Type>(Result);
	}
} // namespace OculusXRAnchors
