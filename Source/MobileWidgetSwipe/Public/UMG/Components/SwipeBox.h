// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Styling/SlateTypes.h"
#include "Slate/Styling/SlateTypesExtension.h"
#include "Widgets/SWidget.h"
#include "Slate/Widgets/Layout/SSwipeBox.h"
#include "Components/PanelWidget.h"
#include "Containers/Ticker.h"
#include "SwipeBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserSwipeedEvent, float, CurrentOffset);

/**
 * An arbitrary Swipeable collection of widgets.  Great for presenting 10-100 widgets in a list.  Doesn't support virtualization.
 */
UCLASS()
class MOBILEWIDGETSWIPE_API USwipeBox : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:

	UE_DEPRECATED(5.2, "Direct access to WidgetStyle is deprecated. Please use the getter or setter.")
	/** The style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category="Style", meta=( DisplayName="Style" ))
	FSwipeBoxStyle WidgetStyle;

	UE_DEPRECATED(5.2, "Direct access to WidgetBarStyle is deprecated. Please use the getter or setter.")
	/** The bar style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category="Style", meta=( DisplayName="Bar Style" ))
	FSwipeBarStyle WidgetBarStyle;

	UE_DEPRECATED(5.2, "Direct access to Orientation is deprecated. Please use the getter or setter.")
	/** The orientation of the Swipeing and stacking in the box. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetOrientation", Category="Swipe")
	TEnumAsByte<EOrientation> Orientation;

	UE_DEPRECATED(5.2, "Direct access to SwipeBarVisibility is deprecated. Please use the getter or setter.")
	/** Visibility */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetSwipeBarVisibility", Category="Swipe")
	ESlateVisibility SwipeBarVisibility;

	UE_DEPRECATED(5.2, "Direct access to ConsumeMouseWheel is deprecated. Please use the getter or setter.")
	/** When mouse wheel events should be consumed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetConsumeMouseWheel", Category = "Swipe")
	EConsumeMouseWheel ConsumeMouseWheel;

	UE_DEPRECATED(5.2, "Direct access to SwipeBarThickness is deprecated. Please use the getter or setter.")
	/** The thickness of the SwipeBar thumb */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetSwipeBarThickness", Category="Swipe")
	FVector2D SwipeBarThickness;

	UE_DEPRECATED(5.2, "Direct access to SwipeBarPadding is deprecated. Please use the getter or setter.")
	/** The margin around the SwipeBar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetSwipeBarPadding", Category = "Swipe")
	FMargin SwipeBarPadding;

	/**  */
	UE_DEPRECATED(5.2, "Direct access to AlwaysShowSwipeBar is deprecated. Please use the getter or setter.")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAlwaysShowSwipeBar", Setter, BlueprintSetter = "SetAlwaysShowSwipeBar", Category = "Swipe")
	bool AlwaysShowSwipeBar;

	/**  */
	UE_DEPRECATED(5.2, "Direct access to AlwaysShowSwipeBarTrack is deprecated. Please use the getter or setter.")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAlwaysShowSwipeBarTrack", Setter, Category = "Swipe")
	bool AlwaysShowSwipeBarTrack;

	UE_DEPRECATED(5.2, "Direct access to AllowOverSwipe is deprecated. Please use the getter or setter.")
	/**  Set the fill option of children to take the whole size along the parent orientation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsSlotFillChildAsParentSize", Category = "Swipe")
	bool SlotFillChildAsParentSize;

	UE_DEPRECATED(5.2, "Direct access to AllowOverSwipe is deprecated. Please use the getter or setter.")
	/**  Disable to stop SwipeBars from activating inertial overSwipeing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAllowOverSwipe", Setter, BlueprintSetter = "SetAllowOverSwipe", Category = "Swipe")
	bool AllowOverSwipe;
	
	UE_DEPRECATED(5.2, "Direct access to AllowStickySwipe is deprecated. Please use the getter or setter.")
	/**  Disable to stop SwipeBars from activating inertial stickySwipeing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAllowStickySwipe", Setter, BlueprintSetter = "SetAllowStickySwipe", Category = "Swipe")
	bool AllowStickySwipe;

	UE_DEPRECATED(5.2, "Direct access to BackPadSwipeing is deprecated. Please use the getter. Note that BackPadSwipeing is only set at construction and is not modifiable at runtime.")
	/** Whether to back pad this Swipe box, allowing user to Swipe backward until child contents are no longer visible */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Getter = "IsBackPadSwipeing", Category = "Swipe")
	bool BackPadSwipeing;

	UE_DEPRECATED(5.2, "Direct access to FrontPadSwipeing is deprecated. Please use the getter. Note that FrontPadSwipeing is only set at construction and is not modifiable at runtime.")
	/** Whether to front pad this Swipe box, allowing user to Swipe forward until child contents are no longer visible */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Getter = "IsFrontPadSwipeing", Category = "Swipe")
	bool FrontPadSwipeing;
	
	UE_DEPRECATED(5.2, "Direct access to bAnimateWheelSwipeing is deprecated. Please use the getter or setter.")
	/** True to lerp smoothly when wheel Swipeing along the Swipe box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAnimateWheelSwipeing", Setter = "SetAnimateWheelSwipeing", BlueprintSetter = "SetAnimateWheelSwipeing", Category = "Swipe")
	bool bAnimateWheelSwipeing = false;

	UE_DEPRECATED(5.2, "Direct access to Easing is deprecated. Please use the getter or setter.")
	/** True to lerp smoothly when wheel Swipeing along the Swipe box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "GetEasing", Setter = "SetEasing", Category = "Swipe|Page Transition")
	TEnumAsByte<EEasingFunc::Type> Easing;
	
	UE_DEPRECATED(5.2, "Direct access to BlendExp is deprecated. Please use the getter or setter.")
	/** True to lerp smoothly when wheel Swipeing along the Swipe box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "GetBlendExp", Setter = "SetBlendExp", Category = "Swipe|Page Transition")
	float BlendExp;
	
	UE_DEPRECATED(5.2, "Direct access to Speed is deprecated. Please use the getter or setter.")
	/** True to lerp smoothly when wheel Swipeing along the Swipe box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "GetSpeed", Setter = "SetSpeed", Category = "Swipe|Page Transition")
	float Speed;

	UE_DEPRECATED(5.2, "Direct access to NavigationDestination is deprecated. Please use the getter or setter.")
	/** Sets where to Swipe a widget to when using explicit navigation or if SwipeWhenFocusChanges is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetNavigationDestination", Getter, Setter, Category = "Swipe")
	EDescendantScrollDestination NavigationDestination;

	UE_DEPRECATED(5.2, "Direct access to NavigationSwipePadding is deprecated. Please use the getter. Note that NavigationSwipePadding is only set at construction and is not modifiable at runtime.")
	/**
	 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
	 * Swipebox.  Use this if you want to ensure there's a preview of the next item the user could Swipe to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Getter, Category="Swipe")
	float NavigationSwipePadding;

	UE_DEPRECATED(5.2, "Direct access to SwipeWhenFocusChanges is deprecated. Please use the getter or setter.")
	/** Swipe behavior when user focus is given to a child widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetSwipeWhenFocusChanges", Category="Swipe", meta=(DisplayName="Swipe When Focus Changes"))
	EScrollWhenFocusChanges SwipeWhenFocusChanges;
	
	UE_DEPRECATED(5.2, "Direct access to bAllowRightClickDragSwipeing is deprecated. Please use the getter or setter.")
	/** Option to disable right-click-drag Swipeing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "IsAllowRightClickDragSwipeing", Setter = "SetAllowRightClickDragSwipeing", Category = "Swipe")
	bool bAllowRightClickDragSwipeing;

	UE_DEPRECATED(5.2, "Direct access to WheelSwipeMultiplier is deprecated. Please use the getter or setter.")
	/** The multiplier to apply when wheel Swipeing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetWheelSwipeMultiplier", Category = "Swipe")
	float WheelSwipeMultiplier = 1.f;

	void SetWidgetStyle(const FSwipeBoxStyle& NewWidgetStyle);

	const FSwipeBoxStyle& GetWidgetStyle() const;

	void SetWidgetBarStyle(const FSwipeBarStyle& NewWidgetBarStyle);

	const FSwipeBarStyle& GetWidgetBarStyle() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel);

	EConsumeMouseWheel GetConsumeMouseWheel() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetOrientation(EOrientation NewOrientation);

	EOrientation GetOrientation() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetSwipeBarVisibility(ESlateVisibility NewSwipeBarVisibility);

	ESlateVisibility GetSwipeBarVisibility() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetSwipeBarThickness(const FVector2D& NewSwipeBarThickness);

	FVector2D GetSwipeBarThickness() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetSwipeBarPadding(const FMargin& NewSwipeBarPadding);

	FMargin GetSwipeBarPadding() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetAlwaysShowSwipeBar(bool NewAlwaysShowSwipeBar);
	
	bool IsAlwaysShowSwipeBar() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetAllowOverSwipe(bool NewAllowOverSwipe);

	bool IsAllowOverSwipe() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetAllowStickySwipe(bool NewAllowStickySwipe);

	bool IsAllowStickySwipe() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetAnimateWheelSwipeing(bool bShouldAnimateWheelSwipeing);

	bool IsAnimateWheelSwipeing() const;

	
	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	EEasingFunc::Type GetEasing() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	void SetEasing(EEasingFunc::Type NewEasing);

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	float GetBlendExp() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	void SetBlendExp(float NewBlendExp);

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	float GetSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	void SetSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	int GetCurrentPage() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe|Page Transition")
	void SetCurrentPage(int32 NewCurrentPage);

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetWheelSwipeMultiplier(float NewWheelSwipeMultiplier);

	float GetWheelSwipeMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetSwipeWhenFocusChanges(EScrollWhenFocusChanges NewSwipeWhenFocusChanges);

	EScrollWhenFocusChanges GetSwipeWhenFocusChanges() const;

	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination);

	EDescendantScrollDestination GetNavigationDestination() const;

	void SetAlwaysShowSwipeBarTrack(bool NewAlwaysShowSwipeBarTrack);

	bool IsAlwaysShowSwipeBarTrack() const;

	float GetNavigationSwipePadding() const;

	void SetAllowRightClickDragSwipeing(bool bShouldAllowRightClickDragSwipeing);

	bool IsAllowRightClickDragSwipeing() const;

	bool IsFrontPadSwipeing() const;

	bool IsBackPadSwipeing() const;
	
	bool IsSlotFillChildAsParentSize() const;

	/** Instantly stops any inertial Swipeing that is currently in progress */
	UFUNCTION(BlueprintCallable, Category = "Swipe")
	void EndInertialSwipeing();

