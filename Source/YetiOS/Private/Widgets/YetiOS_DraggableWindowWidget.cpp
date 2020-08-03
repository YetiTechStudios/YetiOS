// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_DraggableWindowWidget.h"
#include "Widgets/YetiOS_OsWidget.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_Core.h"
#include "Components/SlateWrapperTypes.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/Reply.h"
#include "Components/CanvasPanel.h"


UYetiOS_DraggableWindowWidget::UYetiOS_DraggableWindowWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ResizeMinWidth = 600.f;
	ResizeMaxWidth = 0.f;

	ResizeMinHeight = 400.f;
	ResizeMaxHeight = 0.f;

	bEnableDrag = bEnableResizing = true;

	bIsMouseButtonDown = false;
	bIsDragging = false;
	bIsResizing = false;
	bIsAlignmentAccountedFor = false;

	LastMousePosition = PreResizeAlignment = PreResizeOffset = PreDragSize = FVector2D::ZeroVector;
	bIsFocusable = true;
}

void UYetiOS_DraggableWindowWidget::NativeConstruct()
{
	ParentSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	BringWindowToFront();

	WindowTitleBorderWidget->OnMouseButtonUpEvent.BindUFunction(this, FName("OnMouseButtonUp_WindowTitleBorder"));
	WindowTitleBorderWidget->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_WindowTitleBorder"));

	if (bEnableResizing && ResizeAreaWidget)
	{
		ResizeAreaWidget->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_ResizeArea"));
	}

	Super::NativeConstruct();
}

FReply UYetiOS_DraggableWindowWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (bIsMouseButtonDown)
	{
		FVector2D OutPixelPosition, OutViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(this, InMouseEvent.GetScreenSpacePosition(), OutPixelPosition, OutViewportPosition);
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		const bool bIsMouseOffScreen = OutPixelPosition.X < 5.f || OutPixelPosition.Y < 5.f || OutPixelPosition.X > (ViewportSize.X - 5.f) || OutPixelPosition.Y >(ViewportSize.Y - 5.f);
		if (bIsMouseOffScreen)
		{
			Internal_OnMouseButtonUpEvent();
			return FReply::Handled();
		}

		USlateBlueprintLibrary::AbsoluteToViewport(this, InMouseEvent.GetScreenSpacePosition(), OutPixelPosition, OutViewportPosition);
		FVector2D MouseDelta = OutViewportPosition - LastMousePosition;
		FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(FName("LeftMouseButton")));
		if (bIsDragging)
		{
			ParentSlot->SetPosition(ParentSlot->GetPosition() + MouseDelta);
		}
		else if (bIsResizing)
		{
			if (bIsAlignmentAccountedFor)
			{
				const FVector2D RequestedSize = Internal_DetermineNewSize(MouseDelta);
				ParentSlot->SetSize(RequestedSize);
			}
			else
			{
				ParentSlot->SetAlignment(FVector2D::ZeroVector);
				ParentSlot->SetPosition(ParentSlot->GetPosition() - PreResizeOffset);
				bIsAlignmentAccountedFor = true;
				return FReply::Handled();
			}
		}
		else
		{
			return UWidgetBlueprintLibrary::CaptureMouse(EventReply, this).NativeReply;
		}

		USlateBlueprintLibrary::AbsoluteToViewport(this, InMouseEvent.GetScreenSpacePosition(), OutPixelPosition, LastMousePosition);
		return UWidgetBlueprintLibrary::CaptureMouse(EventReply, this).NativeReply;
	}
	
	return FReply::Handled();
}

FReply UYetiOS_DraggableWindowWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	Internal_OnMouseButtonUpEvent();
	FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(FName("LeftMouseButton")));
	return UWidgetBlueprintLibrary::ReleaseMouseCapture(EventReply).NativeReply;
}

void UYetiOS_DraggableWindowWidget::Internal_OnMouseButtonUpEvent()
{
	if (bIsResizing && bIsAlignmentAccountedFor)
	{
		const FVector2D SizeDifference = ParentSlot->GetSize() - PreDragSize;
		const FVector2D NewPosition = (SizeDifference * PreResizeAlignment) + PreResizeOffset + (ParentSlot->GetPosition());
		ParentSlot->SetPosition(NewPosition);
		ParentSlot->SetAlignment(PreResizeAlignment);
	}

	bIsAlignmentAccountedFor = false;
	bIsMouseButtonDown = false;
	bIsDragging = false;
	bIsResizing = false;
}

