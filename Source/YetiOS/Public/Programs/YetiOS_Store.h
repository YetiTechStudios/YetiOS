// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/YetiOS_BaseProgram.h"
#include "YetiOS_Store.generated.h"

UENUM(BlueprintType)
enum class EYetiOsStoreRegisterResult : uint8
{
	Success,
	EmailNotValid,
	PasswordNotValid,
	UserAlreadyExists,
	Other
};

UENUM(BlueprintType)
enum class EYetiOsStoreSignInResult : uint8
{
	Success,
	EmailNotValid,
	IncorrectPassword,
	NotRegistered
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreItemBought, const bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreRegister, EYetiOsStoreRegisterResult, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoreLogin, EYetiOsStoreSignInResult, Result);

UCLASS(Abstract, DisplayName = "Store")
class YETIOS_API UYetiOS_Store : public UYetiOS_BaseProgram
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti Os Store")
	class UYetiOS_ProgramsRepository* ProgramsRepository;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<FYetiOsStoreUser> RegisteredUsers;
		
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOsStoreUser CurrentUser;

	mutable TArray<FYetiOsStoreItem> StoreItems;

	int32 CurrentUserIndex;

public:

	UYetiOS_Store();

	/**
	* public UYetiOS_Store::RegisterUser
	* Registers a new user for Store. Registered user can Sign In to the Store.
	* @See SignIn function.
	* @param InUserEmail [const FText&] Email (use a fake one) to register.
	* @param InUserPassword [const FText&] Password to use.
	* @param Time [float] Time taken to call the Callback delegate. If 0, call the delegate instantly.
	* @param Callback [const FOnStoreRegister&] Delegate that will be called when Time expires. If time is 0, call instantly.
	* @param InitialCash [float] Initial cash this user will have to spend on Store.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Os Store", meta = (AdvancedDisplay = "5"))
	void RegisterUser(const FText& InUserEmail, const FText& InUserPassword, float Time, const FOnStoreRegister& Callback, float InitialCash = 10000.f);

	/**
	* public UYetiOS_Store::SignIn
	* Login to the store with credentials used when registering.
	* @See RegisterUser function.
	* @param InUserEmail [const FText&] User email. Same one you used when registering.
	* @param InUserPassword [const FText&] Password.
	* @param Time [float] Time taken to call the Callback delegate. If 0, call the delegate instantly.
	* @param Callback [const FOnStoreLogin&] Delegate that will be called when Time expires. If time is 0, call instantly.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Os Store")
	void SignIn(const FText& InUserEmail, const FText& InUserPassword, float Time, const FOnStoreLogin& Callback);

	/**
	* public UYetiOS_Store::SignOut
	* Logs out the current user.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Os Store")	
	void SignOut();

	/**
	* public UYetiOS_Store::BuyStoreItem
	* Buys an item from the store.
	* @param InStoreItem [const FYetiOsStoreItem&] Item to buy.
	* @param Time [float] Time taken to call the Callback delegate. If 0, call the delegate instantly.
	* @param Callback [const FOnStoreItemBought&] Delegate that will be called when Time expires. If time is 0, call instantly.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Os Store", meta = (Keywords = "purchase item program"))
	void BuyStoreItem(const FYetiOsStoreItem& InStoreItem, float Time, const FOnStoreItemBought& Callback);

	/**
	* public UYetiOS_Store::GetCurrentLoggedInStoreUser const
	* Gets the currently logged in user. Returns true if a user is logged in. False otherwise.
	* @param OutUser [FYetiOsStoreUser&] Logged in user (if return value is true).
	* @return [bool] True if a user is currently logged in.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Os Store")
	bool GetCurrentLoggedInStoreUser(FYetiOsStoreUser& OutUser) const;

	/**
	* public UYetiOS_Store::UserOwnsItem const
	* Checks if the user already owns the program.
	* @param InProgramIdentifier [const FName&] Program identifier to check.
	* @return [bool] True if the user already owns it.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Os Store")
	bool UserOwnsItem(const FName& InProgramIdentifier) const;

	/**
	* public UYetiOS_Store::GetStoreItems const
	* Get all items from Store.
	* @param bIgnoreInstalledWithOS [const bool] True to ignore items already installed with OS.
	* @return [TArray<FYetiOsStoreItem>] List of items from Store.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Os Store")
	TArray<FYetiOsStoreItem> GetStoreItems(const bool bForceRefresh = true, const bool bIgnoreInstalledWithOS = false) const;

private:

	bool Internal_ValidateEmail(const FString& InEmailString) const;
	
};
