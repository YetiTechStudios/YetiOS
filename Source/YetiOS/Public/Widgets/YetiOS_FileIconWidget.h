// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_IconWidget.h"
#include "YetiOS_FileIconWidget.generated.h"


UCLASS(Abstract)
class YETIOS_API UYetiOS_FileIconWidget : public UYetiOS_IconWidget
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Yeti OS File Icon Widget", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UYetiOS_FileBase> FileClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* BaseFile;

public:

	UYetiOS_FileIconWidget(const FObjectInitializer& ObjectInitializer);

	static UYetiOS_FileIconWidget* CreateFileIconWidget(class UYetiOS_FileBase* InFile, FYetiOsError& OutErrorMessage);

	void AssociatedProgramInstalled();
	
	UFUNCTION(BlueprintCallable, Category = "Yeti OS File Icon Widget")
	bool OpenFile(FYetiOsError& OutErrorMessage);

	UFUNCTION(BlueprintPure, Category = "Yeti OS File Icon Widget")
	bool IsAssociatedProgramInstalled() const;

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS File Icon Widget", DisplayName = "On Associated Program Installed")
	void K2_OnAssociatedProgramInstalled();
};
