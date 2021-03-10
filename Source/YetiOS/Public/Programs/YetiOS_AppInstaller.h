// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/YetiOS_BaseProgram.h"
#include "YetiOS_AppInstaller.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDownloadComplete, const bool, bSuccess, UYetiOS_AppInstaller*, DownloadedSetup);

UCLASS(Abstract, DisplayName = "Application Installer")
class YETIOS_API UYetiOS_AppInstaller : public UYetiOS_BaseProgram
{
	GENERATED_BODY()

private:

	/** The program this installer should install */
	UPROPERTY(EditDefaultsOnly, Category = "Application Installer")
	TSubclassOf<UYetiOS_BaseProgram> TargetProgram;

	/** Some EULA (End User License Agreement) text. */
	UPROPERTY(EditDefaultsOnly, Category = "Application Installer", meta = (MultiLine = "true"))
	FText EULA_Text;

public:

	UYetiOS_AppInstaller();

	UFUNCTION(BlueprintCallable, Category = "Yeti Global")
	static void DownloadSetup(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_AppInstaller> InstallerClass, float TimeToExecuteCallback, const FOnDownloadComplete& Callback);

	UFUNCTION(BlueprintPure, Category = "Application Installer")
	inline TSubclassOf<UYetiOS_BaseProgram> GetTargetProgram() const { return TargetProgram; }
	
	UFUNCTION(BlueprintPure, Category = "Application Installer")
	bool GetEulaText(FText& OutEula) const;
};
