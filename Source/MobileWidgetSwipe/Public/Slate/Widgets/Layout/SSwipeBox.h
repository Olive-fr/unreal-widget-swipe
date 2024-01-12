// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "SlotBase.h"
#include "Layout/Geometry.h"
#include "Styling/SlateColor.h"
#include "Input/CursorReply.h"
#include "Input/Reply.h"
#include "Input/NavigationReply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Layout/Children.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Slate/Widgets/Layout/SSwipeBar.h"
#include "Framework/Layout/InertialScrollManager.h"
#include "Framework/Layout/OverScroll.h"
#include "Slate/Framework/Layout/Stickyswipe.h"
#include "Slate/Styling/SlateTypesExtension.h"

// #include "SSwipeBox.generated.h"

class FPaintArgs;
class FSlateWindowElementList;
class SSwipePanel;

/** Where to Swipe the descendant to */
// UENUM(BlueprintType)
// enum class EDescendantSwipeDestination : uint8
// {
// 	/**
// 	* Swipe the widget into view using the least amount of energy possible.  So if the new item
// 	* is above the visible set, it will stop as soon as it's in view at the top.  If it's below the
// 	* visible set, it stop it comes into view at the bottom.
// 	*/
// 	IntoView,
//
// 	/** Always Swipe the widget so it appears at the top/Left of the Swipeable area. */
// 	TopOrLeft,
//
// 	/**
// 	* Always Swipe the widget so it appears at the center of the Swipeable area, if possible.
// 	* This won't be possible for the first few items and the last few items, as there's not enough
// 	* slack.
// 	*/
// 	Center,
//
// 	/** Always Swipe the widget so it appears at the bottom/Right of the Swipeable area. */
// 	BottomOrRight,
// };

/** Set behavior when user focus changes inside this Swipe box */
// UENUM(BlueprintType)
// enum class ESwipeWhenFocusChanges : uint8
// {
// 	/** Don't automatically Swipe, navigation or child widget will handle this */
// 	NoSwipe,
//
// 	/** Instantly Swipe using NavigationDestination rule */
// 	InstantSwipe,
//
// 	/** Use animation to Swipe using NavigationDestination rule */
// 	AnimatedSwipe,
// };

/** SSwipeBox can Swipe through an arbitrary number of widgets. */
class MOBILEWIDGETSWIPE_API SSwipeBox : public SCompoundWidget
{
public:
	/** A Slot that provides layout options for the contents of a Swipeable box. */
	class FSlot : public TBasicLayoutWidgetSlot<FSlot>
	{
	public:
		SLATE_SLOT_BEGIN_ARGS(FSlot, TBasicLayoutWidgetSlot<FSlot>)
			SLATE_ARGUMENT(TOptional<FSizeParam>, SizeParam)
			TAttribute<float> _MaxSize;

			/** The widget's DesiredSize will be used as the space required. */
			FSlotArguments& AutoSize()
			{
				_SizeParam = FAuto();
				return Me();
			}

			/** The available space will be distributed proportionately. */
			FSlotArguments& FillSize(TAttribute<float> InStretchCoefficient)
			{
				_SizeParam = FStretch(MoveTemp(InStretchCoefficient));
				return Me();
			}

			/** Set the max size in SlateUnit this slot can be. */
			FSlotArguments& MaxSize(TAttribute<float> InMaxHeight)
			{
				_MaxSize = MoveTemp(InMaxHeight);
				return Me();
			}

		SLATE_SLOT_END_ARGS()

		/** Default values for a slot. */
		FSlot()
			: TBasicLayoutWidgetSlot<FSlot>(HAlign_Fill, VAlign_Fill)
			  , SizeRule(FSizeParam::SizeRule_Auto)
			  , SizeValue(*this, 1.f)
			  , MaxSize(*this, 0.0f)
		{
		}

		void Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs);
		static void RegisterAttributes(FSlateWidgetSlotAttributeInitializer& AttributeInitializer);

		/** Get the space rule this slot should occupy along SwipeBox's direction. */
		FSizeParam::ESizeRule GetSizeRule() const
		{
			return SizeRule;
		}

		/** Get the space rule value this slot should occupy along SwipeBox's direction. */
		float GetSizeValue() const
		{
			return SizeValue.Get();
		}

		/** Get the max size the slot can be.*/
		float GetMaxSize() const
		{
			return MaxSize.Get();
		}

