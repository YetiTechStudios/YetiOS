// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_UserWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_UserWidget.h

* Description:
Base class for all widgets.
*************************************************************************/
UCLASS()
class YETIOS_API UYetiOS_UserWidget : public UUserWidget
{
	GENERATED_BODY()

	FDelegateHandle OnThemeChangedDelegateHandle;
	FDelegateHandle OnColorSchemeChangedDelegateHandle;

protected:

	/** Operating System that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS User Widget", meta = (ExposeOnSpawn = "true"))
	class UYetiOS_Core* OwningOS;

	/** Window that owns this widget. */
	UPROPERTY()
	class UYetiOS_DraggableWindowWidget* OwningWindow;

public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	* public UYetiOS_UserWidget::SetOperatingSystem
	* Sets the Operating System that owns this widget.
	* @param InOS [UYetiOS_Core*] Operating System reference.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS User Widget")	
	void SetOperatingSystem(UYetiOS_Core* InOS);

	/**
	* public UYetiOS_UserWidget::SetWindow
	* Sets the window that owns this widget.
	* @param InWindow [class UYetiOS_DraggableWindowWidget *] Owning Window
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS User Widget")	
	void SetWindow(class UYetiOS_DraggableWindowWidget* InWindow) { OwningWindow = InWindow; }

	/**
	* public UYetiOS_UserWidget::GetOwningWindow const
	* Gets Owning Window
	* @return [class UYetiOS_DraggableWindowWidget*] OwningWindow
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS User Widget")	
	inline class UYetiOS_DraggableWindowWidget* GetOwningWindow() const { return OwningWindow; }

protected:

	/**
	* protected UYetiOS_UserWidget::UnbindEvents
	* Unbinds all events. It is important to call this function when you are going to remove this widget.
	**/
	void UnbindEvents();

	/**
	* protected UYetiOS_UserWidget::K2_SetOperatingSystem
	* Event called when Operating System is set.
	* @param OS [UYetiOS_Core*] Operating System.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS User Widget", DisplayName = "On Operating System Set")	
	void K2_SetOperatingSystem(UYetiOS_Core* OS);

	/**
	* protected UYetiOS_UserWidget::K2_OnThemeChanged
	* Event called when theme changes.
	* @param InNewTheme [const EYetiOsThemeMode] New theme.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS User Widget", DisplayName = "On Theme Changed")	
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);

	/**
	* protected UYetiOS_UserWidget::K2_OnColorSchemeChanged
	* Event called when color scheme was changed.
	* @param InNewColorScheme [const FName&] New color scheme name.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS User Widget", DisplayName = "On Color Scheme Changed")	
	void K2_OnColorSchemeChanged(const FName& InNewColorScheme);

public:

	FORCEINLINE class UYetiOS_Core* GetOwningOS() const { return OwningOS; }
	
};
