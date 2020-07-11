// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Widgets/YetiOS_DeviceWidget.h"

#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "UnrealEd/Public/Dialogs/Dialogs.h"
#include "Components/SceneComponent.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsDeviceManager, All, All)

#define printlog_display(Param1)		UE_LOG(LogYetiOsDeviceManager, Display, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

AYetiOS_DeviceManagerActor::AYetiOS_DeviceManagerActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	bCreateDeviceOnBeginPlay = true;
	bAddWidgetsToScreen = true;
	bExitGameWhenDeviceIsDestroyed = false;
	bCanSaveGame = true;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

AYetiOS_DeviceManagerActor* AYetiOS_DeviceManagerActor::GetDeviceManager(const UObject* WorldContextObject)
{
	ensureAlwaysMsgf(false, TEXT("GetDeviceManager has been deprecated. This function will only return null."));
	return nullptr;
}

class UYetiOS_BaseDevice* AYetiOS_DeviceManagerActor::GetCurrentDevice(const UObject* WorldContextObject)
{
	ensureAlwaysMsgf(false, TEXT("GetCurrentDevice has been deprecated. This function will only return null."));
	return nullptr;
}

void AYetiOS_DeviceManagerActor::ShowBSOD(const UObject* WorldContextObject, class UYetiOS_BaseDevice* InDevice, const FText InFaultingModuleName /*= FText::GetEmpty()*/, const FText InExceptionName /*= FText::GetEmpty()*/, const FText InDetailedException /*= FText::GetEmpty()*/)
{
	InDevice->ShowBSOD(InFaultingModuleName, InExceptionName, InDetailedException);
}

void AYetiOS_DeviceManagerActor::BeginPlay()
{
	Super::BeginPlay();

	if (bCreateDeviceOnBeginPlay)
	{
		FYetiOsError ErrorMessage;
		CreateDevice(ErrorMessage);
	}
}

void AYetiOS_DeviceManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FString EndReasonString = "Unknown";

	switch (EndPlayReason)
	{
		case EEndPlayReason::Destroyed:
			EndReasonString = "Destroyed";
			break;
		case EEndPlayReason::LevelTransition:
			EndReasonString = "Level Transition";
			break;
		case EEndPlayReason::EndPlayInEditor:
			EndReasonString = "End Play In Editor";
			break;
		case EEndPlayReason::RemovedFromWorld:
			EndReasonString = "Removed From World";
			break;
		case EEndPlayReason::Quit:
			EndReasonString = "Quit";
			break;
		default:
			break;
	}

	printlog_display(FString::Printf(TEXT("Ending play on device manager %s. Reason: %s."), *GetName(), *EndReasonString));

	if (CurrentDevice)
	{
		CurrentDevice->DestroyYetiDevice();
		CurrentDevice = nullptr;
	}
}

void AYetiOS_DeviceManagerActor::OnWidgetChanged(class UUserWidget* InNewWidget)
{
	K2_OnWidgetChanged(InNewWidget);
}

void AYetiOS_DeviceManagerActor::OnCurrentDeviceDestroyed()
{
	CurrentDevice = nullptr;
	K2_OnCurrentDeviceDestroyed();
	if (bExitGameWhenDeviceIsDestroyed)
	{
#if WITH_EDITOR
		APlayerController* MyController = UGameplayStatics::GetPlayerController(this, 0);
		if (MyController)
		{
			MyController->ConsoleCommand("exit");
		}
#else
		FGenericPlatformMisc::RequestExit(false);
#endif
	}
}

void AYetiOS_DeviceManagerActor::RestartDevice(class UYetiOS_BaseDevice* InDevice)
{
	check(InDevice == CurrentDevice);
	CurrentDevice = nullptr;

	FTimerDelegate CreateDeviceDelegate;
	FTimerHandle TimerHandle_Dummy;

	FYetiOsError ErrorMessage;
	CreateDeviceDelegate.BindUFunction(this, FName("CreateDevice"), ErrorMessage);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dummy, CreateDeviceDelegate, 1.f, false);
}

void AYetiOS_DeviceManagerActor::CreateDevice(FYetiOsError& OutErrorMessage)
{
	if (CurrentDevice == nullptr && ensureMsgf(DeviceClass != nullptr, TEXT("Device class cannot be null")))
	{
		CurrentDevice = NewObject<UYetiOS_BaseDevice>(this, DeviceClass);
		CurrentDevice->OnCreateDevice(OutErrorMessage);
		return;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_CurrentDeviceCreateErrorCode", "ERR_CREATE_DEVICE");
	OutErrorMessage.ErrorException = LOCTEXT("YetiOS_CurrentDeviceCreateException", "Current device was already created or Device class was null");
}

#undef printlog_display
#undef LOCTEXT_NAMESPACE
