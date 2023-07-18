/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXREditorModule.h"

#include "AssetToolsModule.h"
#include "OculusXRToolStyle.h"
#include "OculusXRToolCommands.h"
#include "OculusXRToolWidget.h"
#include "OculusXRPlatformToolWidget.h"
#include "OculusXRAssetDirectory.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "PropertyEditorModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IAssetTools.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISettingsModule.h"
#include "OculusXREditorSettings.h"
#include "OculusXRPassthroughColorLutAsset.h"

#define LOCTEXT_NAMESPACE "OculusXREditor"

const FName FOculusXREditorModule::OculusPerfTabName = FName("OculusXRPerfCheck");
const FName FOculusXREditorModule::OculusPlatToolTabName = FName("OculusXRPlaformTool");

void FOculusXREditorModule::PostLoadCallback()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
}

void FOculusXREditorModule::StartupModule()
{
	bModuleValid = true;
	RegisterSettings();
	FOculusAssetDirectory::LoadForCook();

	if (!IsRunningCommandlet())
	{
		FOculusToolStyle::Initialize();
		FOculusToolStyle::ReloadTextures();

		FOculusToolCommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);

		PluginCommands->MapAction(
			FOculusToolCommands::Get().OpenPluginWindow,
			FExecuteAction::CreateRaw(this, &FOculusXREditorModule::PluginOpenPerfWindow),
			FCanExecuteAction());
		PluginCommands->MapAction(
			FOculusToolCommands::Get().OpenPlatWindow,
			FExecuteAction::CreateRaw(this, &FOculusXREditorModule::PluginOpenPlatWindow),
			FCanExecuteAction());
		PluginCommands->MapAction(
			FOculusToolCommands::Get().ToggleDeploySo,
			FExecuteAction::CreateLambda([=]() {
				UOculusXRHMDRuntimeSettings* settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();
				settings->bDeploySoToDevice = !settings->bDeploySoToDevice;
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([=]() {
				return GetMutableDefault<UOculusXRHMDRuntimeSettings>()->bDeploySoToDevice;
			}));

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

		// Adds an option to launch the tool to Window->Developer Tools.
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("Miscellaneous", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FOculusXREditorModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

		// We add the Oculus menu on the toolbar
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Play", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FOculusXREditorModule::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OculusPerfTabName, FOnSpawnTab::CreateRaw(this, &FOculusXREditorModule::OnSpawnPluginTab)).SetDisplayName(LOCTEXT("FOculusXREditorTabTitle", "Meta XR Performance Check")).SetMenuType(ETabSpawnerMenuType::Hidden);

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OculusPlatToolTabName, FOnSpawnTab::CreateRaw(this, &FOculusXREditorModule::OnSpawnPlatToolTab)).SetDisplayName(LOCTEXT("FOculusPlatfToolTabTitle", "Meta XR Platform Tool")).SetMenuType(ETabSpawnerMenuType::Hidden);

		// Register asset types
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_OculusXRPassthroughColorLut));
	}
}

void FOculusXREditorModule::ShutdownModule()
{
	if (!bModuleValid)
	{
		return;
	}

	if (!IsRunningCommandlet())
	{
		FOculusToolStyle::Shutdown();
		FOculusToolCommands::Unregister();
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OculusPerfTabName);
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OculusPlatToolTabName);
	}

	FOculusAssetDirectory::ReleaseAll();
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

TSharedRef<SDockTab> FOculusXREditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	/* clang-format off */
	auto myTab = SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SOculusToolWidget)
	];
	/* clang-format on */

	return myTab;
}

TSharedRef<SDockTab> FOculusXREditorModule::OnSpawnPlatToolTab(const FSpawnTabArgs& SpawnTabArgs)
{
	/* clang-format off */
	auto myTab = SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SOculusPlatformToolWidget)
	];
	/* clang-format on */

	return myTab;
}

void FOculusXREditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "OculusXR",
			LOCTEXT("RuntimeSettingsName", "Meta XR"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Meta XR plugin"),
			GetMutableDefault<UOculusXRHMDRuntimeSettings>());

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(UOculusXRHMDRuntimeSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOculusXRHMDSettingsDetailsCustomization::MakeInstance));
	}
}

void FOculusXREditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "OculusXR");
	}
}

FReply FOculusXREditorModule::PluginClickFn(bool text)
{
	PluginOpenPerfWindow();
	return FReply::Handled();
}

void FOculusXREditorModule::PluginOpenPerfWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(OculusPerfTabName);
}

void FOculusXREditorModule::PluginOpenPlatWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(OculusPlatToolTabName);
}

void FOculusXREditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	bool v = false;
	GConfig->GetBool(TEXT("/Script/OculusXREditor.OculusXREditorSettings"), TEXT("bAddMenuOption"), v, GEditorIni);
	if (v)
	{
		Builder.AddMenuEntry(FOculusToolCommands::Get().OpenPluginWindow);
	}
}

void FOculusXREditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.SetLabelVisibility(EVisibility::All);
	Builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FOculusXREditorModule::CreateToolbarEntryMenu, PluginCommands),
		LOCTEXT("OculusToolsToolBarCombo", "Meta XR Tools"),
		LOCTEXT("OculusToolsToolBarComboTooltip", "Meta XR tools"),
		TAttribute<FSlateIcon>::CreateLambda([]() {
			return FSlateIcon(FOculusToolStyle::GetStyleSetName(), "OculusTool.MenuButton");
		}),
		false);
}

// Add the entries to the OculusXR Tools toolbar menu button
TSharedRef<SWidget> FOculusXREditorModule::CreateToolbarEntryMenu(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);
	MenuBuilder.BeginSection("OculusXRBuilds", LOCTEXT("OculusXRBuilds", "Builds"));
	MenuBuilder.AddMenuEntry(FOculusToolCommands::Get().ToggleDeploySo);
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("OculusXRTools", LOCTEXT("OculusXRTools", "Tools"));
	MenuBuilder.AddMenuEntry(FOculusToolCommands::Get().OpenPluginWindow);
	MenuBuilder.AddMenuEntry(FOculusToolCommands::Get().OpenPlatWindow);
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

TSharedRef<IDetailCustomization> FOculusXRHMDSettingsDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FOculusXRHMDSettingsDetailsCustomization);
}

FReply FOculusXRHMDSettingsDetailsCustomization::PluginClickPerfFn(bool text)
{
	FGlobalTabmanager::Get()->TryInvokeTab(FOculusXREditorModule::OculusPerfTabName);
	return FReply::Handled();
}

FReply FOculusXRHMDSettingsDetailsCustomization::PluginClickPlatFn(bool text)
{
	FGlobalTabmanager::Get()->TryInvokeTab(FOculusXREditorModule::OculusPlatToolTabName);
	return FReply::Handled();
}

void FOculusXRHMDSettingsDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	// Labeled "General OculusXR" instead of "General" to enable searchability. The button "Launch Oculus Utilities Window" doesn't show up if you search for "Oculus"
	IDetailCategoryBuilder& CategoryBuilder = DetailLayout.EditCategory("General Meta XR", FText::GetEmpty(), ECategoryPriority::Important);
	/* clang-format off */
	CategoryBuilder.AddCustomRow(LOCTEXT("General", "General"))
	.WholeRowContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("LaunchTool", "Launch Meta XR Performance Window"))
				.OnClicked(this, &FOculusXRHMDSettingsDetailsCustomization::PluginClickPerfFn, true)
			]
			+ SHorizontalBox::Slot().FillWidth(8)
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("LaunchPlatTool", "Launch Meta XR Platform Window"))
				.OnClicked(this, &FOculusXRHMDSettingsDetailsCustomization::PluginClickPlatFn, true)
			]
			+ SHorizontalBox::Slot().FillWidth(8)
		]
	];
	/* clang-format on */
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE(FOculusXREditorModule, OculusXREditor);

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
