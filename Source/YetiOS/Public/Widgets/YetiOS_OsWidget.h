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

	UFUNCTION(BlueprintPure, Category = "Yeti OS Widget")
	const float GetDelayTime(const float InMin = 0.01, const float InMax = 0.2) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Widget")
	const int32 GetRaisedZ_Order() { return ZOrderCounter++; }

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Widget")
	const bool SetActiveUser(FYetiOsUser InNewUser);

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationStarted")
	void K2_OnInstallationStarted(const float& TimeToInstall);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnInstallationFinished")
	void K2_OnFinishInstallation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginLoadingOS")
	void K2_OnBeginLoadingOS();	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Widget", DisplayName = "OnBeginShuttingdownOS")
	void K2_OnBeginShuttingdownOS();

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintPure, Category = "Yeti OS Widget", DisplayName = "GetMousePositionOnViewport")
	FVector2D K2_GetMousePositionOnViewport(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "LoginUser")
	void K2_LoginUser(const FYetiOsUser& UserToLogin);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Widget", DisplayName = "CreateNewWindow")
	void K2_CreateNewWindow(class UYetiOS_BaseProgram* InProgram, class UYetiOS_AppWidget* InProgramWidget);
};
