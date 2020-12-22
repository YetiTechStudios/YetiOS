// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_DraggableWindowWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_DraggableWindowWidget.h

* Description:
Widget that can be dragged around like windows.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Draggable Window")
class YETIOS_API UYetiOS_DraggableWindowWidget : public UUserWidget
{
	GENERATED_BODY()

	FDelegateHandle OnThemeChangedDelegateHandle;
	FDelegateHandle OnShowProgramIconDelegateHandle;
	FDelegateHandle OnPeekPreviewDelegateHandle;
	
private:

	/** Minimum size that is allowed to resize. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMinWidth;

	/** Maximum size that is allowed to resize. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMaxWidth;

	/** Minimum size that is allowed to resize. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMinHeight;

	/** Maximum size that is allowed to resize. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Draggable Window Widget")
	float ResizeMaxHeight;

	/** Program that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Draggable Window Widget", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* OwningProgram;
	
	/** Operating system that owns this widget. */
	UPROPERTY()
	class UYetiOS_Core* OwningOS;

	/** Border widget that the developer should create in UMG designer */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Draggable Window Widget", meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* WindowTitleBorderWidget;

	/** Canvas Panel that the developer should create in UMG designer */
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ProgramCanvas;

	/** Border widget that the developer should create in UMG designer */
	UPROPERTY(meta = (BindWidgetOptional))
	class UBorder* ResizeAreaWidget;

	/** Parent Canvas Panel of this widget. Should not be null because dragging and resizing can ONLY be done if this widget is a child of Canvas Panel. */
	UPROPERTY()
	class UCanvasPanelSlot* ParentSlot;

	/** Allows you to drag this window in OS. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Draggable Window Widget")
	uint8 bEnableDrag : 1;

	/** Allows you to resize this window in OS. */
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

	/**
	* private UYetiOS_DraggableWindowWidget::Internal_OnMouseButtonUpEvent
	* Event called when mouse button up has happened.
	**/
	void Internal_OnMouseButtonUpEvent();

	/**
	* private UYetiOS_DraggableWindowWidget::Internal_DetermineNewSize const
	* Determines the new size of this windows.
	* @param InDelta [const FVector2D&] Change to size happened in current frame.
	* @return [const FVector2D] New size.
	**/
	const FVector2D Internal_DetermineNewSize(const FVector2D& InDelta) const;

protected:

	/** Event called when mouse button up is detected on WindowTitleBorder */
	UFUNCTION()
	FEventReply OnMouseButtonUp_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	/** Event called when mouse button down is detected on WindowTitleBorder */
	UFUNCTION()
	FEventReply OnMouseButtonDown_WindowTitleBorder(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	/** Event called when mouse button down is detected on ResizeAreaWidget */
	UFUNCTION()
	FEventReply OnMouseButtonDown_ResizeArea(FGeometry InGeometry, const FPointerEvent& InMouseEvent);

	/**
	* protected UYetiOS_DraggableWindowWidget::GetWindowText const
	* Returns the text of this window (optionally includes the process ID of owning program).
	* @param bWithProcessID [const bool] Whether process ID of owning program should be included.
	* @return [FText] Program Name
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS Draggable Window")	
	FText GetWindowText(const bool bWithProcessID = false) const;

	/**
	* protected UYetiOS_DraggableWindowWidget::GetCurrentVisibilityState const
	* Gets the current visibility state of this window.
	* @return [EYetiOsProgramVisibilityState] Current state.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	EYetiOsProgramVisibilityState GetCurrentVisibilityState() const;

public:

	/**
	* public UYetiOS_DraggableWindowWidget::CloseWindow
	* Close this window.
	**/
	void CloseWindow();

	/**
	* public UYetiOS_DraggableWindowWidget::BringWindowToFront
	* Updates the Z order and brings window to front.
	* @return [bool] True if window was brought to front.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Draggable Window")
	bool BringWindowToFront();

	/**
	* public UYetiOS_DraggableWindowWidget::OnPeekPreview
	* Automatically called when user peeks desktop.
	* @param bBegin [const bool] Is the user peeking? I mean peeking desktop :P
	**/
	void OnPeekPreview(const bool bBegin);

protected:

	/**
	* virtual protected UYetiOS_DraggableWindowWidget::ChangeVisibilityState
	* Changes the visibility state to a new one.
	* @param InNewState [const EYetiOsProgramVisibilityState] New visibility state.
	* @return [bool] True if the visibility state was changed.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Draggable Window")	
	virtual bool ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState);

public:

	/**
	* public UYetiOS_DraggableWindowWidget::AddWidget
	* Adds the given widget as a child to this window.
	* @param InUserWidget [class UYetiOS_UserWidget*] Widget to add.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS Draggable Window")	
	void AddWidget(class UYetiOS_UserWidget* InUserWidget);

protected:

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnWidgetAdded
	* Event called when a new child widget has been added.
	* @param InWidget [const UYetiOS_UserWidget*] Newly added child widget.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Widget Added")	
	void K2_OnWidgetAdded(const UYetiOS_UserWidget* InWidget);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnDragStart
	* Event called when dragging is started. Only called if Enable Drag is true.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Drag Start")	
	void K2_OnDragStart(const FPointerEvent& InMouseEvent);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnResizeStart
	* Event called when resizing is started. Only called if Enable Resizing is true.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Resize Start")	
	void K2_OnResizeStart(const FPointerEvent& InMouseEvent);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnChangeVisibilityState
	* Event called when visibility state is changed.
	* @param NewState [const EYetiOsProgramVisibilityState] New visibility state.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Visibility State Changed")	
	void K2_OnChangeVisibilityState(const EYetiOsProgramVisibilityState NewState);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnThemeChanged
	* Event called when theme was changed.
	* @param InNewTheme [const EYetiOsThemeMode] New theme.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Theme Changed")	
	void K2_OnThemeChanged(const EYetiOsThemeMode InNewTheme);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnShowProgramIcon
	* Event called when program icon show on windows has changed.
	* @param bShow [const bool] True if program icon should be shown.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Show Program Icon")	
	void K2_OnShowProgramIcon(const bool bShow);

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnBeginPeekPreview
	* Event called when user begins peeking. I mean peeking desktop :P
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On Begin Peek Preview")	
	void K2_OnBeginPeekPreview();

	/**
	* protected UYetiOS_DraggableWindowWidget::K2_OnEndPeekPreview
	* Event called when user stops peeking desktop.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Draggable Window", DisplayName = "On End Peek Preview")	
	void K2_OnEndPeekPreview();

public:

	/**
	* public UYetiOS_DraggableWindowWidget::K2_OnUpdateWindowText
	* Event called when windows text is updated.
	* @param NewText [const FText&] New text.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, BlueprintCallable, Category = "Yeti OS Draggable Window", DisplayName = "Update Window Text")	
	void K2_OnUpdateWindowText(const FText& NewText);

};
