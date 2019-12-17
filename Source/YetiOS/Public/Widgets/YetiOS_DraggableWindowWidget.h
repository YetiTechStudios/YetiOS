// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_DraggableWindowWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_DraggableWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMinWidth;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMaxWidth;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMinHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMaxHeight;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Draggable Window Widget", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* OwningProgram;
	
	UPROPERTY()
	class UYetiOS_Core* OwningOS;

	UPROPERTY(meta = (BindWidget))
	class UBorder* WindowTitleBorderWidget;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ProgramCanvas;

	UPROPERTY(meta = (BindWidgetOptional))
	class UBorder* ResizeAreaWidget;

	UPROPERTY()
	class UCanvasPanelSlot* ParentSlot;

	uint8 bHasMouseCapture : 1;
	uint8 bResizeEnabled : 1;
	uint8 bIsResizing : 1;
	uint8 bIsAlignmentAccountedFor : 1;
	FVector2D Offset;
	FVector2D LastMousePosition;
	FVector2D PreResizeAlignment;
	FVector2D PreResizeOffset;
	FVector2D PreDragSize;
	FEventReply HandledEvent;

public:

	UYetiOS_DraggableWindowWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:

	const FVector2D Internal_DetermineNewSize(const FVector2D& InDelta) const;

protected:

	UFUNCTION()
	FEventReply OnMouseMove_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonUp_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonDown_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonUp_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonDown_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintPure, Category = "Yeti OS Draggable Window")
	FText GetWindowText() const;

public:

	void CloseWindow();

protected:

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Draggable Window")
	void AddProgramWidget(class UYetiOS_AppWidget* InWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Draggable Window", DisplayName = "OnProgramAdded")
	void K2_OnProgramAdded(const UUserWidget* AddedUserWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Draggable Window", DisplayName = "OnResizeStart")
	FEventReply K2_OnResizeStart(const FPointerEvent& InMouseEvent);

};