		/** Set the size Param of the slot, It could be a FStretch or a FAuto. */
		void SetSizeParam(FSizeParam InSizeParam)
		{
			SizeRule = InSizeParam.SizeRule;
			SizeValue.Assign(*this, MoveTemp(InSizeParam.Value));
		}

		/** The widget's DesiredSize will be used as the space required. */
		void SetSizeToAuto()
		{
			SetSizeParam(FAuto());
		}

		/** The available space will be distributed proportionately. */
		void SetSizeToStretch(TAttribute<float> StretchCoefficient)
		{
			SetSizeParam(FStretch(MoveTemp(StretchCoefficient)));
		}

		/** Set the max size in SlateUnit this slot can be. */
		void SetMaxSize(TAttribute<float> InMaxSize)
		{
			MaxSize.Assign(*this, MoveTemp(InMaxSize));
		}

	private:
		/**
		 * How much space this slot should occupy along SwipeBox's direction.
		 * When SizeRule is SizeRule_Auto, the widget's DesiredSize will be used as the space required.
		 * When SizeRule is SizeRule_Stretch, the available space will be distributed proportionately between
		 * peer Widgets depending on the Value property. Available space is space remaining after all the
		 * peers' SizeRule_Auto requirements have been satisfied.
		 */

		/** The sizing rule to use. */
		FSizeParam::ESizeRule SizeRule;

		/** The actual value this size parameter stores. */
		typename TBasicLayoutWidgetSlot<FSlot>::template TSlateSlotAttribute<float> SizeValue;

