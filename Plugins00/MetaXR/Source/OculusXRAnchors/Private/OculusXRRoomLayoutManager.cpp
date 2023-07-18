/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/
#include "OculusXRRoomLayoutManager.h"
#include "OculusXRHMD.h"
#include "OculusXRAnchorDelegates.h"
#include "OculusXRAnchorsModule.h"

namespace OculusXRAnchors
{
	void FOculusXRRoomLayoutManager::OnPollEvent(ovrpEventDataBuffer* EventDataBuffer, bool& EventPollResult)
	{
		ovrpEventDataBuffer& buf = *EventDataBuffer;

		switch (buf.EventType)
		{
			case ovrpEventType_None:
				break;
			case ovrpEventType_SceneCaptureComplete:
			{
				ovrpEventSceneCaptureComplete sceneCaptureComplete;
				unsigned char* bufData = buf.EventData;

				memcpy(&sceneCaptureComplete.requestId, bufData, sizeof(sceneCaptureComplete.requestId));
				bufData += sizeof(ovrpUInt64); //move forward
				memcpy(&sceneCaptureComplete.result, bufData, sizeof(sceneCaptureComplete.result));

				FOculusXRAnchorEventDelegates::OculusSceneCaptureComplete.Broadcast(FOculusXRUInt64(sceneCaptureComplete.requestId), sceneCaptureComplete.result >= 0);
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

	/**
	 * @brief Requests the launch of Capture Flow
	 * @param OutRequestID The requestId returned by the system
	 * @return returns true if sucessfull
	 */
	bool FOculusXRRoomLayoutManager::RequestSceneCapture(uint64& OutRequestID)
	{
		OutRequestID = 0;

		ovrpSceneCaptureRequest sceneCaptureRequest;
		sceneCaptureRequest.request = nullptr;
		sceneCaptureRequest.requestByteCount = 0;

		const ovrpResult bSuccess = FOculusXRHMDModule::GetPluginWrapper().RequestSceneCapture(&sceneCaptureRequest, &OutRequestID);
		if (bSuccess == ovrpFailure)
		{
			return false;
		}

		return true;
	}

	/**
	 * @brief Gets the room layout for a specific space
	 * @param Space The space to get the room layout for
	 * @param MaxWallsCapacity Maximum number of walls to query
	 * @param OutCeilingUuid The ceiling entity's uuid
	 * @param OutFloorUuid The floor entity's uuid
	 * @param OutWallsUuid Array of uuids belonging to the walls in the room layout
	 * @return returns true if sucessfull
	 */
	bool FOculusXRRoomLayoutManager::GetSpaceRoomLayout(const uint64 Space, const uint32 MaxWallsCapacity,
		FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid)
	{
		TArray<ovrpUuid> uuids;
		uuids.InsertZeroed(0, MaxWallsCapacity);

		ovrpRoomLayout roomLayout;
		roomLayout.wallUuidCapacityInput = MaxWallsCapacity;
		roomLayout.wallUuids = uuids.GetData();

		const ovrpResult bSuccess = FOculusXRHMDModule::GetPluginWrapper().GetSpaceRoomLayout(&Space, &roomLayout);
		if (bSuccess == ovrpFailure)
		{
			return false;
		}

		OutCeilingUuid = FOculusXRUUID(roomLayout.ceilingUuid.data);
		OutFloorUuid = FOculusXRUUID(roomLayout.floorUuid.data);

		OutWallsUuid.Empty();
		OutWallsUuid.InsertZeroed(0, roomLayout.wallUuidCountOutput);

		for (int32 i = 0; i < roomLayout.wallUuidCountOutput; ++i)
		{
			OutWallsUuid[i] = FOculusXRUUID(roomLayout.wallUuids[i].data);
		}

		return true;
	}
} // namespace OculusXRAnchors
