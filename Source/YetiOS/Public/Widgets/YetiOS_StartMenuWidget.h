// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_StartMenuWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_StartMenuWidget.h

* Description:
Widget that represents Start Menu
*************************************************************************/
UCLASS(Abstract, DisplayName = "Start Menu Widget")
class YETIOS_API UYetiOS_StartMenuWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_StartMenu;

private:

	/** Weak pointer to start menu object. */
	TWeakObjectPtr<const class UYetiOS_StartMenu> StartMenu;

	/**
	* private static UYetiOS_StartMenuWidget::Internal_CreateStartMenuWidget
	* Create start menu widget for given start menu
	* @param InStartMenu [const UYetiOS_StartMenu*] Start menu that owns this widget.
	* @return [UYetiOS_StartMenuWidget*] Newly created start menu widget.
	**/
	static UYetiOS_StartMenuWidget* Internal_CreateStartMenuWidget(const UYetiOS_StartMenu* InStartMenu);
};
