// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

#include "OculusXRPassthroughLayerComponent.h"

#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "OculusXRHMD.h"
#include "OculusXRPassthroughLayerShapes.h"
#include "Curves/CurveLinearColor.h"

DEFINE_LOG_CATEGORY(LogOculusPassthrough);

void UOculusXRStereoLayerShapeReconstructed::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	const FEdgeStyleParameters EdgeStyleParameters(
		bEnableEdgeColor,
		bEnableColorMap,
		TextureOpacityFactor,
		Brightness,
		Contrast,
		Posterize,
		Saturation,
		EdgeColor,
		ColorScale,
		ColorOffset,
		ColorMapType,
		GetColorArray(bUseColorMapCurve, ColorMapCurve),
		GenerateColorLutDescription(LutWeight, ColorLUTSource, ColorLUTTarget));
	LayerDesc.SetShape<FReconstructedLayer>(EdgeStyleParameters, LayerOrder);
}

void UOculusXRStereoLayerShapeUserDefined::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	const FEdgeStyleParameters EdgeStyleParameters(
		bEnableEdgeColor,
		bEnableColorMap,
		TextureOpacityFactor,
		Brightness,
		Contrast,
		Posterize,
		Saturation,
		EdgeColor,
		ColorScale,
		ColorOffset,
		ColorMapType,
		GetColorArray(bUseColorMapCurve, ColorMapCurve),
		GenerateColorLutDescription(LutWeight, ColorLUTSource, ColorLUTTarget));
	LayerDesc.SetShape<FUserDefinedLayer>(UserGeometryList, EdgeStyleParameters, LayerOrder);
}

void UOculusXRStereoLayerShapeUserDefined::AddGeometry(const FString& MeshName, OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh, FTransform Transform, bool bUpdateTransform)
{
	FUserDefinedGeometryDesc UserDefinedGeometryDesc(
		MeshName,
		PassthroughMesh,
		Transform,
		bUpdateTransform);

	UserGeometryList.Add(UserDefinedGeometryDesc);
}

void UOculusXRStereoLayerShapeUserDefined::RemoveGeometry(const FString& MeshName)
{
	UserGeometryList.RemoveAll([MeshName](const FUserDefinedGeometryDesc& Desc) {
		return Desc.MeshName == MeshName;
	});
}

UOculusXRPassthroughLayerComponent::UOculusXRPassthroughLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOculusXRPassthroughLayerComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
#ifdef WITH_OCULUS_BRANCH
	IStereoLayers* StereoLayers;
	if (LayerId && GEngine->StereoRenderingDevice.IsValid() && (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) != nullptr)
	{
		StereoLayers->DestroyLayer(LayerId);
		LayerId = 0;
	}
#endif
}

void UOculusXRPassthroughLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
#ifndef WITH_OCULUS_BRANCH
	if (Texture == nullptr && !LayerRequiresTexture())
	{
		// UStereoLayerComponent hides components without textures
		Texture = GEngine->DefaultTexture;
	}
#endif
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdatePassthroughObjects();
}

void UOculusXRPassthroughLayerComponent::UpdatePassthroughObjects()
{
	UOculusXRStereoLayerShapeUserDefined* UserShape = Cast<UOculusXRStereoLayerShapeUserDefined>(Shape);
	if (UserShape)
	{
		bool bDirty = false;
		for (FUserDefinedGeometryDesc& Entry : UserShape->GetUserGeometryList())
		{
			if (Entry.bUpdateTransform)
			{
				AStaticMeshActor** StaticMeshActor = PassthroughActorMap.Find(Entry.MeshName);
				if (StaticMeshActor)
				{
					UStaticMeshComponent* StaticMeshComponent = (*StaticMeshActor)->GetStaticMeshComponent();
					if (StaticMeshComponent)
					{
						Entry.Transform = StaticMeshComponent->GetComponentTransform();
						bDirty = true;
					}
				}
			}
		}
		if (bDirty)
		{
			MarkStereoLayerDirty();
		}
	}
}

