// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_StartMenuWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_StartMenuWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_StartMenu;

private:

	TWeakObjectPtr<const class UYetiOS_StartMenu> StartMenu;

	static UYetiOS_StartMenuWidget* Internal_CreateStartMenuWidget(const UYetiOS_StartMenu* InStartMenu);
};
