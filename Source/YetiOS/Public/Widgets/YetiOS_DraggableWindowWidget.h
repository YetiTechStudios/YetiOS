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

	FDelegateHandle OnThemeChangedDelegateHandle;
	FDelegateHandle OnShowProgramIconDelegateHandle;
	
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

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Draggable Window Widget", meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* WindowTitleBorderWidget;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ProgramCanvas;

	UPROPERTY(meta = (BindWidgetOptional))
	class UBorder* ResizeAreaWidget;

	UPROPERTY()
	class UCanvasPanelSlot* ParentSlot;

	/* Allows you to drag this window in OS. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Draggable Window Widget")
	uint8 bEnableDrag : 1;

	/* Allows you to resize this window in OS. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Draggable Window Widget")
	uint8 bEnableResizing : 1;

	uint8 bIsMouseButtonDown : 1;
	uint8 bIsDragging : 1;
	uint8 bIsResizing : 1;
	uint8 bIsAlignmentAccountedFor : 1;
	
	FVector2D LastMousePosition;
	FVector2D PreResizeAlignment;
	FVector2D PreResizeOffset;
	FVector2D PreDragSize;

public:

	UYetiOS_DraggableWindowWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:

	void Internal_OnMouseButtonUpEvent();
	const FVector2D Internal_DetermineNewSize(const FVector2D& InDelta) const;

protected:

	UFUNCTION()
	FEventReply OnMouseButtonUp_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonDown_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION()
	FEventReply OnMouseButtonDown_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	FText GetWindowText(const bool bWithProcessID = false) const;

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	EYetiOsProgramVisibilityState GetCurrentVisibilityState() const;

public:

	void CloseWindow();

	bool BringWindowToFront();

protected:

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	virtual bool ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState);

public:

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	void AddWidget(class UYetiOS_UserWidget* InUserWidget);

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Widget Added")
	void K2_OnWidgetAdded(const UYetiOS_UserWidget* InWidget);

	/* Event called when dragging is started. Only called if Enable Drag is true. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Drag Start")
	void K2_OnDragStart(const FPointerEvent& InMouseEvent);

	/* Event called when resizing is started. Only called if Enable Resizing is true. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Resize Start")
	void K2_OnResizeStart(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Visibility State Changed")
	void K2_OnChangeVisibilityState(const EYetiOsProgramVisibilityState NewState);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Theme Changed")
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Show Program Icon")
	void K2_OnShowProgramIcon(const bool bShow);

};
