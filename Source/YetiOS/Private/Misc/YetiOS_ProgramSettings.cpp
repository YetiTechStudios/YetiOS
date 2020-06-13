// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Misc/YetiOS_ProgramSettings.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsProgramSettings, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsProgramSettings, Log, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsProgramSettings, Error, TEXT("%s"), *FString(Param1))


UYetiOS_ProgramSettings::UYetiOS_ProgramSettings()
{
	ParentProgramIdentifier = NAME_None;
	SaveSlotName = "";
	UserIndex = 0;
}

UYetiOS_ProgramSettings* UYetiOS_ProgramSettings::CreateSettings(const class UYetiOS_BaseProgram* InParentProgram, TSubclassOf<UYetiOS_ProgramSettings> InSettingsClass)
{
	if (InParentProgram->GetProgramIdentifierName().IsNone())
	{
		printlog_error(FString::Printf(TEXT("Program settings cannot be created for %s. Missing program identifier."), *InParentProgram->GetName()));
		return nullptr;
	}

	UYetiOS_ProgramSettings* ProxyProgram = NewObject<UYetiOS_ProgramSettings>(const_cast<UYetiOS_BaseProgram*>(InParentProgram), InSettingsClass);
	ProxyProgram->ParentProgramIdentifier = InParentProgram->GetProgramIdentifierName();
	ProxyProgram->Internal_SetSaveSlotName();
	return ProxyProgram;
}

UYetiOS_ProgramSettings* UYetiOS_ProgramSettings::LoadSettings(const class UYetiOS_BaseProgram* InParentProgram)
{
	UYetiOS_ProgramSettings* Local_LoadInstance = Cast<UYetiOS_ProgramSettings>(UGameplayStatics::CreateSaveGameObject(UYetiOS_ProgramSettings::StaticClass()));
	const FString LoadSlotName = InParentProgram->GetProgramIdentifierName().ToString();
	const int32 LoadUserIndex = Local_LoadInstance->UserIndex;

	if (UGameplayStatics::DoesSaveGameExist(LoadSlotName, LoadUserIndex))
	{
		Local_LoadInstance = Cast<UYetiOS_ProgramSettings>(UGameplayStatics::LoadGameFromSlot(LoadSlotName, LoadUserIndex));
		return Local_LoadInstance;
	}

	return nullptr;
}

bool UYetiOS_ProgramSettings::SaveSettings()
{
	if (K2_CanSave())
	{
		printlog(FString::Printf(TEXT("Beginning to save settings for %s"), *ParentProgramIdentifier.ToString()));
		K2_PreSave();
		return UGameplayStatics::SaveGameToSlot(this, SaveSlotName, UserIndex);
	}

	printlog_error(FString::Printf(TEXT("Failed to save settings for %s"), *ParentProgramIdentifier.ToString()));
	return false;
}

void UYetiOS_ProgramSettings::Destroy()
{
	ParentProgramIdentifier = NAME_None;
	ConditionalBeginDestroy();
}

void UYetiOS_ProgramSettings::Internal_SetSaveSlotName()
{
	SaveSlotName = ParentProgramIdentifier.ToString(); // FString::Printf(TEXT("%s_%s"), *ParentProgramIdentifier.ToString(), FGuid::NewGuid().ToString(EGuidFormats::Short));
}

bool UYetiOS_ProgramSettings::K2_CanSave_Implementation() const
{
	if (ParentProgramIdentifier.IsNone() == false)
	{
		const UYetiOS_BaseProgram* Local_ParentProgram = Cast<UYetiOS_BaseProgram>(GetOuter());
		const AYetiOS_DeviceManagerActor* Local_DeviceManager = Cast<AYetiOS_DeviceManagerActor>(Local_ParentProgram->GetOwningOS()->GetOwningDevice()->GetOuter());
		return Local_DeviceManager->CanSaveGame();
	}

	return false;
}

#undef printlog_error
