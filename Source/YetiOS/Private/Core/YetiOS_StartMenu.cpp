// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_StartMenu.h"
#include "Core/YetiOS_Taskbar.h"
#include "Core/YetiOS_Core.h"
#include "Widgets/YetiOS_StartMenuWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsStartMenu, All, All)

#define printlog(Param1)					UE_LOG(LogYetiOsStartMenu, Log, TEXT("%s"), *FString(Param1))
#define printlog_vv(Param1)					UE_LOG(LogYetiOsStartMenu, VeryVerbose, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)				UE_LOG(LogYetiOsStartMenu, Error, TEXT("%s"), *FString(Param1))

UYetiOS_StartMenu* UYetiOS_StartMenu::CreateStartMenu(class UYetiOS_Taskbar* InTaskbar)
{
	if (InTaskbar == nullptr)
	{
		printlog_error("Create Start Menu failed. Reason: Cannot create without taskbar.");
		return nullptr;
	}

	UYetiOS_Core* InCore = InTaskbar->GetOwningOS();
	if (InTaskbar->GeStartMenuClass() == NULL)
	{
		printlog_error(FString::Printf(TEXT("Create Start Menu failed. Reason: No start menu class provided for %s."), *InCore->GetOsName().ToString()));
		return nullptr;
	}

	const FString StartMenuName = FString::Printf(TEXT("StartMenu_%s"), *InCore->GetOsName().ToString());
	UYetiOS_StartMenu* ProxyObject = NewObject<UYetiOS_StartMenu>(InTaskbar, InTaskbar->GeStartMenuClass(), *StartMenuName);

	if (ProxyObject->StartMenuWidgetClass == NULL)
	{
		ProxyObject->ConditionalBeginDestroy();
		ProxyObject = nullptr;
		printlog_error("Create Start Menu failed. Reason: Start menu widget class is missing.");
		return nullptr;
	}

	ProxyObject->OwningOS = InCore;
	ProxyObject->StartMenuWidget = UYetiOS_StartMenuWidget::Internal_CreateStartMenuWidget(ProxyObject);
	printlog(FString::Printf(TEXT("Start menu created: %s"), *StartMenuName));
	return ProxyObject;
}

class UYetiOS_Core* UYetiOS_StartMenu::GetOwningOS() const
{
	if (OwningOS.IsValid())
	{
		return OwningOS.Get();
	}

	return nullptr;
}

#undef printlog
#undef printlog_vv
#undef printlog_error