OculusXRHMD::FOculusPassthroughMeshRef UOculusXRPassthroughLayerComponent::CreatePassthroughMesh(UStaticMesh* Mesh)
{
	if (!Mesh || !Mesh->GetRenderData())
	{
		UE_LOG(LogOculusPassthrough, Error, TEXT("Passthrough Static Mesh has no Renderdata"));
		return nullptr;
	}

	if (Mesh->GetNumLODs() == 0)
	{
		UE_LOG(LogOculusPassthrough, Error, TEXT("Passthrough Static Mesh has no LODs"));
		return nullptr;
	}

	if (!Mesh->bAllowCPUAccess)
	{
		UE_LOG(LogOculusPassthrough, Error, TEXT("Passthrough Static Mesh Requires CPU Access"));
		return nullptr;
	}

	const int32 LODIndex = 0;
	FStaticMeshLODResources& LOD = Mesh->GetRenderData()->LODResources[LODIndex];

	TArray<int32> Triangles;
	const int32 NumIndices = LOD.IndexBuffer.GetNumIndices();
	for (int32 i = 0; i < NumIndices; ++i)
	{
		Triangles.Add(LOD.IndexBuffer.GetIndex(i));
	}

	TArray<FVector> Vertices;
	const int32 NumVertices = LOD.VertexBuffers.PositionVertexBuffer.GetNumVertices();
	for (int32 i = 0; i < NumVertices; ++i)
	{
		Vertices.Add((FVector)LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(i));
	}

	OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh = new OculusXRHMD::FOculusPassthroughMesh(Vertices, Triangles);
	return PassthroughMesh;
}

void UOculusXRPassthroughLayerComponent::AddSurfaceGeometry(AStaticMeshActor* StaticMeshActor, bool updateTransform)
{
	if (StaticMeshActor)
	{
		UOculusXRStereoLayerShapeUserDefined* UserShape = Cast<UOculusXRStereoLayerShapeUserDefined>(Shape);
		if (UserShape)
		{
			UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
			if (StaticMeshComponent)
			{
				UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
				if (StaticMesh)
				{
					OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh = CreatePassthroughMesh(StaticMesh);
					if (PassthroughMesh)
					{
						const FString MeshName = StaticMeshActor->GetFullName();
						const FTransform Transform = StaticMeshComponent->GetComponentTransform();
						UserShape->AddGeometry(MeshName, PassthroughMesh, Transform, updateTransform);
					}
				}
			}

			PassthroughActorMap.Add(StaticMeshActor->GetFullName(), StaticMeshActor);
			MarkStereoLayerDirty();
		}
	}
}

void UOculusXRPassthroughLayerComponent::RemoveSurfaceGeometry(AStaticMeshActor* StaticMeshActor)
{
	if (StaticMeshActor)
	{
		UOculusXRStereoLayerShapeUserDefined* UserShape = Cast<UOculusXRStereoLayerShapeUserDefined>(Shape);
		if (UserShape)
		{
			UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
			if (StaticMeshComponent)
			{
				UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
				if (StaticMesh)
				{
					const FString MeshName = StaticMeshActor->GetFullName();
					UserShape->RemoveGeometry(MeshName);
				}
			}
			PassthroughActorMap.Remove(StaticMeshActor->GetFullName());
		}
	}

	MarkStereoLayerDirty();
}

bool UOculusXRPassthroughLayerComponent::IsSurfaceGeometry(AStaticMeshActor* StaticMeshActor) const
{
	if (StaticMeshActor)
	{
		UOculusXRStereoLayerShapeUserDefined* UserShape = Cast<UOculusXRStereoLayerShapeUserDefined>(Shape);
		if (UserShape)
		{
			return PassthroughActorMap.Contains(StaticMeshActor->GetFullName());
		}
	}

	return false;
}

void UOculusXRPassthroughLayerComponent::MarkPassthroughStyleForUpdate()
{
	bPassthroughStyleNeedsUpdate = true;
}

bool UOculusXRPassthroughLayerComponent::LayerRequiresTexture()
{
	const bool bIsPassthroughShape = Shape && (Shape->IsA<UOculusXRStereoLayerShapeReconstructed>() || Shape->IsA<UOculusXRStereoLayerShapeUserDefined>());
	return !bIsPassthroughShape;
}

