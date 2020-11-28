// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#include "YetiOSEditorModule.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "YetiOSEditorCommands.h"
#include "YetiOS_ThumbnailRenderer.h"
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

	return Local_YetiOsCommands;
}

IMPLEMENT_MODULE(FYetiOSEditorModule, YetiOSEditor)

#undef LOCTEXT_NAMESPACE