		/** The max size that this slot can be (0 if no max) */
		typename TBasicLayoutWidgetSlot<FSlot>::template TSlateSlotAttribute<float> MaxSize;
	};

	SLATE_BEGIN_ARGS(SSwipeBox)
			: _Style(&FAppStyle::Get().GetWidgetStyle<FSwipeBoxStyle>("SwipeBox"))
			  , _SwipeBarStyle(&FAppStyle::Get().GetWidgetStyle<FSwipeBarStyle>("SwipeBar"))
			  , _ExternalSwipebar()
			  , _Orientation(Orient_Vertical)
			  , _SwipeBarVisibility(EVisibility::Visible)
			  , _SwipeBarAlwaysVisible(false)
			  , _SwipeBarDragFocusCause(EFocusCause::Mouse)
			  , _SwipeBarThickness(FVector2f(_Style->BarThickness, _Style->BarThickness))
			  , _SwipeBarPadding(2.0f)
			  , _AllowOverSwipe(EAllowOverscroll::Yes)
			  , _AllowStickySwipe(EAllowStickyswipe::Yes)
			  , _SlotStretchChildAsParentSize(true)
			  , _BackPadSwipeing(false)
			  , _FrontPadSwipeing(false)
			  , _AnimateWheelSwipeing(false)
			  , _WheelSwipeMultiplier(1.f)
			  , _NavigationDestination(EDescendantScrollDestination::IntoView)
			  , _NavigationSwipePadding(0.0f)
			  , _SwipeWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
			  , _OnUserSwipeed()
			  , _ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
		{
			_Clipping = EWidgetClipping::ClipToBounds;
		}

		SLATE_SLOT_ARGUMENT(FSlot, Slots)

		/** Style used to draw this SwipeBox */
		SLATE_STYLE_ARGUMENT(FSwipeBoxStyle, Style)

		/** Style used to draw this SwipeBox's Swipebar */
		SLATE_STYLE_ARGUMENT(FSwipeBarStyle, SwipeBarStyle)

		/** Custom Swipe bar */
		SLATE_ARGUMENT(TSharedPtr<SSwipeBar>, ExternalSwipebar)

		/** The direction that children will be stacked, and also the direction the box will Swipe. */
		SLATE_ARGUMENT(EOrientation, Orientation)

		SLATE_ARGUMENT(EVisibility, SwipeBarVisibility)

		SLATE_ARGUMENT(bool, SwipeBarAlwaysVisible)

		SLATE_ARGUMENT(EFocusCause, SwipeBarDragFocusCause)

		SLATE_ARGUMENT(UE::Slate::FDeprecateVector2DParameter, SwipeBarThickness)

		/** This accounts for total internal Swipe bar padding; default 2.0f padding from the Swipe bar itself is removed */
		SLATE_ARGUMENT(FMargin, SwipeBarPadding)

		SLATE_ARGUMENT(EAllowOverscroll, AllowOverSwipe);
	
		SLATE_ARGUMENT(EAllowStickyswipe, AllowStickySwipe);

		SLATE_ARGUMENT(bool, SlotStretchChildAsParentSize);

		SLATE_ARGUMENT(bool, BackPadSwipeing);

		SLATE_ARGUMENT(bool, FrontPadSwipeing);

		SLATE_ARGUMENT(bool, AnimateWheelSwipeing);

		SLATE_ARGUMENT(float, WheelSwipeMultiplier);
	
		SLATE_ARGUMENT(EEasingFunc::Type, Easing);
		SLATE_ARGUMENT(float, BlendExp);
		SLATE_ARGUMENT(float, Speed);
		SLATE_ARGUMENT(int, CurrentPageId);

		SLATE_ARGUMENT(EDescendantScrollDestination, NavigationDestination);

		/**
		 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
		 * SwipeBox.  Use this if you want to ensure there's a preview of the next item the user could Swipe to.
		 */
		SLATE_ARGUMENT(float, NavigationSwipePadding);

		SLATE_ARGUMENT(EScrollWhenFocusChanges, SwipeWhenFocusChanges);

		/** Called when the button is clicked */
		SLATE_EVENT(FOnUserSwipeed, OnUserSwipeed)

		SLATE_ARGUMENT(EConsumeMouseWheel, ConsumeMouseWheel);

	SLATE_END_ARGS()

	SSwipeBox();

	/** @return a new slot. Slots contain children for SSwipeBox */
	static FSlot::FSlotArguments Slot();

	void Construct(const FArguments& InArgs);

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	/** Adds a slot to SSwipeBox */
	FScopedWidgetSlotArguments AddSlot();

	/** Removes a slot at the specified location */
	void RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove);

	/** Removes all children from the box */
	void ClearChildren();

	/** @return Returns true if the user is currently interactively Swipeing the view by holding
		        the right mouse button and dragging. */
	bool IsRightClickSwipeing() const;

	EAllowOverscroll GetAllowOverSwipe() const;

	void SetAllowOverSwipe(EAllowOverscroll NewAllowOverSwipe);

	EAllowStickyswipe GetAllowStickySwipe() const;

	void SetAllowStickySwipe(EAllowStickyswipe NewAllowStickySwipe);

	EEasingFunc::Type GetEasing() const;

	void SetEasing(EEasingFunc::Type NewEasing);

	float GetBlendExp() const;

	void SetBlendExp(float NewBlendExp);

	float GetSpeed() const;

	void SetSpeed(float NewSpeed);

	int GetCurrentPage() const;

	void SetCurrentPage(int NewPage);

	void SetAnimateWheelSwipeing(bool bInAnimateWheelSwipeing);

	void SetWheelSwipeMultiplier(float NewWheelSwipeMultiplier);

	void SetSwipeWhenFocusChanges(EScrollWhenFocusChanges NewSwipeWhenFocusChanges);

	float GetSwipeOffset() const;

	float GetViewFraction() const;

	float GetViewOffsetFraction() const;

	/** Gets the Swipe offset of the bottom of the SwipeBox in Slate Units. */
	float GetSwipeOffsetOfEnd() const;

	void SetSwipeOffset(float NewSwipeOffset);

	void SwipeToStart();

	void SwipeToEnd();

	void EndInertialSwipeing();

	/** 
	 * Attempt to Swipe a widget into view, will safely handle non-descendant widgets 
	 *
	 * @param WidgetToFind The widget whose geometry we wish to discover.
	 * @param InAnimateSwipe	Whether or not to animate the Swipe
	 * @param InDestination		Where we want the child widget to stop.
	 */
	void SwipeDescendantIntoView(const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateSwipe = true,
	                             EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView,
	                             float Padding = 0);

	/** Get the current orientation of the SwipeBox. */
	EOrientation GetOrientation();

	void SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination);

	void SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel);

	/** Sets the current orientation of the SwipeBox and updates the layout */
	void SetOrientation(EOrientation InOrientation);

	void SetSwipeBarVisibility(EVisibility InVisibility);

	void SetSwipeBarAlwaysVisible(bool InAlwaysVisible);

	void SetSwipeBarTrackAlwaysVisible(bool InAlwaysVisible);

	void SetSwipeBarThickness(UE::Slate::FDeprecateVector2DParameter InThickness);

	void SetSwipeBarPadding(const FMargin& InPadding);

	void SetSwipeBarRightClickDragAllowed(bool bIsAllowed);

	void SetStyle(const FSwipeBoxStyle* InStyle);

	void SetSwipeBarStyle(const FSwipeBarStyle* InBarStyle);

	void InvalidateStyle();

	void InvalidateSwipeBarStyle();