void UOculusXRPassthroughLayerBase::SetTextureOpacity(float InOpacity)
{
	if (TextureOpacityFactor == InOpacity)
	{
		return;
	}

	TextureOpacityFactor = InOpacity;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::EnableEdgeColor(bool bInEnableEdgeColor)
{
	if (bEnableEdgeColor == bInEnableEdgeColor)
	{
		return;
	}
	bEnableEdgeColor = bInEnableEdgeColor;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::EnableColorMap(bool bInEnableColorMap)
{
	if (bEnableColorMap == bInEnableColorMap)
	{
		return;
	}
	bEnableColorMap = bInEnableColorMap;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetEdgeRenderingColor(FLinearColor InEdgeColor)
{
	if (EdgeColor == InEdgeColor)
	{
		return;
	}
	EdgeColor = InEdgeColor;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::EnableColorMapCurve(bool bInEnableColorMapCurve)
{
	if (bUseColorMapCurve == bInEnableColorMapCurve)
	{
		return;
	}
	bUseColorMapCurve = bInEnableColorMapCurve;
	ColorArray = GenerateColorArray(bUseColorMapCurve, ColorMapCurve);
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorMapCurve(UCurveLinearColor* InColorMapCurve)
{
	if (ColorMapCurve == InColorMapCurve)
	{
		return;
	}
	ColorMapCurve = InColorMapCurve;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorMapType(EOculusXRColorMapType InColorMapType)
{
	if (ColorMapType == InColorMapType)
	{
		return;
	}
	ColorMapType = InColorMapType;
	ColorArray = GenerateColorArray(bUseColorMapCurve, ColorMapCurve);
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorArray(const TArray<FLinearColor>& InColorArray)
{
	if (InColorArray.Num() == 0)
	{
		return;
	}

	if (ColorMapType != ColorMapType_GrayscaleToColor)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetColorArray is ignored for color map types other than Grayscale to Color."));
		return;
	}

	if (bUseColorMapCurve)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("UseColorMapCurve is enabled on the layer. Automatic disable and use the Array for color lookup"));
	}
	bUseColorMapCurve = false;

	ColorArray = InColorArray;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::ClearColorMap()
{
	ColorArray.Empty();
}

void UOculusXRPassthroughLayerBase::SetColorMapControls(float InContrast, float InBrightness, float InPosterize)
{
	if (ColorMapType != ColorMapType_Grayscale && ColorMapType != ColorMapType_GrayscaleToColor)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetColorMapControls is ignored for color map types other than Grayscale and Grayscale to color."));
		return;
	}
	Contrast = FMath::Clamp(InContrast, -1.0f, 1.0f);
	Brightness = FMath::Clamp(InBrightness, -1.0f, 1.0f);
	Posterize = FMath::Clamp(InPosterize, 0.0f, 1.0f);

	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetBrightnessContrastSaturation(float InContrast, float InBrightness, float InSaturation)
{
	if (ColorMapType != ColorMapType_ColorAdjustment)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetBrightnessContrastSaturation is ignored for color map types other than Color Adjustment."));
		return;
	}
	Contrast = FMath::Clamp(InContrast, -1.0f, 1.0f);
	Brightness = FMath::Clamp(InBrightness, -1.0f, 1.0f);
	Saturation = FMath::Clamp(InSaturation, -1.0f, 1.0f);

	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorScaleAndOffset(FLinearColor InColorScale, FLinearColor InColorOffset)
{
	if (ColorScale == InColorScale && ColorOffset == InColorOffset)
	{
		return;
	}
	ColorScale = InColorScale;
	ColorOffset = InColorOffset;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetLayerPlacement(EOculusXRPassthroughLayerOrder InLayerOrder)
{
	if (LayerOrder == InLayerOrder)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("Same layer order as before, no change needed"));
		return;
	}

	LayerOrder = InLayerOrder;
	this->MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorLUTSource(class UOculusXRPassthroughColorLut* InColorLUTSource)
{
	if (ColorMapType != ColorMapType_ColorLut && ColorMapType != ColorMapType_ColorLut_Interpolated)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetColorLUT is ignored for color map types other than Color LUT."));
		return;
	}

	if (InColorLUTSource == ColorLUTSource)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("Same color LUT source as before, no change needed"));
		return;
	}

	ColorLUTSource = InColorLUTSource;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorLUTTarget(class UOculusXRPassthroughColorLut* InColorLUTTarget)
{
	if (ColorMapType != ColorMapType_ColorLut_Interpolated)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetColorLUTTarget is ignored for color map types other than Interpolated Color LUT."));
		return;
	}

	if (InColorLUTTarget == ColorLUTTarget)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("Same color LUT source as before, no change needed"));
		return;
	}

	ColorLUTTarget = InColorLUTTarget;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::SetColorLUTWeight(float InWeight)
{
	if (ColorMapType != ColorMapType_ColorLut && ColorMapType != ColorMapType_ColorLut_Interpolated)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("SetWeight is ignored for color map types other than Color LUT."));
		return;
	}

	if (LutWeight == InWeight)
	{
		UE_LOG(LogOculusPassthrough, Warning, TEXT("Same lut weight as before, no change needed"));
		return;
	}

	LutWeight = InWeight;
	MarkStereoLayerDirty();
}

