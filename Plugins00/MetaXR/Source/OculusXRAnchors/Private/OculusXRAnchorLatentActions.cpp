/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.

This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRAnchorLatentActions.h"
#include "OculusXRAnchorsPrivate.h"
#include "OculusXRHMD.h"
#include "OculusXRAnchorBPFunctionLibrary.h"

//
// Create Spatial Anchor
//
void UOculusXRAsyncAction_CreateSpatialAnchor::Activate()
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Invalid Target Actor passed to CreateSpatialAnchor latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::CreateSpatialAnchor(
		AnchorTransform,
		TargetActor,
		FOculusXRSpatialAnchorCreateDelegate::CreateUObject(this, &UOculusXRAsyncAction_CreateSpatialAnchor::HandleCreateComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for CreateSpatialAnchor latent action."));
		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_CreateSpatialAnchor* UOculusXRAsyncAction_CreateSpatialAnchor::OculusXRAsyncCreateSpatialAnchor(AActor* TargetActor, const FTransform& AnchorTransform)
{
	UOculusXRAsyncAction_CreateSpatialAnchor* Action = NewObject<UOculusXRAsyncAction_CreateSpatialAnchor>();
	Action->TargetActor = TargetActor;
	Action->AnchorTransform = AnchorTransform;

	if (IsValid(TargetActor))
	{
		Action->RegisterWithGameInstance(TargetActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UOculusXRAsyncAction_CreateSpatialAnchor::HandleCreateComplete(EOculusXRAnchorResult::Type CreateResult, UOculusXRAnchorComponent* Anchor)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(CreateResult))
	{
		Success.Broadcast(Anchor, CreateResult);
	}
	else
	{
		Failure.Broadcast(CreateResult);
	}

	SetReadyToDestroy();
}

//
// Erase Space
//
void UOculusXRAsyncAction_EraseAnchor::Activate()
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Invalid Target Actor passed to EraseSpace latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	UOculusXRAnchorComponent* AnchorComponent = TargetActor->FindComponentByClass<UOculusXRAnchorComponent>();
	if (AnchorComponent == nullptr)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("No anchor on actor in EraseSpace latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::EraseAnchor(
		AnchorComponent,
		FOculusXRAnchorEraseDelegate::CreateUObject(this, &UOculusXRAsyncAction_EraseAnchor::HandleEraseAnchorComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for EraseSpace latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_EraseAnchor* UOculusXRAsyncAction_EraseAnchor::OculusXRAsyncEraseAnchor(AActor* TargetActor)
{
	UOculusXRAsyncAction_EraseAnchor* Action = NewObject<UOculusXRAsyncAction_EraseAnchor>();
	Action->TargetActor = TargetActor;

	if (IsValid(TargetActor))
	{
		Action->RegisterWithGameInstance(TargetActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UOculusXRAsyncAction_EraseAnchor::HandleEraseAnchorComplete(EOculusXRAnchorResult::Type EraseResult, FOculusXRUUID UUID)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(EraseResult))
	{
		Success.Broadcast(TargetActor, UUID, EraseResult);
	}
	else
	{
		Failure.Broadcast(EraseResult);
	}

	SetReadyToDestroy();
}

//
// Save Space
//
void UOculusXRAsyncAction_SaveAnchor::Activate()
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Invalid Target Actor passed to SaveSpace latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	UOculusXRAnchorComponent* AnchorComponent = TargetActor->FindComponentByClass<UOculusXRAnchorComponent>();
	if (AnchorComponent == nullptr)
	{
		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	UE_LOG(LogOculusXRAnchors, Log, TEXT("Attempting to save anchor: %s to location %s"), IsValid(AnchorComponent) ? *AnchorComponent->GetName() : TEXT("INVALID ANCHOR"), *UEnum::GetValueAsString(StorageLocation));

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::SaveAnchor(
		AnchorComponent,
		StorageLocation,
		FOculusXRAnchorSaveDelegate::CreateUObject(this, &UOculusXRAsyncAction_SaveAnchor::HandleSaveAnchorComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for SaveSpace latent action."));
		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_SaveAnchor* UOculusXRAsyncAction_SaveAnchor::OculusXRAsyncSaveAnchor(AActor* TargetActor, EOculusXRSpaceStorageLocation StorageLocation)
{
	UOculusXRAsyncAction_SaveAnchor* Action = NewObject<UOculusXRAsyncAction_SaveAnchor>();
	Action->TargetActor = TargetActor;
	Action->StorageLocation = StorageLocation;

	if (IsValid(TargetActor))
	{
		Action->RegisterWithGameInstance(TargetActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UOculusXRAsyncAction_SaveAnchor::HandleSaveAnchorComplete(EOculusXRAnchorResult::Type SaveResult, UOculusXRAnchorComponent* Anchor)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(SaveResult))
	{
		Success.Broadcast(Anchor, SaveResult);
	}
	else
	{
		Failure.Broadcast(SaveResult);
	}

	SetReadyToDestroy();
}

//
// Save Spaces
//
void UOculusXRAsyncAction_SaveAnchors::Activate()
{
	if (TargetAnchors.Num() == 0)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Empty Target Actor array passed to SaveSpaces latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::SaveAnchorList(
		TargetAnchors,
		StorageLocation,
		FOculusXRAnchorSaveListDelegate::CreateUObject(this, &UOculusXRAsyncAction_SaveAnchors::HandleSaveAnchorsComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for SaveSpaceList latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_SaveAnchors* UOculusXRAsyncAction_SaveAnchors::OculusXRAsyncSaveAnchors(const TArray<AActor*>& TargetActors, EOculusXRSpaceStorageLocation StorageLocation)
{
	UOculusXRAsyncAction_SaveAnchors* Action = NewObject<UOculusXRAsyncAction_SaveAnchors>();

	auto ValidActorPtr = TargetActors.FindByPredicate([](AActor* Actor) { return IsValid(Actor); });

	for (auto& it : TargetActors)
	{
		if (!IsValid(it))
		{
			continue;
		}

		UOculusXRAnchorComponent* AnchorComponent = it->FindComponentByClass<UOculusXRAnchorComponent>();
		Action->TargetAnchors.Add(AnchorComponent);
	}

	Action->StorageLocation = StorageLocation;

	if (ValidActorPtr != nullptr)
	{
		Action->RegisterWithGameInstance(*ValidActorPtr);
	}

	return Action;
}

void UOculusXRAsyncAction_SaveAnchors::HandleSaveAnchorsComplete(EOculusXRAnchorResult::Type SaveResult, const TArray<UOculusXRAnchorComponent*>& SavedSpaces)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(SaveResult))
	{
		Success.Broadcast(SavedSpaces, SaveResult);
	}
	else
	{
		Failure.Broadcast(SaveResult);
	}

	SetReadyToDestroy();
}

//
// Query Spaces
//
void UOculusXRAsyncAction_QueryAnchors::Activate()
{
	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::QueryAnchorsAdvanced(
		QueryInfo,
		FOculusXRAnchorQueryDelegate::CreateUObject(this, &UOculusXRAsyncAction_QueryAnchors::HandleQueryAnchorsResults),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for QuerySpaces latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_QueryAnchors* UOculusXRAsyncAction_QueryAnchors::OculusXRAsyncQueryAnchors(EOculusXRSpaceStorageLocation Location, const TArray<FOculusXRUUID>& UUIDs)
{
	FOculusXRSpaceQueryInfo QueryInfo;
	QueryInfo.FilterType = EOculusXRSpaceQueryFilterType::FilterByIds;
	QueryInfo.IDFilter = UUIDs;
	QueryInfo.Location = Location;
	QueryInfo.MaxQuerySpaces = UUIDs.Num();

	UOculusXRAsyncAction_QueryAnchors* Action = NewObject<UOculusXRAsyncAction_QueryAnchors>();
	Action->QueryInfo = QueryInfo;

	return Action;
}

UOculusXRAsyncAction_QueryAnchors* UOculusXRAsyncAction_QueryAnchors::OculusXRAsyncQueryAnchorsAdvanced(const FOculusXRSpaceQueryInfo& QueryInfo)
{
	UOculusXRAsyncAction_QueryAnchors* Action = NewObject<UOculusXRAsyncAction_QueryAnchors>();
	Action->QueryInfo = QueryInfo;

	return Action;
}

void UOculusXRAsyncAction_QueryAnchors::HandleQueryAnchorsResults(EOculusXRAnchorResult::Type QueryResult, const TArray<FOculusXRSpaceQueryResult>& Results)
{
	QueryResults = Results;

	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(QueryResult))
	{
		Success.Broadcast(QueryResults, QueryResult);
	}
	else
	{
		Failure.Broadcast(QueryResult);
	}

	SetReadyToDestroy();
}

//
// Set Component Status
//
void UOculusXRAsyncAction_SetAnchorComponentStatus::Activate()
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Invalid Target Actor passed to SetComponentStatus latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	UOculusXRAnchorComponent* AnchorComponent = TargetActor->FindComponentByClass<UOculusXRAnchorComponent>();
	if (AnchorComponent == nullptr)
	{
		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::SetAnchorComponentStatus(
		AnchorComponent,
		ComponentType,
		bEnabled,
		0,
		FOculusXRAnchorSetComponentStatusDelegate::CreateUObject(this, &UOculusXRAsyncAction_SetAnchorComponentStatus::HandleSetComponentStatusComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for SetComponentStatus latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_SetAnchorComponentStatus* UOculusXRAsyncAction_SetAnchorComponentStatus::OculusXRAsyncSetAnchorComponentStatus(AActor* TargetActor, EOculusXRSpaceComponentType ComponentType, bool bEnabled)
{
	UOculusXRAsyncAction_SetAnchorComponentStatus* Action = NewObject<UOculusXRAsyncAction_SetAnchorComponentStatus>();
	Action->TargetActor = TargetActor;
	Action->ComponentType = ComponentType;
	Action->bEnabled = bEnabled;

	if (IsValid(TargetActor))
	{
		Action->RegisterWithGameInstance(TargetActor->GetWorld());
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UOculusXRAsyncAction_SetAnchorComponentStatus::HandleSetComponentStatusComplete(EOculusXRAnchorResult::Type SetStatusResult, UOculusXRAnchorComponent* Anchor, EOculusXRSpaceComponentType SpaceComponentType, bool bResultEnabled)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(SetStatusResult))
	{
		Success.Broadcast(Anchor, SpaceComponentType, bResultEnabled, SetStatusResult);
	}
	else
	{
		Failure.Broadcast(SetStatusResult);
	}

	SetReadyToDestroy();
}

//
// Set Scene Component Status
//
void UOculusXRAsyncAction_SetComponentStatus::Activate()
{
	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::SetComponentStatus(
		Component->GetSpace(),
		Component->GetType(),
		bEnabled,
		0,
		FOculusXRAnchorSetAnchorComponentStatusDelegate::CreateUObject(this, &UOculusXRAsyncAction_SetComponentStatus::HandleSetComponentStatusComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for SetComponentStatus latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_SetComponentStatus* UOculusXRAsyncAction_SetComponentStatus::OculusXRAsyncSetComponentStatus(UOculusXRBaseAnchorComponent* Component, bool bEnabled)
{
	UOculusXRAsyncAction_SetComponentStatus* Action = NewObject<UOculusXRAsyncAction_SetComponentStatus>();
	Action->Component = Component;
	Action->bEnabled = bEnabled;

	Action->RegisterWithGameInstance(GWorld);

	return Action;
}

void UOculusXRAsyncAction_SetComponentStatus::HandleSetComponentStatusComplete(EOculusXRAnchorResult::Type SetStatusResult, uint64 Space, EOculusXRSpaceComponentType SpaceComponentType)
{
	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(SetStatusResult))
	{
		Success.Broadcast(Component, SetStatusResult);
	}
	else
	{
		Failure.Broadcast(SetStatusResult);
	}

	SetReadyToDestroy();
}

//
// Share Spaces
//
void UOculusXRAsyncAction_ShareAnchors::Activate()
{
	if (TargetAnchors.Num() == 0)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Empty Target Actors array passed to ShareSpaces latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	if (ToShareWithIds.Num() == 0)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Empty Target Player IDs array passed to ShareSpaces latent action."));

		Failure.Broadcast(EOculusXRAnchorResult::Failure);
		return;
	}

	EOculusXRAnchorResult::Type Result;
	bool bStartedAsync = OculusXRAnchors::FOculusXRAnchors::ShareAnchors(
		TargetAnchors,
		ToShareWithIds,
		FOculusXRAnchorShareDelegate::CreateUObject(this, &UOculusXRAsyncAction_ShareAnchors::HandleShareAnchorsComplete),
		Result);

	if (!bStartedAsync)
	{
		UE_LOG(LogOculusXRAnchors, Warning, TEXT("Failed to start async OVR Plugin call for ShareSpaces latent action."));

		Failure.Broadcast(Result);
	}
}

UOculusXRAsyncAction_ShareAnchors* UOculusXRAsyncAction_ShareAnchors::OculusXRAsyncShareAnchors(const TArray<AActor*>& TargetActors, const TArray<FString>& ToShareWithIds)
{
	UOculusXRAsyncAction_ShareAnchors* Action = NewObject<UOculusXRAsyncAction_ShareAnchors>();

	for (const auto& UserIDString : ToShareWithIds)
	{
		uint64 UserId = FCString::Strtoui64(*UserIDString, nullptr, 10);
		if (UserId == 0)
		{
			UE_LOG(LogOculusXRAnchors, Warning, TEXT("UserID provided to share anchors was invalid or unconvertable: %s"), *UserIDString);
		}

		Action->ToShareWithIds.Add(UserId);
	}

	for (auto& it : TargetActors)
	{
		if (!IsValid(it))
		{
			continue;
		}

		UOculusXRAnchorComponent* AnchorComponent = it->FindComponentByClass<UOculusXRAnchorComponent>();
		Action->TargetAnchors.Add(AnchorComponent);
	}

	auto ValidActorPtr = TargetActors.FindByPredicate([](AActor* Actor) { return IsValid(Actor); });
	if (ValidActorPtr != nullptr)
	{
		Action->RegisterWithGameInstance(*ValidActorPtr);
	}
	else
	{
		Action->RegisterWithGameInstance(GWorld);
	}

	return Action;
}

void UOculusXRAsyncAction_ShareAnchors::HandleShareAnchorsComplete(EOculusXRAnchorResult::Type ShareResult, const TArray<UOculusXRAnchorComponent*>& SharedAnchors, const TArray<uint64>& OculusUserIDs)
{
	TArray<FString> OculusUserIDStrings;
	for (const auto& it : OculusUserIDs)
	{
		OculusUserIDStrings.Add(FString::Printf(TEXT("%llu"), it));
	}

	if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(ShareResult))
	{
		Success.Broadcast(SharedAnchors, OculusUserIDStrings, ShareResult);
	}
	else
	{
		Failure.Broadcast(ShareResult);
	}

	// Unbind and mark for destruction
	SetReadyToDestroy();
}