public:
	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual bool ComputeVolatility() const override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual FNavigationReply
	OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	                             const FFocusEvent& InFocusEvent) override;
	// End of SWidget interface

protected:
	void OnClippingChanged();

private:
	/** Builds a default Swipebar */
	TSharedPtr<SSwipeBar> ConstructSwipeBar();

	/** Constructs internal layout widgets for Swipeing vertically using the existing SwipePanel and SwipeBar. */
	void ConstructVerticalLayout();

	/** Constructs internal layout widgets for Swipeing horizontally using the existing SwipePanel and SwipeBar. */
	void ConstructHorizontalLayout();

	/** Gets the component of a vector in the direction of Swipeing based on the Orientation property. */
	FORCEINLINE float GetSwipeComponentFromVector(FVector2f Vector) const
	{
		return float(Orientation == Orient_Vertical ? Vector.Y : Vector.X);
	}

	/** Sets the component of a vector in the direction of Swipeing based on the Orientation property. */
	inline void SetSwipeComponentOnVector(FVector2f& InVector, float Value) const
	{
		if (Orientation == Orient_Vertical)
		{
			InVector.Y = Value;
		}
		else
		{
			InVector.X = Value;
		}
	}

	/** Swipe offset that the user asked for. We will clamp it before actually Swipeing there. */
	float DesiredSwipeOffset;

	/**
	 * Swipe the view by SwipeAmount given its currently AllottedGeometry.
	 *
	 * @param AllottedGeometry  The geometry allotted for this SSwipeBox by the parent
	 * @param SwipeAmount      
	 * @param InAnimateSwipe	Whether or not to animate the Swipe
	 * @return Whether or not the Swipe was fully handled
	 */
	bool SwipeBy(const FGeometry& AllottedGeometry, float LocalSwipeAmount, EAllowOverscroll OverSwipe,
	             bool InAnimateSwipe, bool InActionFromUser = false);

	/** Invoked when the user Swipe via the Swipebar */
	void SwipeBar_OnUserSwipeed(float InSwipeOffsetFraction);

	/** Does the user need a hint that they can Swipe to the start of the list? */
	FSlateColor GetStartShadowOpacity() const;

	/** Does the user need a hint that they can Swipe to the end of the list? */
	FSlateColor GetEndShadowOpacity() const;

	/** Active timer to update inertial Swipeing as needed */
	EActiveTimerReturnType UpdateInertialSwipe(double InCurrentTime, float InDeltaTime);

	/** Check whether the current state of the table warrants inertial Swipe by the specified amount */
	bool CanUseInertialSwipe(float SwipeAmount) const;

	void BeginInertialSwipeing();

	/** Padding to the SwipeBox */
	FMargin SwipeBarSlotPadding;

	union
	{
		// vertical Swipe bar is stored in horizontal box and vice versa
		SHorizontalBox::FSlot* VerticalSwipeBarSlot; // valid when Orientation == Orient_Vertical
		SVerticalBox::FSlot* HorizontalSwipeBarSlot; // valid when Orientation == Orient_Horizontal
	};

protected:
	/** Swipes or begins Swipeing a widget into view, only valid to call when we have layout geometry. */
	bool InternalSwipeDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind,
	                                     bool InAnimateSwipe = true,
	                                     EDescendantScrollDestination InDestination =
		                                     EDescendantScrollDestination::IntoView, float Padding = 0);

	/** returns widget that can receive keyboard focus or nullprt **/
	TSharedPtr<SWidget> GetKeyboardFocusableWidget(TSharedPtr<SWidget> InWidget);

	/** The panel which stacks the child slots */
	TSharedPtr<class SSwipePanel> SwipePanel;

	/** The Swipebar which controls Swipeing for the SwipeBox. */
	TSharedPtr<SSwipeBar> SwipeBar;

	/** The amount we have Swipeed this tick cycle */
	float TickSwipeDelta;

	/** Did the user start an interaction in this list? */
	TOptional<int32> bFingerOwningTouchInteraction;

	/** How much we Swipeed while the rmb has been held */
	float AmountSwipeedWhileRightMouseDown;

	/** The current deviation we've accumulated on scrol, once it passes the trigger amount, we're going to begin Swipeing. */
	float PendingSwipeTriggerAmount;

	/** Helper object to manage inertial Swipeing */
	FInertialScrollManager InertialSwipeManager;

	/** The overSwipe state management structure. */
	FOverscroll OverSwipe;

	/** The stickySwipe state management structure. */
	FStickyswipe StickySwipe;

	/** Whether to permit overSwipe on this Swipe box */
	EAllowOverscroll AllowOverSwipe;
	
	/** Whether to permit stickySwipe on this Swipe box */
	EAllowStickyswipe AllowStickySwipe;

