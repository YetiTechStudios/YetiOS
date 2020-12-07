// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "YetiOS_Factories.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"

#define YETI_CREATE_BLUEPRINT(ClassName)\
{\
		UClass* BlueprintClass = nullptr;\
		UClass* BlueprintGeneratedClass = nullptr;\
		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");\
		KismetCompilerModule.GetBlueprintTypesForClass(ClassName::StaticClass(), BlueprintClass, BlueprintGeneratedClass);\
		return FKismetEditorUtilities::CreateBlueprint(ClassName::StaticClass(), InParent, InName, BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, CallingContext);\
 }

#define YETI_CREATE_OBJECT(ClassName)\
{\
	check(InClass->IsChildOf(ClassName::StaticClass()));\
	return NewObject<ClassName>(InParent, InClass, InName, Flags|RF_Transactional, Context);\
}

UYetiOS_BaseProgram_Factory::UYetiOS_BaseProgram_Factory()
{
	SupportedClass = UYetiOS_BaseProgram::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_BaseProgram_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_BaseProgram);
}

UYetiOS_OperatingSystem_Factory::UYetiOS_OperatingSystem_Factory()
{
	SupportedClass = UYetiOS_Core::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_OperatingSystem_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_Core);
}

UYetiOS_BaseFile_Factory::UYetiOS_BaseFile_Factory()
{
	SupportedClass = UYetiOS_FileBase::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_BaseFile_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_FileBase);
}

UYetiOS_BaseDirectory_Factory::UYetiOS_BaseDirectory_Factory()
{
	SupportedClass = UYetiOS_DirectoryBase::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_BaseDirectory_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_DirectoryBase);
}

UYetiOS_PortableDevice_Factory::UYetiOS_PortableDevice_Factory()
{
	SupportedClass = UYetiOS_PortableDevice::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_PortableDevice_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_PortableDevice);
}

UYetiOS_StationaryDevice_Factory::UYetiOS_StationaryDevice_Factory()
{
	SupportedClass = UYetiOS_StationaryDevice::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_StationaryDevice_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(UYetiOS_StationaryDevice);
}

UYetiOS_DeviceManager_Factory::UYetiOS_DeviceManager_Factory()
{
	SupportedClass = AYetiOS_DeviceManagerActor::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_DeviceManager_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_BLUEPRINT(AYetiOS_DeviceManagerActor);
}

UYetiOS_ProgramsRepository_Factory::UYetiOS_ProgramsRepository_Factory()
{
	SupportedClass = UYetiOS_ProgramsRepository::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_ProgramsRepository_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_OBJECT(UYetiOS_ProgramsRepository);
}

UYetiOS_SystemSettings_Factory::UYetiOS_SystemSettings_Factory()
{
	SupportedClass = UYetiOS_SystemSettings::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

UObject* UYetiOS_SystemSettings_Factory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	YETI_CREATE_OBJECT(UYetiOS_SystemSettings);
}
