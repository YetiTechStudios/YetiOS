// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Programs/YetiOS_AppInstaller.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "UObject/ConstructorHelpers.h"

UYetiOS_AppInstaller::UYetiOS_AppInstaller()
{
	ProgramName = FText::AsCultureInvariant("Application Installer");
	ProgramIdentifier = FName("appinstaller");
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconObject(TEXT("/YetiOS/DefaultIcons/Programs/T_UI_AppInstaller"));
	ProgramIcon = IconObject.Object;

	bAddToDesktop = bCreateInstallationNotification = false;
	bSingleInstanceOnly = true;
	SaveMethod = EProgramSaveMethod::DoNotSave;
	bSupportStore = true;

	static ConstructorHelpers::FClassFinder<UYetiOS_AppIconWidget> IconWidgetClass(TEXT("/YetiOS/Blueprints/Widgets/OS_Addons/ProgramWidgets/WBP_Program_Icon"));
	ProgramIconWidgetClass = IconWidgetClass.Class;
		
	static ConstructorHelpers::FClassFinder<UYetiOS_AppWidget> ProgramWidgetClassRef(TEXT("/YetiOS/Blueprints/Widgets/OS_Addons/ProgramWidgets/ApplicationInstaller/WBP_ApplicationInstaller"));
	ProgramWidgetClass = ProgramWidgetClassRef.Class;
}

void UYetiOS_AppInstaller::DownloadSetup(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_AppInstaller> InstallerClass, float TimeToExecuteCallback, const FOnDownloadComplete& Callback)
{
	FTimerDelegate OnDone;	

	OnDone.BindLambda([Callback, InOS, InstallerClass]
	{
		if (InOS && InstallerClass)
		{
			const UYetiOS_DirectoryRoot* RootDir = InOS->GetRootDirectory();
			if (RootDir)
			{				
				if (UYetiOS_DirectoryBase* DownloadsDirectory = RootDir->GetChildDirectoryByType(EDirectoryType::Downloads))
				{
					FYetiOsError OutErrorMessage;
					UYetiOS_AppInstaller* NewProgram = UYetiOS_BaseProgram::CreateProgram<UYetiOS_AppInstaller>(InOS, InstallerClass, OutErrorMessage, false);
					if (UYetiOS_DirectoryBase::AddProgramToDirectory(DownloadsDirectory, NewProgram))
					{
						Callback.Execute(true, NewProgram);
						return;
					}
				}
			}
		}

		Callback.Execute(false, nullptr);
	});

	if (TimeToExecuteCallback > KINDA_SMALL_NUMBER)
	{
		FTimerHandle DummyHandle;
		InOS->GetWorld()->GetTimerManager().SetTimer(DummyHandle, OnDone, TimeToExecuteCallback, false);
	}
	else
	{
		OnDone.Execute();
	}
}

bool UYetiOS_AppInstaller::GetEulaText(FText& OutEula) const
{
	if (EULA_Text.IsEmptyOrWhitespace())
	{
		OutEula = FText::GetEmpty();
		return false;
	}

	OutEula = EULA_Text;
	return true;
}