const FVector2D UYetiOS_DraggableWindowWidget::Internal_DetermineNewSize(const FVector2D& InDelta) const
{
	const FVector2D Local_Original = ParentSlot->GetSize();
	const float Local_OriginalX = Local_Original.X;
	const float Local_OriginalY = Local_Original.Y;
	const float Local_DeltaX = InDelta.X;
	const float Local_DeltaY = InDelta.Y;

	float TempWidth = 0.f;
	float TempHeight = 0.f;

	if (ResizeMaxWidth > 0.f)
	{
		const float Local_ValueToClamp = Local_OriginalX + Local_DeltaX;
		TempWidth = FMath::Clamp(Local_ValueToClamp, ResizeMinWidth, ResizeMaxWidth);
	}
	else
	{
		TempWidth = FMath::Max<float>((Local_OriginalX + Local_DeltaX), ResizeMinWidth);
	}

	if (ResizeMaxHeight > 0.f)
	{
		const float Local_ValueToClamp = Local_OriginalY + Local_DeltaY;
		TempHeight = FMath::Clamp(Local_ValueToClamp, ResizeMinHeight, ResizeMaxHeight);
	}
	else
	{
		TempHeight = FMath::Max<float>((Local_OriginalY + Local_DeltaY), ResizeMinHeight);
	}

	return FVector2D(TempWidth, TempHeight);
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonUp_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply EventReply = FEventReply();
	EventReply.NativeReply = NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	return EventReply;
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonDown_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	BringWindowToFront();
	if (bEnableDrag)
	{
		OnMouseButtonUp_WindowTitleBorder(InGeometry, InMouseEvent);
		bIsMouseButtonDown = true;
		bIsDragging = true;

		FVector2D OutPixelPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(this, InMouseEvent.GetScreenSpacePosition(), OutPixelPosition, LastMousePosition);
		FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(FName("LeftMouseButton")));
		K2_OnDragStart(InMouseEvent);
		return UWidgetBlueprintLibrary::CaptureMouse(EventReply, this);
	}

	return FEventReply();
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonDown_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsMouseButtonDown = true;
	bIsResizing = true;

	FVector2D OutPixelPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(this, InMouseEvent.GetScreenSpacePosition(), OutPixelPosition, LastMousePosition);
	PreResizeAlignment = ParentSlot->GetAlignment();
	PreDragSize = ParentSlot->GetSize();
	PreResizeOffset = PreDragSize * PreResizeAlignment;
	FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(FName("LeftMouseButton")));
	K2_OnResizeStart(InMouseEvent);
	return UWidgetBlueprintLibrary::CaptureMouse(EventReply, this);
}

FText UYetiOS_DraggableWindowWidget::GetWindowText(const bool bWithProcessID /*= false*/) const
{
	if (bWithProcessID)
	{
		return FText::FromString(FString::Printf(TEXT("[%i] %s"), OwningProgram->GetProcessID(), *OwningProgram->GetProgramName().ToString()));
	}

	return OwningProgram->GetProgramName();
}

EYetiOsProgramVisibilityState UYetiOS_DraggableWindowWidget::GetCurrentVisibilityState() const
{
	return OwningProgram->GetCurrentVisibilityState();
}

void UYetiOS_DraggableWindowWidget::CloseWindow()
{
	OwningOS->GetOsWidget()->RemoveTaskbarButton(this);
	ProgramCanvas->ClearChildren();
	RemoveFromParent();
}

void UYetiOS_DraggableWindowWidget::BringWindowToFront()
{
	if (OwningOS)
	{
		OwningOS->UpdateWindowZOrder(this);
	}
}

bool UYetiOS_DraggableWindowWidget::ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState)
{
	if (OwningProgram->ChangeVisibilityState(InNewState))
	{
		K2_OnChangeVisibilityState(InNewState);
		return true;
	}

	return false;	
}

void UYetiOS_DraggableWindowWidget::AddProgramWidget(class UYetiOS_AppWidget* InWidget)
{
	OwningOS = OwningProgram->GetOwningOS();
	ProgramCanvas->AddChildToCanvas(InWidget);
	InWidget->SetWindow(this);
	K2_OnProgramAdded(InWidget);
	OwningOS->GetOsWidget()->AddTaskbarButton(this);
}
