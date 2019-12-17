// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnOperatingSystemInstallationStart, const float& /* Installation Time */ );

// Helper defines to make defining the delegate members easy
#define DEFINE_YETIOS_DELEGATE(DelegateType) \
	private: F##DelegateType DelegateType; \
	public: F##DelegateType& Get##DelegateType() { return DelegateType; } 

#define DEFINE_YETIOS_DELEGATE_TYPED(DelegateVariable, DelegateType) \
	private: DelegateType DelegateVariable; \
	public: DelegateType& Get##DelegateVariable() { return DelegateVariable; }

class YETIOS_API FYetiOsDelegates
{
public:

	static FYetiOsDelegates& Get()
	{
		static FYetiOsDelegates YetiDelegate;
		return YetiDelegate;
	}
};
