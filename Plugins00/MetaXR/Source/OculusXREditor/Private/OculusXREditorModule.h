/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "CoreMinimal.h"
#include "IOculusXREditorModule.h"
#include "Modules/ModuleInterface.h"
#include "IDetailCustomization.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"

class FToolBarBuilder;
class FMenuBuilder;

#define OCULUS_EDITOR_MODULE_NAME "OculusXREditor"

enum class ECheckBoxState : uint8;

class FOculusXREditorModule : public IOculusXREditorModule
{
public:
	FOculusXREditorModule()
		: bModuleValid(false){};

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void PostLoadCallback() override;

	void RegisterSettings();
	void UnregisterSettings();

	void PluginOpenPerfWindow();
	FReply PluginClickFn(bool text);

	void PluginOpenPlatWindow();

public:
	static const FName OculusPerfTabName;
	static const FName OculusPlatToolTabName;

private:
	void AddToolbarExtension(FToolBarBuilder& Builder);
	TSharedRef<SWidget> CreateToolbarEntryMenu(TSharedPtr<class FUICommandList> Commands);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<class SDockTab> OnSpawnPlatToolTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	bool bModuleValid;
};

class IDetailLayoutBuilder;

class FOculusXRHMDSettingsDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	// End of IDetailCustomization interface

	FReply PluginClickPerfFn(bool text);
	FReply PluginClickPlatFn(bool text);
};
