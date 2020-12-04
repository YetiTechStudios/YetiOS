// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "UObject/Package.h"
#include "Templates/SubclassOf.h"
#include <string>
#include "YetiOS_Types.generated.h"

#define ENUM_TO_STRING(EnumClassName, ValueOfEnum)				GetEnumAsString<EnumClassName>(FString(TEXT(#EnumClassName)), ValueOfEnum)
#define STRING_TO_ENUM(EnumClassName, ValueOfEnum, OutEnum)		GetEnumValueFromString<EnumClassName>(FString(TEXT(#EnumClassName)), ValueOfEnum, OutEnum)

class UYetiOS_DirectoryRoot;
class UYetiOS_BaseProgram;

UENUM(BlueprintType)
enum class EYetiOS_DialogType : uint8
{
	None			UMETA(Hidden),
	Ok,
	OkCancel,
	YesNo,
	YesNoCancel,
	RetryCancel,
	AbortRetryIgnore
};

UENUM(BlueprintType)
enum class EYetiOS_DialogResult : uint8
{
	None			UMETA(Hidden),
	Abort,
	Cancel,
	Ignore,
	No,
	Ok,
	Retry,
	Yes
};

UENUM(BlueprintType)
enum class EYetiOsNotificationType : uint8
{
	TYPE_Info							UMETA(DisplayName = "Info"),
	TYPE_Warning						UMETA(DisplayName = "Warning"),
	TYPE_Error							UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class EYetiOsNotificationCategory : uint8
{
	CATEGORY_Device						UMETA(DisplayName = "Device"),
	CATEGORY_OS							UMETA(DisplayName = "OS"),
	CATEGORY_App						UMETA(DisplayName = "App")
};

UENUM(BlueprintType)
enum class EYetiOsTerminalMessageLevel : uint8
{
	LEVEL_Normal						UMETA(DisplayName = "Normal"),
	LEVEL_Warning						UMETA(DisplayName = "Warning"),
	LEVEL_Error							UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class EYetiOsThemeMode : uint8
{
	Light								UMETA(DisplayName = "Light"),
	Dark								UMETA(DisplayName = "Dark"),
	MAX									UMETA(Hidden) // Add new theme modes ABOVE this.
};

UENUM(BlueprintType)
enum class EYetiOsColorTypes : uint8
{
	PrimaryColor,
	SecondayColor,
	AccentOne,
	AccentTwo,
	AccentThree
};

UENUM(BlueprintType)
enum class EDirectoryType : uint8
{
	Root,
	Desktop,
	Documents,
	Downloads,
	Home,
	Music,
	Pictures,
	Videos,
	Other
};

UENUM(BlueprintType)
enum class EYetiOsDeviceState : uint8
{	
	/* Device is starting. */
	STATE_Starting						UMETA(DisplayName = "Starting"),
	
	/* Device is actively running. */
	STATE_Running						UMETA(DisplayName = "Running"),
	
	/* Device is shutting down. */
	STATE_PowerOff						UMETA(DisplayName = "Power Off"),
	
	/* Device is restarting. */
	STATE_Restart						UMETA(DisplayName = "Restart"),

	/** Device is in bsod state */
	STATE_BSOD							UMETA(DisplayName = "Blue Screen"),
	
	/* Invalid state. */
	STATE_None							UMETA(Hidden)
};

/* @See: UYetiOS_BaseDevice::StartDevice */
UENUM(BlueprintType)
enum class EYetiOsDeviceStartResult : uint8
{	
	/* Hardware failure could mean that required hardware was not found or 0 durability. */
	DEVICESTART_HardwareFail			UMETA(DisplayName = "Hardware Failure"),

	/* An operating system is required to boot so if no OS then this is the result. */
	DEVICESTART_NoOs					UMETA(DisplayName = "No Operating System"),

