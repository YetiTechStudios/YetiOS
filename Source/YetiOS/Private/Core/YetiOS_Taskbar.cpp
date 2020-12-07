// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_Taskbar.h"
#include "Core/YetiOS_Core.h"
#include "Widgets/YetiOS_TaskbarWidget.h"
#include "Core/YetiOS_StartMenu.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsTaskbar, All, All)

#define printlog(Param1)					UE_LOG(LogYetiOsTaskbar, Log, TEXT("%s"), *FString(Param1))
#define printlog_vv(Param1)					UE_LOG(LogYetiOsTaskbar, VeryVerbose, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)				UE_LOG(LogYetiOsTaskbar, Error, TEXT("%s"), *FString(Param1))

UYetiOS_Taskbar* UYetiOS_Taskbar::CreateTaskbar(class UYetiOS_Core* InCore)
{
	if (InCore == nullptr)
	{
		printlog_error("Create Taskbar failed. Reason: NULL OS.");
		return nullptr;
	}

	if (InCore->GetTaskbarClass() == NULL)
	{
		printlog_error(FString::Printf(TEXT("Create Taskbar failed. Reason: No taskbar class provided for %s."), *InCore->GetOsName().ToString()));
		return nullptr;
	}

	const FString TaskbarName = FString::Printf(TEXT("Taskbar_%s"), *InCore->GetOsName().ToString());
	UYetiOS_Taskbar* ProxyObject = NewObject<UYetiOS_Taskbar>(InCore, InCore->GetTaskbarClass(), *TaskbarName);

	if (ProxyObject->TaskbarWidgetClass == NULL)
	{
		ProxyObject->ConditionalBeginDestroy();
		ProxyObject = nullptr;
		printlog_error("Create Taskbar failed. Reason: Taskbar widget class is missing.");
		return nullptr;
	}

	ProxyObject->OwningOS = InCore;
	ProxyObject->TaskbarWidget = UYetiOS_TaskbarWidget::Internal_CreateTaskbarWidget(ProxyObject);	
	printlog(FString::Printf(TEXT("Taskbar created: %s"), *TaskbarName));
	ProxyObject->StartMenu = UYetiOS_StartMenu::CreateStartMenu(ProxyObject);
	return ProxyObject;
}

void UYetiOS_Taskbar::PeekDesktop(const bool bEnable)
{
	if (bEnablePeekPreview)
	{
		GetOwningOS()->OnPeekPreview.Broadcast(bEnable);
	}
}

class UYetiOS_Core* UYetiOS_Taskbar::GetOwningOS() const
{
	return OwningOS.Get();
}

bool UYetiOS_Taskbar::GetStartMenu(class UYetiOS_StartMenu*& OutStartMenu) const
{
	OutStartMenu = StartMenu;
	return OutStartMenu != nullptr;
}

#undef printlog
#undef printlog_vv
#undef printlog_error
