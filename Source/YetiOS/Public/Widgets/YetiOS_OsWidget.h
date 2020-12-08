// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "YetiOS_Types.h"
#include "YetiOS_OsWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_OsWidget.h

* Description:
Widget that represents the Operating System.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Operating System Widget")
class YETIOS_API UYetiOS_OsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_Core;

	FDelegateHandle OnThemeChangedDelegateHandle;
	
private:

	/** Operating system that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	/** Devices that owns the Operating System */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseDevice* OwningDevice;

	/** Current Z Order */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Widget", meta = (AllowPrivateAccess = "true"))
	int32 ZOrderCounter;
	
public:

	UYetiOS_OsWidget(const FObjectInitializer& ObjectInitializer);

private:

	/**
	* private static UYetiOS_OsWidget::Internal_CreateOsWidget
	* Creates the Operating System Widget.
	* @param OsCore [const UYetiOS_Core*] Operating System that owns this widget.
	* @return [UYetiOS_OsWidget*] Newly created widget.
	**/
	static UYetiOS_OsWidget* Internal_CreateOsWidget(const UYetiOS_Core* OsCore);

public:

	/**
	* public UYetiOS_OsWidget::StartOsInstallation
	* Start simulating Operating System installation.
	* @param InTimeToInstall [const float&] Simulated time to install.
	**/
	void StartOsInstallation(const float& InTimeToInstall);

	/**
	* public UYetiOS_OsWidget::FinishOsInstallation
	* Finish simulation of Operating System installation.
	**/
	void FinishOsInstallation();

	/**
	* public UYetiOS_OsWidget::BeginLoadOS
	* Begin simulating loading of Operating System.
	**/
	void BeginLoadOS();

	/**
	* public UYetiOS_OsWidget::BeginShutdownOS
	* Begin simulating shutting down of Operating System.
	**/
	void BeginShutdownOS();

	/**
	* public UYetiOS_OsWidget::BeginRestartOS
	* Begin simulating restarting of Operating System.
	**/
	void BeginRestartOS();

	/**
	* public UYetiOS_OsWidget::AddIconWidgetToDesktop
	* Add the given icon widget to desktop
	* @param InAppIconWidget [UYetiOS_AppIconWidget*] Icon widget.
	**/
	void AddIconWidgetToDesktop(UYetiOS_AppIconWidget* InAppIconWidget);

	/**
	* public UYetiOS_OsWidget::RemoveDesktopShortcut
	* Removes the given icon widget from desktop
	* @param InAppIconWidget [UYetiOS_AppIconWidget*] Icon widget
	**/
	void RemoveDesktopShortcut(UYetiOS_AppIconWidget* InAppIconWidget);

	/**
	* public UYetiOS_OsWidget::OnBatteryLevelChanged
	* Called when battery has changed.
	* @param CurrentBatteryLevel [const float&] New battery level.
	**/
	void OnBatteryLevelChanged(const float& CurrentBatteryLevel);

	/**
	* public UYetiOS_OsWidget::OnLowBatteryWarningReceived
	* Function called when on low battery warning received.
	* @param bIsLowBattery [const bool] Is low battery
	**/
	void OnLowBatteryWarningReceived(const bool bIsLowBattery);

	/**
	* public UYetiOS_OsWidget::ReceiveNotification
	* Receive a notification from the Operating System.
	* @param InNotification [const FYetiOsNotification] Notification that was received.
	**/
	void ReceiveNotification(const FYetiOsNotification InNotification);

	/**
	* public UYetiOS_OsWidget::CreateNewWindow
	* Create a new draggable window for this widget.
	* @param InProgram [class UYetiOS_BaseProgram*] Program that requested new window.
	* @param InWidget [class UYetiOS_UserWidget*] Widget to be added to draggable window.
	* @param OverrideSize [const FVector2D&] If the size needs to be overridden.
	* @return [class UYetiOS_DraggableWindowWidget*] New draggable window.
	**/
	class UYetiOS_DraggableWindowWidget* CreateNewWindow(class UYetiOS_BaseProgram* InProgram, class UYetiOS_UserWidget* InWidget, const FVector2D& OverrideSize);

	/**
	* public UYetiOS_OsWidget::GetDelayTime const
	* Returns a random time between min and max (both inclusive).
	* @return [const float] New time.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Widget")	
	const float GetDelayTime(const float InMin = 0.01, const float InMax = 0.2) const;

	/**
	* public UYetiOS_OsWidget::SetActiveUser
	* Sets the active user of this Operating System.
	* @param InNewUser [FYetiOsUser] User to switch to.
	* @return [const bool] True if user was active.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")	
	const bool SetActiveUser(FYetiOsUser InNewUser);

	/**
	* public UYetiOS_OsWidget::ChangePassword
	* Changes the password for the given user.
	* @param InNewUser [FYetiOsUser&] User that needs password change.
	* @param InNewPassword [const FText] New password.
	* @return [bool] True if password was changed.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")	
	bool ChangePassword(UPARAM(ref) FYetiOsUser& InNewUser, const FText InNewPassword);

	/**
	* public UYetiOS_OsWidget::PlayNotificationSound
	* Plays a notification sound. Sounds are defined in Operating System.
	* @param InNotification [const FYetiOsNotification&] Notification to play sound.
	* @param InVolume [const float] Volume level.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Widget")
	void PlayNotificationSound(const FYetiOsNotification& InNotification, const float InVolume = 1.f);

protected:

	/**
	* protected UYetiOS_OsWidget::K2_OnInstallationStarted
	* Event called when Operating System installation has started.
	* @param TimeToInstall [const float&] Time required to install
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationStarted")
	void K2_OnInstallationStarted(const float& TimeToInstall);
	
	/**
	* protected UYetiOS_OsWidget::K2_OnFinishInstallation
	* Event called when Operating System installation finished.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationFinished")	
	void K2_OnFinishInstallation();

	/**
	* protected UYetiOS_OsWidget::K2_OnBeginLoadingOS
	* Event called when OS begins loading.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginLoadingOS")	
	void K2_OnBeginLoadingOS();	

	/**
	* protected UYetiOS_OsWidget::K2_OnAddDesktopShortcut
	* Event called when app icon widget is added to desktop.
	* @param AppIconWidget [UYetiOS_AppIconWidget*] App icon widget.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnAddDesktopShortcut")
	void K2_OnAddDesktopShortcut(UYetiOS_AppIconWidget* AppIconWidget);

	/**
	* protected UYetiOS_OsWidget::K2_OnRemoveDesktopShortcut
	* Event called when app icon is removed from desktop.
	* @param AppIconWidget [UYetiOS_AppIconWidget *] App icon widget.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnRemoveDesktopShortcut")	
	void K2_OnRemoveDesktopShortcut(UYetiOS_AppIconWidget* AppIconWidget);

	/**
	* protected UYetiOS_OsWidget::K2_OnBeginShuttingdownOS
	* Event called when Operating System begins shutting down.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginShuttingdownOS")
	void K2_OnBeginShuttingdownOS();

	/**
	* protected UYetiOS_OsWidget::K2_OnBeginRestartingOS
	* Event called when Operating System begins restarting.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginRestartingOS")	
	void K2_OnBeginRestartingOS();

	/**
	* protected UYetiOS_OsWidget::K2_OnReceivedNotification
	* Event called when a notification is received.
	* @param NewNotification [const FYetiOsNotification] Notification received.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnReceivedNotification")	
	void K2_OnReceivedNotification(const FYetiOsNotification NewNotification);

	/**
	* protected UYetiOS_OsWidget::K2_OnBatteryLevelChanged
	* Event called when battery level changed.
	* @param CurrentBatteryLevel [const float&] new level.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget|Portable", DisplayName = "OnBatteryLevelChanged")	
	void K2_OnBatteryLevelChanged(const float& CurrentBatteryLevel);

	/**
	* protected UYetiOS_OsWidget::K2_OnLowBatteryWarningReceived
	* Event called when low battery warning received.
	* @param bIsLowBattery [const bool] Is low battery.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget|Portable", DisplayName = "OnLowBatteryWarningReceived")
	void K2_OnLowBatteryWarningReceived(const bool bIsLowBattery);

	/**
	* protected UYetiOS_OsWidget::K2_OnThemeChanged
	* Event called when theme is changed.
	* @param InNewTheme [const EYetiOsThemeMode] New theme
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "On Theme Changed")	
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);

public:

	/**
	* public UYetiOS_OsWidget::K2_GetMousePositionOnViewport
	* Event called to get information about the location of mouse pointer.
	* @return [FVector2D] Mouse pointer location
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintPure, Category = "Yeti OS Widget", DisplayName = "GetMousePositionOnViewport")	
	FVector2D K2_GetMousePositionOnViewport(const FPointerEvent& InMouseEvent);

	/**
	* public UYetiOS_OsWidget::K2_LoginUser
	* Event called when user wants to login.
	* @param UserToLogin [const FYetiOsUser&] user to login
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "LoginUser")	
	void K2_LoginUser(const FYetiOsUser& UserToLogin);

protected:

	/**
	* protected UYetiOS_OsWidget::K2_CreateWindow
	* Event called when a draggable window is created.
	* @param InProgram [class UYetiOS_BaseProgram*] Program that requested new window.
	* @param InWidget [class UYetiOS_UserWidget*] Widget to be added as child.
	* @param OverrideSize [const FVector2D&] Override window size.
	* @return [class UYetiOS_DraggableWindowWidget*] Newly created window widget.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "CreateWindow")	
	class UYetiOS_DraggableWindowWidget* K2_CreateWindow(class UYetiOS_BaseProgram* InProgram, class UYetiOS_UserWidget* InWidget, const FVector2D& OverrideSize);
};
