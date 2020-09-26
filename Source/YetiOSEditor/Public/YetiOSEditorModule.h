// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FYetiOSEditorModule : public IModuleInterface
{

private:
	TSharedPtr<FUICommandList> YetiOsCommands;

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void Internal_CreateMenuBarEntry();
	TSharedPtr<FUICommandList> Internal_CreateCommands();
};