	/* Device cannot start because of no battery power. Only applicable if owning device is portable. */
	DEVICESTART_NoBattery				UMETA(DisplayName = "No battery power"),

	/* Device started successfully with no issues. Yay! */
	DEVICESTART_Success					UMETA(DisplayName = "Success")
};

UENUM(BlueprintType)
enum class EYetiOsHardwareInstallResult : uint8
{
	HWINSTALL_DeviceConfigInvalid		UMETA(DisplayName = "Device Properties Missing"),
	HWINSTALL_IncompatibleDevice		UMETA(DisplayName = "Device Unsupported"),
	HWINSTALL_DeviceRunning				UMETA(DisplayName = "Device Running"),
	HWINSTALL_NoDevice					UMETA(DisplayName = "No Device"),
	HWINSTALL_AlreadyInstalled			UMETA(DisplayName = "Already Installed"),
	HWINSTALL_Success					UMETA(DisplayName = "Success")
};

UENUM(BlueprintType)
enum class EYetiOsProgramVisibilityState : uint8
{
	STATE_Normal						UMETA(DisplayName = "Normal"),
	STATE_Minimize						UMETA(DisplayName = "Minimize"),
	STATE_Maximize						UMETA(DisplayName = "Maximize")
};

UENUM(BlueprintType)
enum class EYetiOsOperatingSystemReleaseState : uint8
{
	STATE_Beta							UMETA(DisplayName = "Beta"),
	STATE_TechnicalPreview				UMETA(DisplayName = "Technical Preview"),
	STATE_FullRelease					UMETA(DisplayName = "Final Release")
};

/* https://en.wikipedia.org/wiki/CPU_socket#List_of_CPU_sockets_and_slots */
UENUM(BlueprintType)
enum class EYetiOsSocketType : uint8
{
	SOCKET_1155							UMETA(DisplayName = "LGA 1155"),
	SOCKET_2011							UMETA(DisplayName = "LGA 2011"),
	SOCKET_1150							UMETA(DisplayName = "LGA 1150"),
	SOCKET_1151							UMETA(DisplayName = "LGA 1151"),
	SOCKET_2066							UMETA(DisplayName = "LGA 2066")
};

/* Device memory. */
UENUM(BlueprintType)
enum class EYetiOsMemorySize : uint8
{
	SIZE_64, // 64 MB Ram
	SIZE_128, // 128 MB Ram
	SIZE_256, // 256 MB Ram
	SIZE_512, // 512 MB Ram
	SIZE_1024, // 1 GB Ram
	SIZE_2048, // 2 GB Ram
	SIZE_4096, // 4 GB Ram
	SIZE_8192 // 8 GB Ram
};

UENUM(BlueprintType)
enum class EYetiOsPortableBatteryEfficiencyLoss : uint8
{
	LOSS_TenPercent				UMETA(DisplayName = "10% Loss"),
	LOSS_TwentyPercent			UMETA(DisplayName = "20% Loss"),
	LOSS_ThirtyPercent			UMETA(DisplayName = "30% Loss"),
	LOSS_FourtyPercent			UMETA(DisplayName = "40% Loss"),
	LOSS_FiftyPercent			UMETA(DisplayName = "50% Loss"),
	LOSS_SixtyPercent			UMETA(DisplayName = "60% Loss"),
	LOSS_SeventyPercent			UMETA(DisplayName = "70% Loss"),
	LOSS_EightyPercent			UMETA(DisplayName = "80% Loss"),
	LOSS_NinetyPercent			UMETA(DisplayName = "90% Loss"),
	LOSS_NoLoss					UMETA(DisplayName = "No Loss"),
};

static FORCEINLINE const float GetMemorySizeImplementation(const EYetiOsMemorySize InMemorySize)
{
	float ReturnSize = 0.f;
	switch (InMemorySize)
	{
		case EYetiOsMemorySize::SIZE_64:
			ReturnSize = 64.f;
			break;
		case EYetiOsMemorySize::SIZE_128:
			ReturnSize = 128.f;
			break;
		case EYetiOsMemorySize::SIZE_256:
			ReturnSize = 256.f;
			break;
		case EYetiOsMemorySize::SIZE_512:
			ReturnSize = 512.f;
			break;
		case EYetiOsMemorySize::SIZE_1024:
			ReturnSize = 1024.f;
			break;
		case EYetiOsMemorySize::SIZE_2048:
			ReturnSize = 2048.f;
			break;
		case EYetiOsMemorySize::SIZE_4096:
			ReturnSize = 4096.f;
			break;
		case EYetiOsMemorySize::SIZE_8192:
			ReturnSize = 8192.f;
			break;
		default:
			ensureAlwaysMsgf(false, TEXT("Given memory size is invalid."));
			break;
	}

	return ReturnSize;
}

static FORCEINLINE const float GetMaxMemorySizeImplementation()
{
	return GetMemorySizeImplementation(EYetiOsMemorySize::SIZE_8192);
}

static FORCEINLINE const FString GetSocketNameImplementation(EYetiOsSocketType InSocketType)
{
	FString MySocketName = "Unknown";
	switch (InSocketType)
	{
		case EYetiOsSocketType::SOCKET_1155:
			MySocketName = "LGA Socket 1155";
			break;
		case EYetiOsSocketType::SOCKET_2011:
			MySocketName = "LGA Socket 2011";
			break;
		case EYetiOsSocketType::SOCKET_1150:
			MySocketName = "LGA Socket 1150";
			break;
		case EYetiOsSocketType::SOCKET_1151:
			MySocketName = "LGA Socket 1151";
			break;
		case EYetiOsSocketType::SOCKET_2066:
			MySocketName = "LGA Socket 2066";
			break;
		default:
			ensureAlwaysMsgf(false, TEXT("Incorrect socket enum."));
			break;
	}

	return MySocketName;
}

USTRUCT(BlueprintType)
struct FYetiOsColorCollection
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Color Collection")
	FName CollectionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Color Collection")
	TMap<EYetiOsColorTypes, FLinearColor> SystemColors;

