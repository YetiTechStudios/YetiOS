// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "YetiOS_OsWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_OsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_Core;

	FDelegateHandle OnThemeChangedDelegateHandle;
	
private:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseDevice* OwningDevice;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	int32 ZOrderCounter;
	
public:

	UYetiOS_OsWidget(const FObjectInitializer& ObjectInitializer);

private:

	static UYetiOS_OsWidget* Internal_CreateOsWidget(const UYetiOS_Core* OsCore);

public:

	void StartOsInstallation(const float& InTimeToInstall);
	void FinishOsInstallation();

	void BeginLoadOS();
	void BeginShutdownOS();
	void BeginRestartOS();

	void OnBatteryLevelChanged(const float& CurrentBatteryLevel);
	void OnLowBatteryWarningReceived(const bool bIsLowBattery);

	void AddTaskbarButton(class UYetiOS_DraggableWindowWidget* InWindowWidget);
	void RemoveTaskbarButton(class UYetiOS_DraggableWindowWidget* InWindowWidget);

	void ReceiveNotification(const FYetiOsNotification InNotification);

	UFUNCTION(BlueprintPure, Category = "Yeti OS Widget")
	const float GetDelayTime(const float InMin = 0.01, const float InMax = 0.2) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Widget")
	const int32 GetRaisedZ_Order() { return ZOrderCounter++; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")
	const bool SetActiveUser(FYetiOsUser InNewUser);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")
	bool ChangePassword(UPARAM(ref) FYetiOsUser& InNewUser, const FText InNewPassword);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")
	void PlayNotificationSound(const FYetiOsNotification& InNotification, const float InVolume = 1.f);

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationStarted")
	void K2_OnInstallationStarted(const float& TimeToInstall);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationFinished")
	void K2_OnFinishInstallation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginLoadingOS")
	void K2_OnBeginLoadingOS();	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginShuttingdownOS")
	void K2_OnBeginShuttingdownOS();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginRestartingOS")
	void K2_OnBeginRestartingOS();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnWindowChangeFromTaskbar")
	void K2_OnWindowChangeFromTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget, const bool bAddedToTaskbar);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnReceivedNotification")
	void K2_OnReceivedNotification(const FYetiOsNotification NewNotification);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget|Portable", DisplayName = "OnBatteryLevelChanged")
	void K2_OnBatteryLevelChanged(const float& CurrentBatteryLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget|Portable", DisplayName = "OnLowBatteryWarningReceived")
	void K2_OnLowBatteryWarningReceived(const bool bIsLowBattery);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "On Theme Changed")
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintPure, Category = "Yeti OS Widget", DisplayName = "GetMousePositionOnViewport")
	FVector2D K2_GetMousePositionOnViewport(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "LoginUser")
	void K2_LoginUser(const FYetiOsUser& UserToLogin);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "CreateNewWindow")
	void K2_CreateNewWindow(class UYetiOS_BaseProgram* InProgram, class UYetiOS_AppWidget* InProgramWidget);
};
