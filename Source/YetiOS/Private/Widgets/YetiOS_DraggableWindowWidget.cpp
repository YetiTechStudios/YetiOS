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

	bHasMouseCapture = false;
	bResizeEnabled = true;
	bIsResizing = false;
}

void UYetiOS_DraggableWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	HandledEvent.NativeReply = FReply::Handled();

	OwningOS = OwningProgram->GetOwningOS();
	ParentSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);

	WindowTitleBorderWidget->OnMouseMoveEvent.BindUFunction(this, FName("OnMouseMove_WindowTitleBorder"));
	WindowTitleBorderWidget->OnMouseButtonUpEvent.BindUFunction(this, FName("OnMouseButtonUp_WindowTitleBorder"));
	WindowTitleBorderWidget->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_WindowTitleBorder"));

	if (ResizeAreaWidget)
	{
		ResizeAreaWidget->OnMouseButtonUpEvent.BindUFunction(this, FName("OnMouseButtonUp_ResizeArea"));
		ResizeAreaWidget->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_ResizeArea"));
	}
}

FReply UYetiOS_DraggableWindowWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Local_Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (bIsResizing)
	{
		if (bIsAlignmentAccountedFor)
		{
			FVector2D DummyPixelPosition;
			FVector2D OutViewportPosition;
			USlateBlueprintLibrary::AbsoluteToViewport(this, UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InMouseEvent), DummyPixelPosition, OutViewportPosition);
			const FVector2D NewSize = Internal_DetermineNewSize(OutViewportPosition - LastMousePosition);
			ParentSlot->SetSize(NewSize);
			LastMousePosition = OutViewportPosition;
		}
		else
		{
			ParentSlot->SetAlignment(FVector2D::ZeroVector);
			ParentSlot->SetPosition(ParentSlot->GetPosition() - PreResizeOffset);
			bIsAlignmentAccountedFor = true;
		}
	}

	return Local_Reply;
}

FReply UYetiOS_DraggableWindowWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsResizing = false;
	UWidgetBlueprintLibrary::ReleaseMouseCapture(HandledEvent);
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
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

FEventReply UYetiOS_DraggableWindowWidget::OnMouseMove_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply EventReply = HandledEvent;
	if (bHasMouseCapture)
	{
		FVector2D MousePosition = OwningOS->GetOsWidget()->K2_GetMousePositionOnViewport(InMouseEvent);
		UCanvasPanelSlot* SlotPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
		SlotPanel->SetPosition(MousePosition - Offset);
		FEventReply Reply = HandledEvent;
		EventReply = UWidgetBlueprintLibrary::CaptureMouse(Reply, nullptr);
	}

	return EventReply;
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonUp_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	bHasMouseCapture = false;
	return HandledEvent;
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonDown_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	Offset = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InMouseEvent));
	bHasMouseCapture = (Offset.Y <= WindowTitleBorderWidget->GetDesiredSize().Y); 
	UCanvasPanelSlot* SlotPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	SlotPanel->SetZOrder(OwningOS->GetOsWidget()->GetRaisedZ_Order());
	return HandledEvent;
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonUp_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsResizing = false;
	return HandledEvent;
}

FEventReply UYetiOS_DraggableWindowWidget::OnMouseButtonDown_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bResizeEnabled)
	{
		bIsResizing = true;
		FVector2D DummyPixelPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(this, UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InMouseEvent), DummyPixelPosition, LastMousePosition);
		PreResizeAlignment = ParentSlot->GetAlignment();
		PreDragSize = ParentSlot->GetSize();
		PreResizeOffset = FVector2D(PreDragSize.X * PreResizeAlignment.X, PreDragSize.Y * PreResizeAlignment.Y);
		return K2_OnResizeStart(InMouseEvent);
	}

	return HandledEvent;
}

FText UYetiOS_DraggableWindowWidget::GetWindowText() const
{
	return OwningProgram->GetProgramName();
}

void UYetiOS_DraggableWindowWidget::CloseWindow()
{
	ProgramCanvas->ClearChildren();
	RemoveFromParent();
}

void UYetiOS_DraggableWindowWidget::AddProgramWidget(class UYetiOS_AppWidget* InWidget)
{
	ProgramCanvas->AddChildToCanvas(InWidget);
	InWidget->SetWindow(this);
	K2_OnProgramAdded(InWidget);
}