	FYetiOsColorCollection()
	{
		CollectionName = NAME_None;
	}
};

USTRUCT(BlueprintType)
struct FYetiOsError
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Error Struct")
	FText ErrorCode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Error Struct")
	FText ErrorException;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Error Struct")
	FText ErrorDetailedException;

	FYetiOsError()
	{
		ErrorCode = FText::GetEmpty();
		ErrorException = FText::GetEmpty();
		ErrorDetailedException = FText::GetEmpty();
	}

	FYetiOsError(const FText InErrorCode)
	{
		ErrorCode = InErrorCode;
		ErrorException = FText::GetEmpty();
		ErrorDetailedException = FText::GetEmpty();
	}

	FYetiOsError(const FText InErrorCode, const FText InErrorException)
	{
		ErrorCode = InErrorCode;
		ErrorException = InErrorException;
		ErrorDetailedException = FText::GetEmpty();
	}

	FYetiOsError(const FText InErrorCode, const FText InErrorException, const FText InErrorExceptionDetailed)
	{
		ErrorCode = InErrorCode;
		ErrorException = InErrorException;
		ErrorDetailedException = InErrorExceptionDetailed;
	}
};

USTRUCT(BlueprintType)
struct FYetiOsPortableBattery
{
	GENERATED_USTRUCT_BODY();
	
	/* Battery Name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery")
	FText BatteryName;

	/* Brand name of this battery. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery")
	FText BatteryBrand;
	
	/* Total battery power. In milliamp Hour units */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery", DisplayName = "Battery Capacity (mAh)", meta = (UIMin = "1000", ClampMin = "1000", UIMax = "5000"))
	float BatteryCapacity;

