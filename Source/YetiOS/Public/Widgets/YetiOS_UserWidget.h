// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class YETIOS_API UYetiOS_UserWidget : public UUserWidget
{
	GENERATED_BODY()

	FDelegateHandle OnThemeChangedDelegateHandle;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS User Widget", meta = (ExposeOnSpawn = "true"))
	class UYetiOS_Core* OwningOS;

	UPROPERTY()
	class UYetiOS_DraggableWindowWidget* OwningWindow;

public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS User Widget")
	void SetOperatingSystem(UYetiOS_Core* InOS);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS User Widget")
	void SetWindow(class UYetiOS_DraggableWindowWidget* InWindow) { OwningWindow = InWindow; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS User Widget")
	inline class UYetiOS_DraggableWindowWidget* GetOwningWindow() const { return OwningWindow; }

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS User Widget", DisplayName = "On Operating System Set")
	void K2_SetOperatingSystem(UYetiOS_Core* OS);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS User Widget", DisplayName = "On Theme Changed")
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);
	
};