#if WITH_EDITORONLY_DATA
	/** Padding to the SwipeBox */
	UE_DEPRECATED(5.0, "SwipeBarPadding is deprecated, Use SetSwipeBarPadding")
	FMargin SwipeBarPadding;
#endif

	/** Whether chidrend stretch mode must be interpreted to take full size of the slot */
	bool SlotStretchChildAsParentSize;

	/** Whether to back pad this Swipe box, allowing user to Swipe backward until child contents are no longer visible */
	bool BackPadSwipeing;

	/** Whether to front pad this Swipe box, allowing user to Swipe forward until child contents are no longer visible */
	bool FrontPadSwipeing;

	/**
	 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
	 * SwipeBox.  Use this if you want to ensure there's a preview of the next item the user could Swipe to.
	 */
	float NavigationSwipePadding;

	/** Sets where to Swipe a widget to when using explicit navigation or if SwipeWhenFocusChanges is enabled */
	EDescendantScrollDestination NavigationDestination;

	/** Swipe behavior when user focus is given to a child widget */
	EScrollWhenFocusChanges SwipeWhenFocusChanges;

	/**	The current position of the software cursor */
	FVector2f SoftwareCursorPosition;

	/** Fired when the user Swipes the SwipeBox */
	FOnUserSwipeed OnUserSwipeed;

	/** The Swipeing and stacking orientation. */
	EOrientation Orientation;

	/** Style resource for the SwipeBox */
	const FSwipeBoxStyle* Style;

	/** Style resource for the Swipebar */
	const FSwipeBarStyle* SwipeBarStyle;

	/** How we should handle Swipeing with the mouse wheel */
	EConsumeMouseWheel ConsumeMouseWheel;

	/** Cached geometry for use with the active timer */
	FGeometry CachedGeometry;

	/** Swipe into view request. */
	TFunction<void(FGeometry)> SwipeIntoViewRequest;

	TSharedPtr<FActiveTimerHandle> UpdateInertialSwipeHandle;

	double LastSwipeTime;

	/** Multiplier applied to each click of the Swipe wheel (applied alongside the global Swipe amount) */
	float WheelSwipeMultiplier = 1.f;

	/** Whether to animate wheel Swipeing */
	bool bAnimateWheelSwipeing : 1;

	/**	Whether the software cursor should be drawn in the viewport */
	bool bShowSoftwareCursor : 1;

	/** Whether or not the user supplied an external Swipebar to control Swipeing. */
	bool bSwipeBarIsExternal : 1;

	/** Are we actively Swipeing right now */
	bool bIsSwipeing : 1;

	/** Should the current Swipeing be animated or immediately jump to the desired Swipe offer */
	bool bAnimateSwipe : 1;

	/** If true, will Swipe to the end next Tick */
	bool bSwipeToEnd : 1;

	/** Whether the active timer to update the inertial Swipe is registered */
	bool bIsSwipeingActiveTimerRegistered : 1;

	bool bAllowsRightClickDragSwipeing : 1;

	bool bTouchPanningCapture : 1;
};

class MOBILEWIDGETSWIPE_API SSwipePanel : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SSwipePanel)
		{
			_Visibility = EVisibility::SelfHitTestInvisible;
		}

		SLATE_ARGUMENT(EOrientation, Orientation)
		SLATE_ARGUMENT(bool, SlotStretchChildAsParentSize)
		SLATE_ARGUMENT(bool, BackPadSwipeing)
		SLATE_ARGUMENT(bool, FrontPadSwipeing)

	SLATE_END_ARGS()

	SSwipePanel()
		: Children(this)
	{
	}

	UE_DEPRECATED(5.0, "Direct construction of FSlot is deprecated")
	void Construct(const FArguments& InArgs, const TArray<SSwipeBox::FSlot*>& InSlots);

	void Construct(const FArguments& InArgs, TArray<SSwipeBox::FSlot::FSlotArguments> InSlots);

