// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_BaseDialogProgram.h"
#include "Widgets/YetiOS_DialogWidget.h"

void UYetiOS_BaseDialogProgram::Internal_SetDialogWidget(class UYetiOS_DialogWidget* InDialogWidget)
{
	DialogWidget = InDialogWidget;
}

void UYetiOS_BaseDialogProgram::CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown /*= false*/)
{
	DialogWidget->CloseDialog();
	Super::CloseProgram(OutErrorMessage);
}
