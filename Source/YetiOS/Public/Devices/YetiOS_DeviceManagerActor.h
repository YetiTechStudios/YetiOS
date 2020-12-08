// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YetiOS_Types.h"
#include "YetiOS_DeviceManagerActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClockTimerTick);

/*************************************************************************
* File Information:
YetiOS_DeviceManagerActor.h

* Description:
Main actor where everything begins.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, hideCategories = (Rendering, Input, Actor, LOD, Cooking, Collision, Tick, Replication))
class YETIOS_API AYetiOS_DeviceManagerActor : public AActor
{
	GENERATED_BODY()

	FTimerHandle TimerHandle_ClockTick;
	
private:

	UPROPERTY()
	class USceneComponent* SceneComponent;

	/** If true create the device on BeginPlay Event. If false, user can manually create the device by calling CreateDevice() method. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Device Manager Actor")
	uint8 bCreateDeviceOnBeginPlay : 1;

	/** If true then add widget automatically on screen when device is created. Other wise manually add it via OnWidgetChanged event in Blueprints. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Device Manager Actor")
	uint8 bAddWidgetsToScreen : 1;

	/** Execute "exit" command when device is destroyed. On Non-Editor build, this will request a platform clean exit. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Device Manager Actor")
	uint8 bExitGameWhenDeviceIsDestroyed : 1;

	/** If true, then support save game functionality. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Device Manager Actor", AdvancedDisplay)
	uint8 bCanSaveGame : 1;

	/** Device class to create. */	
	UPROPERTY(EditAnywhere, Category = "Yeti OS Device Manager Actor")
	TSubclassOf<class UYetiOS_BaseDevice> DeviceClass;	

	/** Reference to the current device that was created. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseDevice* CurrentDevice;
	
public:	

	/** Delegate that ticks every second until EndPlay is called. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Device Manager|Delegates")
	FOnClockTimerTick OnClockTick;
	
	AYetiOS_DeviceManagerActor();

	/**
	* public static AYetiOS_DeviceManagerActor::ShowBSOD
	* Shows the Blue Screen widget.	
	* @param InFaultingModuleName [const FText] A fake module name. Can be error code also (if any).
	* @param InExceptionName [const FText] Exception name (if any).
	* @param InDetailedException [const FText] Detailed exception name (if any).
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Global", meta = (WorldContext = "WorldContextObject"))	
	static void ShowBSOD(const UObject* WorldContextObject, class UYetiOS_BaseDevice* InDevice, const FYetiOsError& InErrorMessage);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	* public AYetiOS_DeviceManagerActor::OnWidgetChanged
	* This function is called whenever the widget is changed on device.
	* @See: UYetiOS_BaseDevice::ChangeOnScreenWidget
	* @param InNewWidget [class UUserWidget *] Widget to change to.
	**/
	void OnWidgetChanged(class UUserWidget* InNewWidget);

	/**
	* public AYetiOS_DeviceManagerActor::OnCurrentDeviceDestroyed
	* This function is called when the device is destroyed.
	* @See: UYetiOS_BaseDevice::DestroyYetiDevice
	**/
	void OnCurrentDeviceDestroyed();

	/**
	* public AYetiOS_DeviceManagerActor::RestartDevice
	* Restarts the current device.
	**/
	void RestartDevice();

protected:

	/**
	* protected AYetiOS_DeviceManagerActor::CreateDevice
	* Creates a new device. This will trigger an ensure assert if Device class is null.
	* @param OutErrorMessage [FYetiOsError &] Outputs any error message (if any)
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Device Manager")	
	void CreateDevice(FYetiOsError& OutErrorMessage);

public:

	/**
	* public AYetiOS_DeviceManagerActor::GetDevice const
	* Returns the device that was created by this manager.
	* @return [class UYetiOS_BaseDevice*] Current Device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Device Manager")	
	class UYetiOS_BaseDevice* GetDevice() const { return CurrentDevice; }

protected:
		
	/**
	* protected AYetiOS_DeviceManagerActor::K2_OnWidgetChanged
	* Event called when widget is changed.
	* @See: OnWidgetChanged() function.
	* @param NewWidget [const class UUserWidget*] Newly changed widget.
	**/	
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti Device Manager", DisplayName = "OnWidgetChanged")	
	void K2_OnWidgetChanged(const class UUserWidget* NewWidget);

	/**
	* protected AYetiOS_DeviceManagerActor::K2_OnCurrentDeviceDestroyed
	* Event called when current device is destroyed.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti Device Manager", DisplayName = "OnCurrentDeviceDestroyed")	
	void K2_OnCurrentDeviceDestroyed();

	/**
	* protected AYetiOS_DeviceManagerActor::K2_OnClockTimerTick
	* Event called every second as long as this device manager is alive.
	* @See Internal_OnClockTimerTick
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti Device Manager", DisplayName = "On Clock Timer Tick")
	void K2_OnClockTimerTick();

private:

	/**
	* private AYetiOS_DeviceManagerActor::Internal_OnClockTimerTick
	* Automatically called every second from BeginPlay until EndPlay.
	**/
	UFUNCTION()	
	void Internal_OnClockTimerTick();

public:

	/**
	* public AYetiOS_DeviceManagerActor::AddWidgetsToScreen const
	* True if widgets can be added to screen.
	* @return [FORCEINLINE const bool] Returns bAddWidgetsToScreen.
	**/
	FORCEINLINE const bool AddWidgetsToScreen() const { return bAddWidgetsToScreen; }
	FORCEINLINE const bool CanSaveGame() const { return bCanSaveGame; }
};
