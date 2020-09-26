// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#include "YetiOSEditorCommands.h"

#define LOCTEXT_NAMESPACE "FYetiOSEditorCommands"

void FYetiOSEditorCommands::RegisterCommands()
{
	UI_COMMAND(SupportCommand, "Support Bash (Patreon)", "Become a Patron for Bash!", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(HelpCommand, "OSTS API Docs", "Operating System and Terminal Simulator Documentation", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(DiscordCommand, "YetiTech Studios Discord Server", "Join Discord Server for support", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(MarketplaceCommand, "Visit Unreal Marketplace profile", "Visit our Marketplace Page", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
