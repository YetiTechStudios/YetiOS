// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_BsodWidget.h"
#include "Kismet/GameplayStatics.h"

UYetiOS_BsodWidget* UYetiOS_BsodWidget::CreateBsodWidget(const UObject* WorldContextObject, TSubclassOf<class UYetiOS_BsodWidget> InBsodWidgetClass, const FText InFaultingModuleName /*= FText::GetEmpty()*/, const FText InExceptionName /*= FText::GetEmpty()*/, const FText InDetailedException /*= FText::GetEmpty()*/)
{
	APlayerController* MyController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	UYetiOS_BsodWidget* ProxyBsod = CreateWidget<UYetiOS_BsodWidget>(MyController, InBsodWidgetClass);
	ProxyBsod->FaultingModuleName = InFaultingModuleName;
	ProxyBsod->ExceptionName = InExceptionName;
	ProxyBsod->DetailedExceptionName = InDetailedException;
	return ProxyBsod;
}

const FText UYetiOS_BsodWidget::GenerateRandomErrorCode()
{
	static TArray<FString> CodeArray;
	CodeArray.SetNum(16);
	CodeArray.Add("0");
	CodeArray.Add("1");
	CodeArray.Add("2");
	CodeArray.Add("3");
	CodeArray.Add("4");
	CodeArray.Add("5");
	CodeArray.Add("6");
	CodeArray.Add("7");
	CodeArray.Add("8");
	CodeArray.Add("9");
	CodeArray.Add("A");
	CodeArray.Add("B");
	CodeArray.Add("C");
	CodeArray.Add("D");
	CodeArray.Add("E");
	CodeArray.Add("F");
	FString StopCode = "0x";
	for (const auto& It : CodeArray)
	{
		const int32 RandomIndex = FMath::RandRange(0, CodeArray.Num() - 1);
		StopCode = StopCode + CodeArray[RandomIndex];
	}

	return FText::FromString(StopCode);
}
