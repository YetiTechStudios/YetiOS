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

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_StartMenuWidget* StartMenuWidget;

	TWeakObjectPtr<UYetiOS_Core> OwningOS;

protected:

	static UYetiOS_StartMenu* CreateStartMenu(class UYetiOS_Taskbar* InTaskbar);

public:

	UFUNCTION(BlueprintPure, Category = "Yeti OS Start Menu")
	UYetiOS_StartMenuWidget* GetStartMenuWidget() const { return StartMenuWidget; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Start Menu")
	class UYetiOS_Core* GetOwningOS() const;

public:

	FORCEINLINE TSubclassOf<class UYetiOS_StartMenuWidget> GetStartMenuWidgetClass() const { return StartMenuWidgetClass; }
};