public:
	EOrientation GetOrientation()
	{
		return Orientation;
	}

	void SetOrientation(EOrientation InOrientation)
	{
		Orientation = InOrientation;
	}

	virtual void
	OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	template <EOrientation Orientation, typename SlotType>
	static void ArrangeChildrenInStackOverride(EFlowDirection InLayoutFlow, const TPanelChildren<SlotType>& Children,
	                                           const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren,
	                                           float InOffset, bool InAllowShrink, bool InSlotStretchChildAsParentSize)
	{
		// Allotted space will be given to fixed-size children first.
		// Remaining space will be proportionately divided between stretch children (SizeRule_Stretch)
		// based on their stretch coefficient

		if (Children.Num() > 0)
		{
			float StretchCoefficientTotal = 0.0f;
			float FixedTotal = 0.0f;
			float StretchSizeTotal = 0.0f;

			bool bAnyChildVisible = false;
			// Compute the sum of stretch coefficients (SizeRule_Stretch) and space required by fixed-size widgets (SizeRule_Auto),
			// as well as the total desired size.
			for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
			{
				const SlotType& CurChild = Children[ChildIndex];

				if (CurChild.GetWidget()->GetVisibility() != EVisibility::Collapsed)
				{
					bAnyChildVisible = true;
					// All widgets contribute their margin to the fixed space requirement
					// UE_LOG(LogTemp, Warning, TEXT("FixedTotal: %f +=: %f"), FixedTotal, CurChild.GetPadding().template GetTotalSpaceAlong<Orientation>())
					FixedTotal += CurChild.GetPadding().template GetTotalSpaceAlong<Orientation>();


					FVector2f ChildDesiredSize = CurChild.GetWidget()->GetDesiredSize();

					// Auto-sized children contribute their desired size to the fixed space requirement
					const float ChildSize = (Orientation == Orient_Vertical)
						                        ? ChildDesiredSize.Y
						                        : ChildDesiredSize.X;

					if (CurChild.GetSizeRule() == FSizeParam::SizeRule_Stretch)
					{
						if (InSlotStretchChildAsParentSize)
						{
							const float TotalSize = (Orientation == Orient_Vertical)
												? AllottedGeometry.GetLocalSize().Y
												: AllottedGeometry.GetLocalSize().X;
							// for stretch children we save sum up the stretch coefficients
							StretchCoefficientTotal += CurChild.GetSizeValue();
							StretchSizeTotal += TotalSize;
						}
						else
						{
							// for stretch children we save sum up the stretch coefficients
							StretchCoefficientTotal += CurChild.GetSizeValue();
							StretchSizeTotal += ChildSize;
						}
					}
					else
					{
						// Clamp to the max size if it was specified
						float MaxSize = CurChild.GetMaxSize();
						FixedTotal += MaxSize > 0.0f ? FMath::Min(MaxSize, ChildSize) : ChildSize;
					}
				}
			}

			if (!bAnyChildVisible)
			{
				return;
			}

			//When shrink is not allowed, we'll ensure to use all the space desired by the stretchable widgets.
			const float MinSize = InAllowShrink ? 0.0f : StretchSizeTotal;

			// The space available for SizeRule_Stretch widgets is any space that wasn't taken up by fixed-sized widgets.
			const float NonFixedSpace = FMath::Max(MinSize, (Orientation == Orient_Vertical)
				                                                ? AllottedGeometry.GetLocalSize().Y - FixedTotal
				                                                : AllottedGeometry.GetLocalSize().X - FixedTotal);

			// UE_LOG(LogTemp, Warning, TEXT("MinSize: %f , AllottedGeometry.GetLocalSize().X: %f, FixedTotal: %f"), MinSize, AllottedGeometry.GetLocalSize().X, FixedTotal)
			float PositionSoFar = 0.0f;

			// Now that we have the total fixed-space requirement and the total stretch coefficients we can
			// arrange widgets top-to-bottom or left-to-right (depending on the orientation).
			for (TPanelChildrenConstIterator<SlotType> It(Children, Orientation, InLayoutFlow); It; ++It)
			{
				const SlotType& CurChild = *It;
				const EVisibility ChildVisibility = CurChild.GetWidget()->GetVisibility();

				// Figure out the area allocated to the child in the direction of BoxPanel
				// The area allocated to the slot is ChildSize + the associated margin.
				float ChildSize = 0.0f;
				if (ChildVisibility != EVisibility::Collapsed)
				{
					// The size of the widget depends on its size type
					if (CurChild.GetSizeRule() == FSizeParam::SizeRule_Stretch)
					{
						// UE_LOG(LogTemp, Warning, TEXT("ChildSize: %f * %f / %f"), NonFixedSpace, CurChild.GetSizeValue(), StretchCoefficientTotal)
						// if (InSlotStretchChildAsParentSize)
						// {
						// 	ChildSize = NonFixedSpace;// * CurChild.GetSizeValue();
						// }
						// else
						// {
							if (StretchCoefficientTotal > 0.0f)
							{
								// Stretch widgets get a fraction of the space remaining after all the fixed-space requirements are met
								ChildSize = NonFixedSpace * CurChild.GetSizeValue() / StretchCoefficientTotal;
							}
						// }
					}
					else
					{
						const FVector2f ChildDesiredSize = CurChild.GetWidget()->GetDesiredSize();

						// Auto-sized widgets get their desired-size value
						ChildSize = (Orientation == Orient_Vertical)
							            ? ChildDesiredSize.Y
							            : ChildDesiredSize.X;
					}

					// Clamp to the max size if it was specified
					float MaxSize = CurChild.GetMaxSize();
					if (MaxSize > 0.0f)
					{
						ChildSize = FMath::Min(MaxSize, ChildSize);
					}
				}

				const FMargin SlotPadding(LayoutPaddingWithFlow(InLayoutFlow, CurChild.GetPadding()));

				FVector2f SlotSize = (Orientation == Orient_Vertical)
					                     ? FVector2f(AllottedGeometry.GetLocalSize().X,
					                                 ChildSize +
					                                 (InSlotStretchChildAsParentSize?0: SlotPadding.template GetTotalSpaceAlong<Orient_Vertical>())
					                                 )
					                     : FVector2f(
						                     ChildSize +
						                     (InSlotStretchChildAsParentSize?0: SlotPadding.template GetTotalSpaceAlong<Orient_Horizontal>()),
						                     AllottedGeometry.GetLocalSize().Y);

				// Figure out the size and local position of the child within the slot			
				AlignmentArrangeResult XAlignmentResult = AlignChild<Orient_Horizontal>(
					InLayoutFlow, SlotSize.X, CurChild, SlotPadding);
				AlignmentArrangeResult YAlignmentResult = AlignChild<
					Orient_Vertical>(SlotSize.Y, CurChild, SlotPadding);

				const FVector2f LocalPosition = (Orientation == Orient_Vertical)
					                                ? FVector2f(XAlignmentResult.Offset,
					                                            PositionSoFar + YAlignmentResult.Offset + InOffset)
					                                : FVector2f(PositionSoFar + XAlignmentResult.Offset + InOffset,
					                                            YAlignmentResult.Offset);

				const FVector2f LocalSize = FVector2f(XAlignmentResult.Size, YAlignmentResult.Size);

				// Add the information about this child to the output list (ArrangedChildren)
				ArrangedChildren.AddWidget(ChildVisibility, AllottedGeometry.MakeChild(
					                           // The child widget being arranged
					                           CurChild.GetWidget(),
					                           // Child's local position (i.e. position within parent)
					                           LocalPosition,
					                           // Child's size
					                           LocalSize
				                           ));
				// UE_LOG(LogTemp, Warning, TEXT("LocalPosition: x: %f, y: %f"), LocalPosition.X, LocalPosition.Y)
				// UE_LOG(LogTemp, Warning, TEXT("PositionSoFar :%f + XAlignmentResult.Offset :%f + InOffset :%f"), PositionSoFar, XAlignmentResult.Offset, InOffset)
				if (ChildVisibility != EVisibility::Collapsed)
				{
					// Offset the next child by the size of the current child and any post-child (bottom/right) margin
					PositionSoFar += (Orientation == Orient_Vertical) ? SlotSize.Y : SlotSize.X;
				}
			}
		}
	}

	virtual FChildren* GetChildren() override
	{
		return &Children;
	}

	float PhysicalOffset;
	TPanelChildren<SSwipeBox::FSlot> Children;

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

private:
	EOrientation Orientation;
	bool SlotStretchChildAsParentSize;
	bool BackPadSwipeing;
	bool FrontPadSwipeing;
};