public:

	/** Called when the Swipe has changed */
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnUserSwipeedEvent OnUserSwipeed;

	/**
	 * Updates the Swipe offset of the Swipebox.
	 * @param NewSwipeOffset is in Slate Units.
	 */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SetSwipeOffset(float NewSwipeOffset);
	
	/**
	 * Gets the Swipe offset of the Swipebox in Slate Units.
	 */
	UFUNCTION(BlueprintCallable, Category="Widget")
	float GetSwipeOffset() const;

	/** Gets the Swipe offset of the bottom of the SwipeBox in Slate Units. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	float GetSwipeOffsetOfEnd() const;

	/** Gets the fraction currently visible in the Swipebox */
	UFUNCTION(BlueprintCallable, Category="Widget")
	float GetViewFraction() const;

	UFUNCTION(BlueprintCallable, Category="Widget")
	float GetViewOffsetFraction() const;

	/** Swipes the SwipeBox to the top instantly */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SwipeToStart();

	/** Swipes the SwipeBox to the bottom instantly during the next layout pass. */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SwipeToEnd();

	/** Swipes the SwipeBox to the widget during the next layout pass. */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SwipeWidgetIntoView(UWidget* WidgetToFind, bool AnimateSwipe = true, EDescendantScrollDestination SwipeDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UObject Interface
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FArchive& Ar) override;
#endif // if WITH_EDITORONLY_DATA
	//~ End UObject Interface

#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
	virtual void OnDescendantSelectedByDesigner( UWidget* DescendantWidget ) override;
	virtual void OnDescendantDeselectedByDesigner( UWidget* DescendantWidget ) override;
	//~ End UWidget Interface
#endif

protected:

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	void SlateHandleUserSwipeed(float CurrentOffset);

	// Initialize IsFocusable in the constructor before the SWidget is constructed.
	void InitBackPadSwipeing(bool InBackPadSwipeing);
	// Initialize IsFocusable in the constructor before the SWidget is constructed.
	void InitFrontPadSwipeing(bool InFrontPadSwipeing);
	// Initialize IsFocusable in the constructor before the SWidget is constructed.
	void InitNavigationSwipePadding(float InNavigationSwipePadding);

protected:
	/** The desired Swipe offset for the underlying Swipebox.  This is a cache so that it can be set before the widget is constructed. */
	float DesiredSwipeOffset;

	TSharedPtr<class SSwipeBox> MySwipeBox;

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

#if WITH_EDITOR
	FTSTicker::FDelegateHandle TickHandle;
#endif //WITH_EDITOR
};
