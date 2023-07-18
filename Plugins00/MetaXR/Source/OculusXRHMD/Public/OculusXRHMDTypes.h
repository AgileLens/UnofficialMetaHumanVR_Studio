// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RHI.h"
#include "RHIResources.h"
#include "Engine/Texture2D.h"
#include "UObject/SoftObjectPath.h"
#include "OculusXRHMDTypes.generated.h"

USTRUCT()
struct FOculusXRSplashDesc
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (AllowedClasses = "/Script/Engine.Texture", ToolTip = "Texture to display"))
	FSoftObjectPath TexturePath;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "transform of center of quad (meters)."))
	FTransform TransformInMeters;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "Dimensions in meters."))
	FVector2D QuadSizeInMeters;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "A delta rotation that will be added each rendering frame (half rate of full vsync)."))
	FQuat DeltaRotation;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "Texture offset amount from the top left corner."))
	FVector2D TextureOffset;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "Texture scale."))
	FVector2D TextureScale;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (ToolTip = "Whether the splash layer uses it's alpha channel."))
	bool bNoAlphaChannel;

	// Runtime data
	UTexture* LoadingTexture;
	FTextureRHIRef LoadedTexture;
	bool bIsDynamic;

	FOculusXRSplashDesc()
		: TransformInMeters(FVector(4.0f, 0.f, 0.f))
		, QuadSizeInMeters(3.f, 3.f)
		, DeltaRotation(FQuat::Identity)
		, TextureOffset(0.0f, 0.0f)
		, TextureScale(1.0f, 1.0f)
		, bNoAlphaChannel(false)
		, LoadingTexture(nullptr)
		, LoadedTexture(nullptr)
		, bIsDynamic(false)
	{
	}

	bool operator==(const FOculusXRSplashDesc& d) const
	{
		return TexturePath == d.TexturePath && TransformInMeters.Equals(d.TransformInMeters) && QuadSizeInMeters == d.QuadSizeInMeters && DeltaRotation.Equals(d.DeltaRotation) && TextureOffset == d.TextureOffset && TextureScale == d.TextureScale && bNoAlphaChannel == d.bNoAlphaChannel && LoadingTexture == d.LoadingTexture && LoadedTexture == d.LoadedTexture && bIsDynamic == d.bIsDynamic;
	}
};

