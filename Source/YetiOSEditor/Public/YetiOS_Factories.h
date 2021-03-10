// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetTypeActions_Base.h"
#include "Programs/YetiOS_AppInstaller.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Core/YetiOS_FileBase.h"
#include "Devices/YetiOS_PortableDevice.h"
#include "Devices/YetiOS_StationaryDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Misc/YetiOS_ProgramsRepository.h"
#include "Misc/YetiOS_SystemSettings.h"
#include "YetiOS_Factories.generated.h"

static EAssetTypeCategories::Type YetiOS_AssetCategory;

class FAssetTypeActions_YetiOsBase : public FAssetTypeActions_Base
{
	virtual uint32 GetCategories() override { return YetiOS_AssetCategory; }
	virtual FColor GetTypeColor() const override { return FColor(127, 255, 255); }
};

class FAssetTypeActions_BaseDirectory : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Directory"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Directory which you can use in your Operating System."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_DirectoryBase::StaticClass(); }
};

class FAssetTypeActions_BaseFile : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New File"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new File which you can use in your Directory."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_FileBase::StaticClass(); }
};

class FAssetTypeActions_BaseProgram : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Program"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Program which you can use for your Operating System."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_BaseProgram::StaticClass(); }
};

class FAssetTypeActions_AppInstaller : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Installer"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new installer which you can use to simulate setup."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_AppInstaller::StaticClass(); }
};

class FAssetTypeActions_OperatingSystem : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Operating System"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Operating System which you can use for your Device."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_Core::StaticClass(); }
};

class FAssetTypeActions_PortableDevice : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Portable Device"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Portable Device which you can use in your Device Manager."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_PortableDevice::StaticClass(); }
};

class FAssetTypeActions_StationaryDevice : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Stationary Device"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Stationary Device which you can use in your Device Manager."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_StationaryDevice::StaticClass(); }
};

class FAssetTypeActions_DeviceManager : public FAssetTypeActions_YetiOsBase
{
	virtual FText GetName() const override { return FText::FromString("New Device Manager"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs a new Device Manager which starts your device."); }
	virtual UClass* GetSupportedClass() const override { return AYetiOS_DeviceManagerActor::StaticClass(); }
};

class FAssetTypeActions_ProgramsRepository : public FAssetTypeActions_YetiOsBase
{
	virtual FColor GetTypeColor() const override { return FColor(56, 43, 21); }
	virtual FText GetName() const override { return FText::FromString("Operating System Programs Collection"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs an asset where you can put all your programs for your Operating System."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_ProgramsRepository::StaticClass(); }
};

class FAssetTypeActions_SystemSettings : public FAssetTypeActions_YetiOsBase
{
	virtual FColor GetTypeColor() const override { return FColor(112, 134, 156); }
	virtual FText GetName() const override { return FText::FromString("Operating System Settings"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Constructs an asset that acts as Settings for your Operating System."); }
	virtual UClass* GetSupportedClass() const override { return UYetiOS_SystemSettings::StaticClass(); }
};

UCLASS()
class YETIOSEDITOR_API UYetiOS_BaseProgram_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_BaseProgram_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_AppInstaller_Factory : public UFactory
{
	GENERATED_BODY()

	public:

	UYetiOS_AppInstaller_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_BaseFile_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_BaseFile_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_BaseDirectory_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_BaseDirectory_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_OperatingSystem_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_OperatingSystem_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_PortableDevice_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_PortableDevice_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_StationaryDevice_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_StationaryDevice_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_DeviceManager_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_DeviceManager_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_ProgramsRepository_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_ProgramsRepository_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};

UCLASS()
class YETIOSEDITOR_API UYetiOS_SystemSettings_Factory : public UFactory
{
	GENERATED_BODY()

public:

	UYetiOS_SystemSettings_Factory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

};