void UOculusXRPassthroughLayerBase::RemoveColorLut()
{
	ColorLUTSource = nullptr;
	ColorLUTTarget = nullptr;
	MarkStereoLayerDirty();
}

TArray<FLinearColor> UOculusXRPassthroughLayerBase::GenerateColorArrayFromColorCurve(const UCurveLinearColor* InColorMapCurve) const
{
	if (InColorMapCurve == nullptr)
	{
		return TArray<FLinearColor>();
	}

	TArray<FLinearColor> NewColorArray;
	constexpr uint32 TotalEntries = 256;
	NewColorArray.Empty();
	NewColorArray.SetNum(TotalEntries);

	for (int32 Index = 0; Index < TotalEntries; ++Index)
	{
		const float Alpha = ((float)Index / TotalEntries);
		NewColorArray[Index] = InColorMapCurve->GetLinearColorValue(Alpha);
	}
	return NewColorArray;
}

TArray<FLinearColor> UOculusXRPassthroughLayerBase::GetOrGenerateNeutralColorArray()
{
	if (NeutralColorArray.Num() == 0)
	{
		const uint32 TotalEntries = 256;
		NeutralColorArray.SetNum(TotalEntries);

		for (int32 Index = 0; Index < TotalEntries; ++Index)
		{
			NeutralColorArray[Index] = FLinearColor((float)Index / TotalEntries, (float)Index / TotalEntries, (float)Index / TotalEntries);
		}
	}

	return NeutralColorArray;
}

TArray<FLinearColor> UOculusXRPassthroughLayerBase::GenerateColorArray(bool bInUseColorMapCurve, const UCurveLinearColor* InColorMapCurve)
{
	TArray<FLinearColor> NewColorArray;
	if (bInUseColorMapCurve)
	{
		NewColorArray = GenerateColorArrayFromColorCurve(InColorMapCurve);
	}

	// Check for existing Array, otherwise generate a neutral one
	if (NewColorArray.Num() == 0)
	{
		NewColorArray = GetOrGenerateNeutralColorArray();
	}

	return NewColorArray;
}

TArray<FLinearColor> UOculusXRPassthroughLayerBase::GetColorArray(bool bInUseColorMapCurve, const UCurveLinearColor* InColorMapCurve)
{
	if (ColorArray.Num() == 0)
	{
		if (bInUseColorMapCurve)
		{
			return GenerateColorArray(bInUseColorMapCurve, InColorMapCurve);
		}
		return GetOrGenerateNeutralColorArray();
	}

	return ColorArray;
}

FColorLutDesc UOculusXRPassthroughLayerBase::GenerateColorLutDescription(float InLutWeight, UOculusXRPassthroughColorLut* InLutSource, UOculusXRPassthroughColorLut* InLutTarget)
{
	TArray<uint64> ColorLuts;
	if (InLutSource != nullptr && InLutSource->ColorLutType != EColorLutType::None)
	{
		uint64 ColorLutHandle = InLutSource->GetHandle();
		if (ColorLutHandle != 0)
		{
			ColorLuts.Add(ColorLutHandle);
		}
	}

	if (ColorMapType == EOculusXRColorMapType::ColorMapType_ColorLut_Interpolated && ColorLuts.Num() > 0 && InLutSource->ColorLutType != EColorLutType::None)
	{
		uint64 ColorLutHandle = InLutTarget->GetHandle();
		if (ColorLutHandle != 0)
		{
			ColorLuts.Add(ColorLutHandle);
		}
	}

	return FColorLutDesc(ColorLuts, InLutWeight);
}
