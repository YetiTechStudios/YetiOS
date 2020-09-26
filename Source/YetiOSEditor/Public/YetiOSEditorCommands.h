// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FYetiOSEditorCommands : public TCommands<FYetiOSEditorCommands>
{
public:
	FYetiOSEditorCommands()
		: TCommands<FYetiOSEditorCommands>(TEXT("YetiTechStudios"), NSLOCTEXT("Contexts", "YetiTechStudios", "YetiTech Studios"), NAME_None, FCoreStyle::Get().GetStyleSetName())
	{
	}
public:
	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> SupportCommand;
	TSharedPtr<FUICommandInfo> HelpCommand;
	TSharedPtr<FUICommandInfo> DiscordCommand;
	TSharedPtr<FUICommandInfo> MarketplaceCommand;
};
