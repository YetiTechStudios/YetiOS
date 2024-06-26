// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Widgets/YetiOS_DeviceWidget.h"

#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "UnrealEd/Public/Dialogs/Dialogs.h"
#include "Components/SceneComponent.h"
#endif
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsDeviceManager, All, All)

#define printlog(Param1)		UE_LOG(LogYetiOsDeviceManager, Log, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)	UE_LOG(LogYetiOsDeviceManager, Warning, TEXT("%s"), *FString(Param1))

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

void AYetiOS_DeviceManagerActor::ShowBSOD(const UObject* WorldContextObject, class UYetiOS_BaseDevice* InDevice, const FYetiOsError& InErrorMessage)
{
	checkf(InDevice != nullptr, TEXT("Device cannot be null for BSOD."));
	InDevice->ShowBSOD(InErrorMessage.ErrorCode, InErrorMessage.ErrorException, InErrorMessage.ErrorDetailedException);
}

void AYetiOS_DeviceManagerActor::BeginPlay()
{
	Super::BeginPlay();

	if (bCreateDeviceOnBeginPlay)
	{
		FYetiOsError ErrorMessage;
		CreateDevice(ErrorMessage);
	}

	Internal_OnClockTimerTick();
	GetWorldTimerManager().SetTimer(TimerHandle_ClockTick, this, &AYetiOS_DeviceManagerActor::Internal_OnClockTimerTick, 1.f, true);
}

void AYetiOS_DeviceManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnClockTick.Clear();
	GetWorldTimerManager().ClearTimer(TimerHandle_ClockTick);
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

	printlog(FString::Printf(TEXT("Ending play on device manager %s. Reason: %s."), *GetName(), *EndReasonString));

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
	const bool bGC = CurrentDevice->CanGarbageCollect();
	CurrentDevice->ConditionalBeginDestroy();
	CurrentDevice = nullptr;
	K2_OnCurrentDeviceDestroyed();

	if (bGC && GEngine)
	{
		GEngine->ForceGarbageCollection(true);
		printlog_warn("Forced garbage collection on device destroyed.");
	}

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

void AYetiOS_DeviceManagerActor::RestartDevice()
{
	const bool bGC = CurrentDevice->CanGarbageCollect();
	CurrentDevice->ConditionalBeginDestroy();
	CurrentDevice = nullptr;	

	FTimerDelegate CreateDeviceDelegate;
	FTimerHandle TimerHandle_Dummy;

	FYetiOsError ErrorMessage;
	CreateDeviceDelegate.BindUFunction(this, FName("CreateDevice"), ErrorMessage);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dummy, CreateDeviceDelegate, 1.f, false);

	if (bGC && GEngine)
	{
		GEngine->ForceGarbageCollection(true);
		printlog_warn("Forced garbage collection on device restart.");
	}
}

void AYetiOS_DeviceManagerActor::CreateDevice(FYetiOsError& OutErrorMessage)
{
	if (CurrentDevice == nullptr && ensureMsgf(DeviceClass != nullptr, TEXT("Device class cannot be null")))
	{
		CurrentDevice = NewObject<UYetiOS_BaseDevice>(this, DeviceClass);
		CurrentDevice->OnCreateDevice();
		return;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_CurrentDeviceCreateErrorCode", "ERR_CREATE_DEVICE");
	OutErrorMessage.ErrorException = LOCTEXT("YetiOS_CurrentDeviceCreateException", "Current device was already created or Device class was null");
}

void AYetiOS_DeviceManagerActor::Internal_OnClockTimerTick()
{
	K2_OnClockTimerTick();
	OnClockTick.Broadcast();
}

#undef printlog
#undef printlog_warn
#undef LOCTEXT_NAMESPACE
