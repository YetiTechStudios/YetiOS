// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#include "YetiOSEditorModule.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "YetiOSEditorCommands.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "YetiOS_Factories.h"
#include "YetiOS_ThumbnailRenderer.h"
#include "Core/YetiOS_DirectoryBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOs, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOs, Log, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "FYetiOSEditorModule"

void FYetiOSEditorModule::StartupModule()
{
	UThumbnailManager::Get().UnregisterCustomRenderer(UBlueprint::StaticClass());
	UThumbnailManager::Get().RegisterCustomRenderer(UBlueprint::StaticClass(), UYetiOS_ThumbnailRenderer::StaticClass());
	printlog("Registered new thumbnail renderer for UBlueprint.");

	YetiOsCommands = Internal_CreateCommands();
	Internal_CreateMenuBarEntry();
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	YetiOS_AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("YetiOsCategory")), LOCTEXT("YetiOsCategory", "Operating System Simulator"));

	TSharedRef<IAssetTypeActions> Category_BaseDirectory = MakeShareable(new FAssetTypeActions_BaseDirectory);
	TSharedRef<IAssetTypeActions> Category_BaseFile = MakeShareable(new FAssetTypeActions_BaseFile);
	TSharedRef<IAssetTypeActions> Category_BaseProgram = MakeShareable(new FAssetTypeActions_BaseProgram);
	TSharedRef<IAssetTypeActions> Category_OS = MakeShareable(new FAssetTypeActions_OperatingSystem);
	TSharedRef<IAssetTypeActions> Category_PortableDevice = MakeShareable(new FAssetTypeActions_PortableDevice);
	TSharedRef<IAssetTypeActions> Category_StationaryDevice = MakeShareable(new FAssetTypeActions_StationaryDevice);
	TSharedRef<IAssetTypeActions> Category_DeviceManager = MakeShareable(new FAssetTypeActions_DeviceManager);
	TSharedRef<IAssetTypeActions> Category_ProgramsRepository = MakeShareable(new FAssetTypeActions_ProgramsRepository);

	AssetTools.RegisterAssetTypeActions(Category_BaseDirectory);
	AssetTools.RegisterAssetTypeActions(Category_BaseFile);
	AssetTools.RegisterAssetTypeActions(Category_BaseProgram);
	AssetTools.RegisterAssetTypeActions(Category_OS);
	AssetTools.RegisterAssetTypeActions(Category_PortableDevice);
	AssetTools.RegisterAssetTypeActions(Category_StationaryDevice);
	AssetTools.RegisterAssetTypeActions(Category_DeviceManager);
	AssetTools.RegisterAssetTypeActions(Category_ProgramsRepository);
	
	printlog("Registered Content Browser extensions.");
}

void FYetiOSEditorModule::ShutdownModule()
{
	FYetiOSEditorCommands::Unregister();
}

void FYetiOSEditorModule::Internal_CreateMenuBarEntry()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		auto static AddMenuExtension = [](FMenuBuilder& Builder)
		{
			Builder.BeginSection("Help", LOCTEXT("YetiOSEditor_Help", "Help"));
			Builder.AddMenuEntry(FYetiOSEditorCommands::Get().MarketplaceCommand);
			Builder.AddMenuEntry(FYetiOSEditorCommands::Get().HelpCommand);
			Builder.AddMenuEntry(FYetiOSEditorCommands::Get().DiscordCommand);
			Builder.EndSection();

			Builder.BeginSection("Support", LOCTEXT("YetiOSEditor_Support", "Support"));
			Builder.AddMenuEntry(FYetiOSEditorCommands::Get().SupportCommand);
			Builder.EndSection();
		};

		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, YetiOsCommands, FMenuBarExtensionDelegate::CreateStatic([](FMenuBarBuilder& Builder)
		{
			Builder.AddPullDownMenu(
				LOCTEXT("YetiOSEditorEditorMenu", "YetiTech Studios"),
				LOCTEXT("YetiOSEditorEditorMenu_Tooltip", "Help and Documentation"),
				FNewMenuDelegate::CreateStatic(AddMenuExtension), "YetiOSEditorPlugin");
		}));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

		printlog("Registered Menubar.");
	}
}

TSharedPtr<FUICommandList> FYetiOSEditorModule::Internal_CreateCommands()
{
	FYetiOSEditorModule* YetiOsModule = FModuleManager::GetModulePtr<FYetiOSEditorModule>("YetiOSEditor");;

	FYetiOSEditorCommands::Register();
	TSharedPtr<FUICommandList> Local_YetiOsCommands = MakeShareable(new FUICommandList);

	Local_YetiOsCommands->MapAction(
		FYetiOSEditorCommands::Get().SupportCommand,
		FExecuteAction::CreateLambda([]()
	{
		FPlatformProcess::LaunchURL(TEXT("https://patreon.com/YetiTechStudios"), NULL, NULL);
	}), FCanExecuteAction());

	Local_YetiOsCommands->MapAction(
		FYetiOSEditorCommands::Get().HelpCommand,
		FExecuteAction::CreateLambda([]()
	{
		FPlatformProcess::LaunchURL(TEXT("http://bit.ly/YTDocs"), NULL, NULL);
	}), FCanExecuteAction());

	Local_YetiOsCommands->MapAction(
		FYetiOSEditorCommands::Get().DiscordCommand,
		FExecuteAction::CreateLambda([]()
	{
		FPlatformProcess::LaunchURL(TEXT("https://discord.gg/S7ykM2q"), NULL, NULL);
	}), FCanExecuteAction());

	Local_YetiOsCommands->MapAction(
		FYetiOSEditorCommands::Get().MarketplaceCommand,
		FExecuteAction::CreateLambda([]()
	{
		FPlatformProcess::LaunchURL(TEXT("https://www.unrealengine.com/marketplace/profile/YetiTech+Studios"), NULL, NULL);
	}), FCanExecuteAction());

	printlog("Registered FUICommandList for YetiOS Commands.");
	return Local_YetiOsCommands;
}

IMPLEMENT_MODULE(FYetiOSEditorModule, YetiOSEditor)

#undef LOCTEXT_NAMESPACE

#undef printlog
