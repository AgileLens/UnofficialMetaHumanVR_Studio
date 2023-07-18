// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OculusXRHMDTypes.h"
#include "OculusXRFunctionLibrary.h"
#include "OculusXRHMDRuntimeSettings.generated.h"

UENUM()
enum class EOculusXRSupportedDevices : uint8
{
	/** 0 was the deprecated Meta Quest */
	Quest2 = 1 UMETA(DisplayName = "Meta Quest 2"),
	QuestPro = 2 UMETA(DisplayName = "Meta Quest Pro"),
};

/**
* Implements the settings for the OculusVR plugin.
*/
UCLASS(config = Engine, defaultconfig)
class OCULUSXRHMD_API UOculusXRHMDRuntimeSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Whether the Splash screen is enabled. */
	UPROPERTY(config, EditAnywhere, Category = "Engine SplashScreen")
	bool bAutoEnabled;

	/** An array of splash screen descriptors listing textures to show and their positions. */
	UPROPERTY(config, EditAnywhere, Category = "Engine SplashScreen")
	TArray<FOculusXRSplashDesc> SplashDescs;

	/** This selects the XR API that the engine will use. If unsure, OVRPlugin OpenXR is the recommended API. */
	UPROPERTY(config, EditAnywhere, Category = General, meta = (DisplayName = "XR API", ConfigRestartRequired = true))
	EOculusXRXrApi XrApi;

	/** The target color space */
	UPROPERTY(config, EditAnywhere, Category = General)
	EOculusXRColorSpace ColorSpace;

	/** Whether the controller hand poses align to the Meta XR pose definitions or the OpenXR pose definitions */
	UPROPERTY(config, EditAnywhere, Category = General, meta = (EditCondition = "XrApi != EOculusXRXrApi::NativeOpenXR"))
	EOculusXRControllerPoseAlignment ControllerPoseAlignment;

	/** Whether Dash is supported by the app, which will keep the app in foreground when the User presses the oculus button (needs the app to handle input focus loss!) */
	UPROPERTY(config, EditAnywhere, Category = PC)
	bool bSupportsDash;

	/** Whether the app's depth buffer is shared with the Rift Compositor, for layer (including Dash) compositing, PTW, and potentially more. */
	UPROPERTY(config, EditAnywhere, Category = PC)
	bool bCompositesDepth;

	/** Computes mipmaps for the eye buffers every frame, for a higher quality distortion */
	UPROPERTY(config, EditAnywhere, Category = PC)
	bool bHQDistortion;

	/** Maximum allowed pixel density. */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Dynamic Resolution", DisplayName = "Enable Dynamic Resolution")
	bool bDynamicResolution;

	/** Minimum allowed pixel density. */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Dynamic Resolution")
	float PixelDensityMin;

	/** Maximum allowed pixel density. */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Dynamic Resolution")
	float PixelDensityMax;

	/** Default CPU level controlling CPU frequency on the mobile device */
	UPROPERTY(config, meta = (DeprecatedProperty, DeprecationMessage = "Use Blueprint Function Get/SetSuggestedCpuAndGpuPerformanceLevels instead."))
	int CPULevel_DEPRECATED;

	/** Default GPU level controlling GPU frequency on the mobile device */
	UPROPERTY(config, meta = (DeprecatedProperty, DeprecationMessage = "Use Blueprint Function Get/SetSuggestedCpuAndGpuPerformanceLevels instead."))
	int GPULevel_DEPRECATED;

	/** Select supported Meta Quest Devices */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Supported Meta Quest devices"))
	TArray<EOculusXRSupportedDevices> SupportedDevices;

	/** Suggested CPU perf level when application starts on Oculus Quest */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	EOculusXRProcessorPerformanceLevel SuggestedCpuPerfLevel;

	/** Suggested GPU perf level when application starts on Oculus Quest */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	EOculusXRProcessorPerformanceLevel SuggestedGpuPerfLevel;

	/** Foveated rendering method */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Foveated Rendering", meta = (EditCondition = "XrApi == EOculusXRXrApi::OVRPluginOpenXR"))
	EOculusXRFoveatedRenderingMethod FoveatedRenderingMethod;

	/** Foveated rendering level */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Foveated Rendering", meta = (EditCondition = "XrApi != EOculusXRXrApi::NativeOpenXR"))
	EOculusXRFoveatedRenderingLevel FoveatedRenderingLevel;

	/** Whether foveated rendering levels will change dynamically based on performance headroom or not (up to the set Foveation Level) */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Foveated Rendering", meta = (EditCondition = "XrApi != EOculusXRXrApi::NativeOpenXR"))
	bool bDynamicFoveatedRendering;

	/** Whether eye tracked foveated rendering can be used with the app. */
	UPROPERTY(config, EditAnywhere, Category = "Mobile|Foveated Rendering", meta = (EditCondition = "XrApi == EOculusXRXrApi::OVRPluginOpenXR"))
	bool bSupportEyeTrackedFoveatedRendering;

	/** Whether the app's depth buffer is shared with the compositor to enable depth testing against other layers.
	Mobile depth composition has performance overhead both on the engine (for resolving depth) and on the compositor (for depth testing against other layers) */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Composite Depth"))
	bool bCompositeDepthMobile;

	/** If enabled the app will be focus aware. This will keep the app in foreground when the User presses the oculus button (needs the app to handle input focus loss!) */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (EditCondition = "false"))
	bool bFocusAware;

	/** [Experimental]Enable Late latching for reducing HMD and controller latency, improve tracking prediction quality, multiview and vulkan must be enabled for this feature. */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	bool bLateLatching;

	/** If enabled the app will use the Oculus system keyboard for input fields. This requires that the app be focus aware. */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	bool bRequiresSystemKeyboard;

	/** Whether controllers and/or hands can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	EOculusXRHandTrackingSupport HandTrackingSupport;

	/** Note that a higher tracking frequency will reserve some performance headroom from the application's budget. */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	EOculusXRHandTrackingFrequency HandTrackingFrequency;

	/** The version of hand tracking algorithm */
	UPROPERTY(config, EditAnywhere, Category = Mobile)
	EOculusXRHandTrackingVersion HandTrackingVersion;

	/** Whether passthrough functionality can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Passthrough Enabled"))
	bool bInsightPassthroughEnabled;

	/** Whether Spatial Anchors can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Anchor Support"))
	bool bAnchorSupportEnabled;

	/** Whether Scene can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Scene Support"))
	bool bSceneSupportEnabled;

	/** Whether body tracking functionality can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Body Tracking Enabled", EditCondition = "XrApi == EOculusXRXrApi::OVRPluginOpenXR"))
	bool bBodyTrackingEnabled;

	/** Whether eye tracking functionality can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Eye Tracking Enabled", EditCondition = "XrApi == EOculusXRXrApi::OVRPluginOpenXR"))
	bool bEyeTrackingEnabled;

	/** Whether face tracking functionality can be used with the app */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Face Tracking Enabled", EditCondition = "XrApi == EOculusXRXrApi::OVRPluginOpenXR"))
	bool bFaceTrackingEnabled;

	/** On supported Oculus mobile platforms, copy compiled .so directly to device. Allows updating compiled code without rebuilding and installing an APK. */
	UPROPERTY(config, EditAnywhere, Category = Mobile, meta = (DisplayName = "Deploy compiled .so directly to device"))
	bool bDeploySoToDevice;

	/** Whether experimental features listed below can be used with the app. */
	UPROPERTY(config, EditAnywhere, Category = Experimental)
	bool bSupportExperimentalFeatures;

private:
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void PostInitProperties() override;

	void LoadFromIni();
	void RenameProperties();
};