	/* Speed at which this battery should charge. Higher rate charges battery faster. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery", DisplayName = "Charge Rate (mA)", meta = (UIMin = "1000", ClampMin = "1000", UIMax = "5000"))
	float ChargeRate;

	/* Simulates energy loss while charging. Higher loss means more time to charge. This can also be considered as battery health. Higher loss means less healthy.
	 * For example: A battery with 3200 mAh and charge rate of 1000 mA will take 3.2 hours to fully charge without energy loss. With 40% energy loss it would take 4.5 hours to fully charge.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery")
	EYetiOsPortableBatteryEfficiencyLoss EfficiencyLoss;

	/* If battery level is <= to this, then device emits a warning signal. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Portable Battery", meta = (UIMin = "0.15", UIMax = "0.95", ClampMin = "0.1", ClampMax = "0.99"))
	float LowBatteryWarningLevel;

	FORCEINLINE const float GetEfficiencyLossvalue() const
	{
		switch (EfficiencyLoss)
		{
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_TenPercent:
				return 11.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_TwentyPercent:
				return 12.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_ThirtyPercent:
				return 13.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_FourtyPercent:
				return 14.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_FiftyPercent:
				return 15.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_SixtyPercent:
				return 16.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_SeventyPercent:
				return 17.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_EightyPercent:
				return 18.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_NinetyPercent:
				return 19.f;
				break;
			case EYetiOsPortableBatteryEfficiencyLoss::LOSS_NoLoss:
			default:
				break;
		}

		return 10.f;
	}

	FORCEINLINE const float GetBatteryHealth(const bool bNormalize) const
	{
		return bNormalize ? ((uint8)EfficiencyLoss + 1) / 10.f : (((uint8)EfficiencyLoss + 1) * 100.f) / 10.f;
	}

	FORCEINLINE const float GetTimeToFullyRechargeInHours() const { return ((BatteryCapacity / ChargeRate) * GetEfficiencyLossvalue()) / 10.f; }

	FYetiOsPortableBattery()
	{
		BatteryCapacity = 1200.f;
		ChargeRate = 2000.f;
		LowBatteryWarningLevel = 0.2;
		EfficiencyLoss = EYetiOsPortableBatteryEfficiencyLoss::LOSS_NoLoss;
	}
};

USTRUCT(BlueprintType)
struct FYetiOSTemperature
{
	GENERATED_USTRUCT_BODY();
	
	/* Enable temperature settings? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Temperature")
	uint8 bEnableTemperature : 1;
	
	/* Maximum temperature this hardware can withstand before destroying. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Temperature", meta = (EditCondition = "bEnableTemperature"))
	uint8 MaxTemperature;

	/* Notify the player when this temperature is reached. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Temperature", meta = (EditCondition = "bEnableTemperature"))
	uint8 TemperatureWarningLevel;
	
	FYetiOSTemperature()
	{
		bEnableTemperature = true;
		MaxTemperature = 100;
		TemperatureWarningLevel = 80;
	}
};

USTRUCT(BlueprintType)
struct FYetiOsUser
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Yeti OS User")
	FText UserName;

	/* Password (if any) for this user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Yeti OS User")
	FText Password;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Yeti OS User", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* UserIcon;

	FORCEINLINE const bool operator==(const FYetiOsUser& Other) const
	{
		return Other.UserName.EqualTo(UserName) && Other.Password.EqualTo(Password);
	}

	FYetiOsUser()
	{
		UserName = FText::GetEmpty();
		Password = FText::GetEmpty();
		UserIcon = nullptr;
	}

	FYetiOsUser(const FString InName)
	{
		UserName = FText::AsCultureInvariant(InName);
		Password = FText::GetEmpty();
		UserIcon = nullptr;
	}

	FYetiOsUser(const FString InName, const FString InPassword)
	{
		UserName = FText::AsCultureInvariant(InName);
		Password = FText::AsCultureInvariant(InPassword);
		UserIcon = nullptr;
	}

	FYetiOsUser(const FString InName, const FString InPassword, class UObject* InIcon)
	{
		UserName = FText::AsCultureInvariant(InName);
		Password = FText::AsCultureInvariant(InPassword);
		UserIcon = InIcon;
	}
};

USTRUCT(BlueprintType)
struct FYetiOsNotification
{
	GENERATED_USTRUCT_BODY()

	static const int32 DEFAULT_DURATION = 5;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	FText Title;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	FText Description;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	FText Code;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	FText TimeStamp;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	class UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	EYetiOsNotificationType Level;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	EYetiOsNotificationCategory Category;

	UPROPERTY(BlueprintReadWrite, Category = "Yeti OS Notification")
	float Duration;

public:

	FYetiOsNotification()
	{
		Title = FText::GetEmpty();
		Description = FText::GetEmpty();
		Code = FText::GetEmpty();
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = EYetiOsNotificationCategory::CATEGORY_OS;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(FText InTitle)
	{
		Title = InTitle;
		Description = FText::GetEmpty();
		Code = Description;
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = EYetiOsNotificationCategory::CATEGORY_OS;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(FText InTitle, FText InDesciption)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = EYetiOsNotificationCategory::CATEGORY_OS;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(FText InTitle, FText InDesciption, FText InCode)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = InCode;
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = EYetiOsNotificationCategory::CATEGORY_OS;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle)
	{
		Title = InTitle;
		Description = FText::GetEmpty();
		Code = Description;
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, FText InCode)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = InCode;
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, float InDuration)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = nullptr;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = InCategory;
		Duration = InDuration;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, UTexture2D* InIcon)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = InIcon;
		Level = EYetiOsNotificationType::TYPE_Info;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, EYetiOsNotificationType InLevel)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = nullptr;
		Level = InLevel;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, FText InCode, EYetiOsNotificationType InLevel)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = InCode;
		Icon = nullptr;
		Level = InLevel;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, UTexture2D* InIcon, EYetiOsNotificationType InLevel)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = InIcon;
		Level = InLevel;
		Category = InCategory;
		Duration = DEFAULT_DURATION;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, UTexture2D* InIcon, EYetiOsNotificationType InLevel, float InDuration)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = FText::GetEmpty();
		Icon = InIcon;
		Level = InLevel;
		Category = InCategory;
		Duration = InDuration;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}

	FYetiOsNotification(EYetiOsNotificationCategory InCategory, FText InTitle, FText InDesciption, FText InCode, UTexture2D* InIcon, EYetiOsNotificationType InLevel, float InDuration)
	{
		Title = InTitle;
		Description = InDesciption;
		Code = InCode;
		Icon = InIcon;
		Level = InLevel;
		Category = InCategory;
		Duration = InDuration;

		FDateTime Now = FDateTime::Now();
		TimeStamp = FText::AsCultureInvariant(Now.ToString());
	}
};

USTRUCT(BlueprintType)
struct FYetiOS_Version
{
	GENERATED_USTRUCT_BODY();

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	uint8 Major;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	uint8 Minor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	uint8 Patch;

public:

	FORCEINLINE const FString ToString() const
	{
		return ToString(false);
	}

	FORCEINLINE const FString ToString(const bool bIgnorePatch) const
	{
		if (bIgnorePatch)
		{
			return FString::Printf(TEXT("%i.%i"), Major, Minor);
		}

		return FString::Printf(TEXT("%i.%i.%i"), Major, Minor, Patch);
	}

	FORCEINLINE bool operator==(const FYetiOS_Version& Other) const
	{
		return Internal_IsSameVersion(FYetiOS_Version(Major, Minor, Patch), Other);
	}

	FORCEINLINE bool operator<(const FYetiOS_Version& Other) const
	{
		return Internal_IsVersionOneSmaller(FYetiOS_Version(Major, Minor, Patch), Other);
	}

	FORCEINLINE bool operator>(const FYetiOS_Version& Other) const
	{
		return Internal_IsVersionTwoSmaller(FYetiOS_Version(Major, Minor, Patch), Other);
	}

private:

	static FORCEINLINE bool Internal_IsSameVersion(const FYetiOS_Version& VersionOne, const FYetiOS_Version& VersionTwo)
	{
		return Internal_VersionCompare(VersionOne, VersionTwo) == 0;
	}

	static FORCEINLINE bool Internal_IsVersionOneSmaller(const FYetiOS_Version& VersionOne, const FYetiOS_Version& VersionTwo)
	{
		return Internal_VersionCompare(VersionOne, VersionTwo) < 0;
	}

	static FORCEINLINE bool Internal_IsVersionTwoSmaller(const FYetiOS_Version& VersionOne, const FYetiOS_Version& VersionTwo)
	{
		return Internal_VersionCompare(VersionOne, VersionTwo) > 0;
	}

private:

	static FORCEINLINE_DEBUGGABLE const int32 Internal_VersionCompare(const FYetiOS_Version& VersionOne, const FYetiOS_Version& VersionTwo)
	{
		std::string v1 = std::string(TCHAR_TO_UTF8(*VersionOne.ToString()));
		std::string v2 = std::string(TCHAR_TO_UTF8(*VersionTwo.ToString()));
		int32 vnum1 = 0, vnum2 = 0;

		for (int32 i = 0, j = 0; (i < v1.length() || j < v2.length());)
		{
			while (i < v1.length() && v1[i] != '.')
			{
				vnum1 = vnum1 * 10 + (v1[i] - '0');
				i++;
			}

			while (j < v2.length() && v2[j] != '.')
			{
				vnum2 = vnum2 * 10 + (v2[j] - '0');
				j++;
			}

			if (vnum1 > vnum2)
			{
				return 1;
			}

			if (vnum2 > vnum1)
			{
				return -1;
			}

			vnum1 = vnum2 = 0;
			i++;
			j++;
		}

		return 0;
	}

public:

	FYetiOS_Version()
	{
		Major = 1;
		Minor = Patch = 0;
	}

	FYetiOS_Version(const uint8 NewMajor)
	{
		Major = NewMajor;
		Minor = Patch = 0;
	}

	FYetiOS_Version(const uint8 NewMajor, const uint8 NewMinor)
	{
		Major = NewMajor;
		Minor = NewMinor;
		Patch = 0;
	}

	FYetiOS_Version(const uint8 NewMajor, const uint8 NewMinor, const uint8 NewPatch)
	{
		Major = NewMajor;
		Minor = NewMinor;
		Patch = NewPatch;
	}
};

USTRUCT()
struct FYetiOsDeviceSaveLoad
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	uint8 bSaveLoad_OsInstalled : 1;

	UPROPERTY()
	float SaveLoad_BatteryLevel;

	UPROPERTY()
	int64 SaveLoad_RemainingSpace;
};

USTRUCT()
struct FYetiOsOperatingSystemSaveLoad
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	struct FYetiOS_Version SaveLoad_OSVersion;

	UPROPERTY()
	TArray<FYetiOsUser> SaveLoad_OsUsers;
};

USTRUCT()
struct FYetiOsDirectorySaveLoad
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	FString SaveLoad_DirPath;
	
	UPROPERTY()
	FText SaveLoad_DirectoryName;

	UPROPERTY()
	UTexture2D* SaveLoad_DirectoryIcon;

	UPROPERTY()
	uint8 bSaveLoad_CanCreateNewFolder : 1;

	UPROPERTY()
	uint8 bSaveLoad_CanCreateNewFile : 1;

	UPROPERTY()
	uint8 bSaveLoad_IsHidden : 1;

	UPROPERTY()
	TSet<class UYetiOS_FileBase*> SaveLoad_Files;

	UPROPERTY()
	TArray<TSubclassOf<class UYetiOS_DirectoryBase>> SaveLoad_ChildDirectoryClasses;

};

USTRUCT(BlueprintType)
struct FYetiOsProgramSaveLoad
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	TSubclassOf<class UYetiOS_BaseProgram> SaveLoad_ProgramClass;
	
	UPROPERTY()
	FText SaveLoad_ProgramName;

	UPROPERTY()
	FName SaveLoad_ProgramIdentifier;

	UPROPERTY()
	class UObject* SaveLoad_ProgramIcon;

	UPROPERTY()
	float SaveLoad_ProgramSpace;

	UPROPERTY()
	bool bSaveLoad_SingleInstanceOnly;
};

USTRUCT()
struct FYetiOS_DeviceCPU
{
	GENERATED_USTRUCT_BODY();

	/** How many should be created. Better to keep it <= 4 */
	UPROPERTY(EditAnywhere, Category = "Device CPU")
	uint8 Total;

