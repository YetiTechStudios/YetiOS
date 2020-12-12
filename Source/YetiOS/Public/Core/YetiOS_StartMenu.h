// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_StartMenu.generated.h"

UCLASS(Abstract, Blueprintable, DisplayName = "Operating System Start Menu")
class YETIOS_API UYetiOS_StartMenu : public UObject
{
	GENERATED_BODY()

	friend class UYetiOS_Taskbar;

private:

	/** Shows all the installed apps. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Start Menu")
	uint8 bShowAppList : 1;

	/** Shows apps that were recently installed. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Start Menu")
	uint8 bShowRecentlyAddedApps : 1;

	/** Widget that represents the start menu. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Start Menu")
	TSubclassOf<class UYetiOS_StartMenuWidget> StartMenuWidgetClass;

	/** Reference to the actual start menu widget. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_StartMenuWidget* StartMenuWidget;

	/** Weak pointer to the Operating System that owns this start menu. */
	TWeakObjectPtr<UYetiOS_Core> OwningOS;

protected:

	/**
	* protected static UYetiOS_StartMenu::CreateStartMenu
	* Creates the Start Menu object.
	* @param InTaskbar [class UYetiOS_Taskbar*] Taskbar that this start menu should be added to.
	* @return [UYetiOS_StartMenu*] Returns Start Menu reference.
	**/
	static UYetiOS_StartMenu* CreateStartMenu(class UYetiOS_Taskbar* InTaskbar);

public:

	/**
	* public UYetiOS_StartMenu::GetStartMenuWidget const
	* Returns the start menu widget.
	* @return [UYetiOS_StartMenuWidget*] Start Menu Widget
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Start Menu")	
	UYetiOS_StartMenuWidget* GetStartMenuWidget() const { return StartMenuWidget; }

	/**
	* public UYetiOS_StartMenu::GetOwningOS const
	* Gets the Operating System of this Start Menu.
	* @return [class UYetiOS_Core*] Operating System.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Start Menu")	
	class UYetiOS_Core* GetOwningOS() const;

public:

	FORCEINLINE TSubclassOf<class UYetiOS_StartMenuWidget> GetStartMenuWidgetClass() const { return StartMenuWidgetClass; }
};
