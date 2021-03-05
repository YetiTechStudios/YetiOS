// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Programs/YetiOS_Store.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Misc/YetiOS_ProgramsRepository.h"
#include "Core/YetiOS_Core.h"
#include <regex>

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsStore, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsStore, Log, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsStore, Error, TEXT("%s"), *FString(Param1))

UYetiOS_Store::UYetiOS_Store()
{
	bSupportStore = false;
}

void UYetiOS_Store::RegisterUser(const FText& InUserEmail, const FText& InUserPassword, float Time, const FOnStoreRegister& Callback, float InitialCash /*= 10000.f*/)
{
	DECLARE_DELEGATE(FOnDone);
	FOnDone OnDone;

	OnDone.BindLambda([&, InUserEmail, InUserPassword, InitialCash, Callback]
	{
		if (CurrentUser.IsValid())
		{
			Callback.Execute(EYetiOsStoreRegisterResult::Other);
		}
		else if (Internal_ValidateEmail(InUserEmail.ToString()) == false)
		{
			Callback.Execute(EYetiOsStoreRegisterResult::EmailNotValid);
		}
		else if (InUserPassword.IsEmptyOrWhitespace())
		{
			Callback.Execute(EYetiOsStoreRegisterResult::PasswordNotValid);
		}
		else
		{
			FYetiOsStoreUser TempUser = FYetiOsStoreUser(InUserEmail, InUserPassword, InitialCash);
			if (RegisteredUsers.Contains(TempUser))
			{
				Callback.Execute(EYetiOsStoreRegisterResult::UserAlreadyExists);
			}
			else
			{
				RegisteredUsers.Add(TempUser);
				Callback.Execute(EYetiOsStoreRegisterResult::Success);
			}
		}
	});

	if (Time > KINDA_SMALL_NUMBER)
	{
		FTimerHandle TimerHandle_DummyHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DummyHandle, OnDone, Time, false);
	}
	else
	{
		OnDone.Execute();
	}
}

void UYetiOS_Store::SignIn(const FText& InUserEmail, const FText& InUserPassword, float Time, const FOnStoreLogin& Callback)
{
	DECLARE_DELEGATE(FOnDone);
	FOnDone OnDone;

	OnDone.BindLambda([&, InUserEmail, InUserPassword, Callback]
	{
		if (CurrentUser.IsValid())
		{
			Callback.Execute(EYetiOsStoreSignInResult::Success);
		}
		else
		{
			if (Internal_ValidateEmail(InUserEmail.ToString()) == false)
			{
				Callback.Execute(EYetiOsStoreSignInResult::EmailNotValid);
				return;
			}

			const FYetiOsStoreUser TempUser = FYetiOsStoreUser(InUserEmail, InUserPassword);
			for (int32 i = 0; i < RegisteredUsers.Num(); ++i)
			{
				const int32 Val = RegisteredUsers[i].IsSameUser(TempUser);
				if (Val == 1)
				{
					CurrentUser = RegisteredUsers[i];
					CurrentUserIndex = i;
					Callback.Execute(EYetiOsStoreSignInResult::Success);
					return;
				}
				else if (Val == 0)
				{
					Callback.Execute(EYetiOsStoreSignInResult::IncorrectPassword);
					return;
				}
			}

			Callback.Execute(EYetiOsStoreSignInResult::NotRegistered);
		}
	});

	if (Time > KINDA_SMALL_NUMBER)
	{
		FTimerHandle TimerHandle_DummyHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DummyHandle, OnDone, Time, false);
	}
	else
	{
		OnDone.Execute();
	}
}

void UYetiOS_Store::SignOut()
{
	CurrentUserIndex = INDEX_NONE;
	CurrentUser.SignOut();
}

void UYetiOS_Store::BuyStoreItem(const FYetiOsStoreItem& InStoreItem, float Time, const FOnStoreItemBought& Callback)
{
	DECLARE_DELEGATE(FOnDone);
	FOnDone OnDone;

	OnDone.BindLambda([&, InStoreItem, Callback]
	{
		if (CurrentUser.IsValid() && CurrentUser.ReduceCash(InStoreItem.StoreDetail.Price))
		{
			CurrentUser.AddOwnedProgram(InStoreItem.Identifier);
			RegisteredUsers[CurrentUserIndex] = CurrentUser;
			Callback.Execute(true);
		}
		else
		{
			Callback.Execute(false);
		}
	});
	
	if (Time > KINDA_SMALL_NUMBER)
	{
		FTimerHandle TimerHandle_DummyHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DummyHandle, OnDone, Time, false);
	}
	else
	{
		OnDone.Execute();
	}
}

bool UYetiOS_Store::GetCurrentLoggedInStoreUser(FYetiOsStoreUser& OutUser) const
{
	if (CurrentUser.IsValid())
	{
		OutUser = CurrentUser;
		return true;
	}

	OutUser = FYetiOsStoreUser();
	return false;
}

bool UYetiOS_Store::UserOwnsItem(const FName& InProgramIdentifier) const
{
	if (CurrentUser.IsValid())
	{
		return CurrentUser.OwnsProgram(InProgramIdentifier);
	}

	return false;
}

TArray<FYetiOsStoreItem> UYetiOS_Store::GetStoreItems(const bool bForceRefresh /*= true*/, const bool bIgnoreInstalledWithOS /*= false*/) const
{
	if (bForceRefresh)
	{
		StoreItems.Empty();
		if (ProgramsRepository)
		{
			const TArray<UYetiOS_BaseProgram*>& Programs = ProgramsRepository->GetDefaultProgramObjects(bIgnoreInstalledWithOS);
			const int32 ProgramsCount = Programs.Num();

			for (int32 i = 0; i < ProgramsCount; ++i)
			{
				FYetiOsStoreItem TempItem;
				TempItem.Identifier = Programs[i]->GetProgramIdentifierName();
				TempItem.bIsOwned = UserOwnsItem(TempItem.Identifier);
				TempItem.bIsInstalled = OwningOS->IsProgramInstalled(TempItem.Identifier);
				TempItem.bRequiresMinVersion = Programs[i]->RequireMinimumOsVersion();
				TempItem.Icon = Programs[i]->GetProgramIcon();
				TempItem.MinVersion = Programs[i]->GetMinimumOsVersionRequired();
				TempItem.Name = Programs[i]->GetProgramName();
				TempItem.StoreDetail = Programs[i]->GetStoreDetail();
				TempItem.Size = Programs[i]->GetProgramSpace();
				TempItem.Version = Programs[i]->GetProgramVersion();
				StoreItems.Emplace(TempItem);
			}
		}
		else
		{
			printlog_error("Programs Repository was not found. You will need to assign one (just assign the same one you set for OS).");
		}
	}
	else
	{
		for (FYetiOsStoreItem& It : StoreItems)
		{
			It.bIsOwned = UserOwnsItem(It.Identifier);
			It.bIsInstalled = OwningOS->IsProgramInstalled(It.Identifier);
		}
	}
	
	return StoreItems;
}

bool UYetiOS_Store::Internal_ValidateEmail(const FString& InEmailString) const
{	
	const std::regex EmailPattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");	
	return std::regex_match(TCHAR_TO_UTF8(*InEmailString), EmailPattern);
}

#undef printlog
#undef printlog_error