	/** Class of object to construct. */
	UPROPERTY(EditAnywhere, Category = "Device CPU")
	TSubclassOf<class UYetiOS_CPU> Class;

	FYetiOS_DeviceCPU()
	{
		Total = 1;
		Class = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FYetiOS_DeviceMemory
{
	GENERATED_USTRUCT_BODY();

	/** How many should be created. Better to keep it <= 4 */
	UPROPERTY(EditAnywhere, Category = "Device Memory")
	uint8 Total;

	/** Class of object to construct. */
	UPROPERTY(EditAnywhere, Category = "Device Memory")
	TSubclassOf<class UYetiOS_Memory> Class;

	FYetiOS_DeviceMemory()
	{
		Total = 1;
		Class = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FYetiOS_DeviceGPU
{
	GENERATED_USTRUCT_BODY();

	/** How many should be created. Better to keep it <= 4 */
	UPROPERTY(EditAnywhere, Category = "Device GPU")
	uint8 Total;

	/** Class of object to construct. */
	UPROPERTY(EditAnywhere, Category = "Device GPU")
	TSubclassOf<class UYetiOS_GraphicsCard> Class;

	FYetiOS_DeviceGPU()
	{
		Total = 1;
		Class = nullptr;
	}
};

USTRUCT()
struct FYetiOS_DeviceClasses
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	TSubclassOf<class UYetiOS_Motherboard> MotherboardClass;

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	FYetiOS_DeviceCPU CPU;

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	FYetiOS_DeviceMemory Memory;

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	FYetiOS_DeviceGPU GPU;

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	TSubclassOf<class UYetiOS_HardDisk> HardDiskClass;

	UPROPERTY(EditAnywhere, Category = "Device Classes")
	TSubclassOf<class UYetiOS_PowerSupply> PowerSupplyClass;
};

USTRUCT()
struct FYetiOS_RepoProgram
{
	GENERATED_USTRUCT_BODY();
	
public:
	
	/** WHether Program should be installed as part of the OS when Operating System is installed. */
	UPROPERTY(EditAnywhere, Category = "Repo Program")
	uint8 bInstallWithOS : 1;
	
	/** Program */
	UPROPERTY(EditAnywhere, Category = "Repo Program")
	TSubclassOf<class UYetiOS_BaseProgram> ProgramClass;

	friend uint32 GetTypeHash(const FYetiOS_RepoProgram& Other)
	{
		return GetTypeHash(Other.bInstallWithOS);
	}
	
	FYetiOS_RepoProgram()
	{
		bInstallWithOS = true;
	}
};


template<typename TEnum>
static FORCEINLINE const FString GetEnumAsString(const FString& Name, TEnum Value)
{
	if (const UEnum* EnumClass = FindObject<UEnum>(ANY_PACKAGE, *Name, true))
	{
		return EnumClass->GetNameByValue((int64)Value).ToString().Replace(*FString::Printf(TEXT("%s::"), *Name), TEXT(""));
	}

	ensureAlwaysMsgf(false, TEXT("Invalid enum %s"), *Name);
	return FString("Invalid");
}

template <typename EnumType>
static FORCEINLINE bool GetEnumValueFromString(const FString& EnumName, const FString& EnumString, EnumType& OutEnum)
{
	const FString TargetString = EnumString.Replace(TEXT(" "), TEXT(""));
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!Enum)
	{
		ensureAlwaysMsgf(false, TEXT("Failed to find %s using string %s"), *EnumName, *EnumString);
		OutEnum = EnumType(0);
		return false;
	}

	OutEnum = (EnumType)Enum->GetIndexByName(FName(*TargetString));
	return true;
}

static FORCEINLINE const FYetiOsError GetErrorStruct(const FText& InErrorCode, const FText& InErrorException, const FText& InErrorExceptionDetail = FText::GetEmpty())
{
	return FYetiOsError(InErrorCode, InErrorException, InErrorExceptionDetail);
}

#define LOCTEXT_NAMESPACE "YetiOS"

namespace YetiOS_CommonErrors
{
	static const FText MbNotCompatibleErrorCode = LOCTEXT("MbCompatibleErrorCode", "HW_MB_NOT_COMPATIBLE");
	static const FText MbNotCompatibleErrorException = LOCTEXT("MbCompatibleErrorException", "Motherboard not compatible with target device.");

	static const FText MbErrorCode = LOCTEXT("NoMbErrorCode", "HW_MB_FAIL");
	static const FText MbErrorException = LOCTEXT("NoMbErrorException", "You need a motherboard to boot this device.");

	static const FText CpuErrorCode = LOCTEXT("NoCpuErrorCode", "HW_CPU_FAIL");
	static const FText CpuErrorException = LOCTEXT("NoCpuErrorException", "You need a cpu to boot this device.");

	static const FText RamErrorCode = LOCTEXT("NoRamErrorCode", "HW_RAM_FAIL");
	static const FText RamErrorException = LOCTEXT("NoRamErrorException", "Memory Error.");

	static const FText GpuErrorCode = LOCTEXT("NoGpuErrorCode", "HW_GPU_FAIL");
	static const FText GpuErrorException = LOCTEXT("NoGpuErrorException", "Failed to access Graphics Card (GPU).");

	static const FText HddErrorCode = LOCTEXT("NoHddErrorCode", "HW_HDD_FAIL");
	static const FText HddErrorException = LOCTEXT("NoHddErrorException", "Failed to read/write from Hard Disk.");

	static const FText PsuErrorCode = LOCTEXT("NoPsuErrorCode", "HW_PSU_FAIL");
	static const FText PsuErrorException = LOCTEXT("NoPsuErrorException", "Failed to access Power Supply.");

	static const FText OsInstallCode = LOCTEXT("StartInstallationCode", "OS_INSTALL_FAIL");
	static const FText OsInstallException = LOCTEXT("StartInstallation", "Insufficient space.");
	static const FText OsInstallDetailedException = LOCTEXT("StartInstallationDescription", "Not enough space to install Operating System on this device.");

	static const FText OsDesktopCode = LOCTEXT("DesktopErrorCode", "NO_DESKTOP");
	static const FText OsDesktopException = LOCTEXT("DesktopError", "Desktop not found.");
	static const FText OsDesktopDetailedException = LOCTEXT("DesktopErrorDescription", "Failed to find Desktop Directory.");

	static const FText OsRootCode = LOCTEXT("RootDirErrorCode", "NO_ROOT_DIR");
	static const FText OsRootException = LOCTEXT("RootDirError", "Root directory not found.");
	static const FText OsRootDetailedException = LOCTEXT("RootDirDescription", "Failed to find Root Directory.");
}

#undef LOCTEXT_NAMESPACE
