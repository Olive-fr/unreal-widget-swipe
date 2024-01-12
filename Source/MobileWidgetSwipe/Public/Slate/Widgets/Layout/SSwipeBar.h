// Copyright Epic Games, Inc. All Rights Reserved.
 
#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Styling/SlateColor.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"
#include "Slate/Styling/SlateTypesExtension.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/SlateWidgetStyleAsset.h"

class SSwipeBarTrack;
class SSpacer;
class SImage;

DECLARE_DELEGATE_OneParam(
	FOnUserSwipeed,
	float );	/** SwipeOffset as a fraction between 0 and 1 */


class SSwipeBarTrack;

class MOBILEWIDGETSWIPE_API SSwipeBar : public SBorder
{
public:

	SLATE_BEGIN_ARGS( SSwipeBar )
		: _Style( &FAppStyle::Get().GetWidgetStyle<FSwipeBarStyle>("SwipeBar") )
		, _OnUserSwipeed()
		, _AlwaysShowSwipeBar(false)
		, _AlwaysShowSwipeBarTrack(true)
#if PLATFORM_UI_HAS_MOBILE_SCROLLBARS
		, _HideWhenNotInUse(true)
#else
		, _HideWhenNotInUse(false)
#endif
		, _PreventThrottling(false)
		, _Orientation( Orient_Vertical )
		, _DragFocusCause( EFocusCause::Mouse )
		, _Thickness()
		, _Padding( 2.0f )
		{}

		/** The style to use for this SwipeBar */
		SLATE_STYLE_ARGUMENT( FSwipeBarStyle, Style )
		SLATE_EVENT( FOnUserSwipeed, OnUserSwipeed )
		SLATE_ARGUMENT( bool, AlwaysShowSwipeBar )
		SLATE_ARGUMENT( bool, AlwaysShowSwipeBarTrack )
		SLATE_ARGUMENT( bool, HideWhenNotInUse )
		SLATE_ARGUMENT( bool, PreventThrottling )
		SLATE_ARGUMENT( EOrientation, Orientation )
		SLATE_ARGUMENT( EFocusCause, DragFocusCause )
		/** The thickness of the SwipeBar thumb */
		SLATE_ATTRIBUTE( FVector2D, Thickness )
		/** The margin around the SwipeBar */
		SLATE_ATTRIBUTE( FMargin, Padding )
	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 *
	 * @param	InArgs	The declaration data for this widget
	 */
	void Construct(const FArguments& InArgs);

	/**
	 * Set the handler to be invoked when the user Swipes.
	 *
	 * @param InHandler   Method to execute when the user Swipes the SwipeBar
	 */
	void SetOnUserSwipeed( const FOnUserSwipeed& InHandler );

	/**
	 * Set the offset and size of the track's thumb.
	 * Note that the maximum offset is 1.0-ThumbSizeFraction.
	 * If the user can view 1/3 of the items in a single page, the maximum offset will be ~0.667f
	 *
	 * @param InOffsetFraction     Offset of the thumbnail from the top as a fraction of the total available Swipe space.
	 * @param InThumbSizeFraction  Size of thumbnail as a fraction of the total available Swipe space.
	 * @param bCallOnUserSwipeed  If true, OnUserSwipeed will be called with InOffsetFraction
	 */
	virtual void SetState( float InOffsetFraction, float InThumbSizeFraction, bool bCallOnUserSwipeed = false);

	// SWidget
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	// SWidget

	/** @return true if Swipeing is possible; false if the view is big enough to fit all the content */
	bool IsNeeded() const;

	/** @return normalized distance from top */
	float DistanceFromTop() const;

	/** @return normalized distance from bottom */
	float DistanceFromBottom() const;

	/** @return normalized percentage of track covered by thumb bar */
	float ThumbSizeFraction() const;

	/** @return the SwipeBar's visibility as a product of internal rules and user-specified visibility */
	EVisibility ShouldBeVisible() const;

	/** @return True if the user is Swipeing by dragging the Swipe bar thumb. */
	bool IsSwipeing() const;

	/** @return the orientation in which the SwipeBar is Swipeing. */
	EOrientation GetOrientation() const; 

	/** Set argument Style */
	void SetStyle(const FSwipeBarStyle* InStyle);

	/** Invalidate the style */
	void InvalidateStyle();

	/** Set UserVisibility attribute */
	void SetUserVisibility(TAttribute<EVisibility> InUserVisibility) { UserVisibility = InUserVisibility; }

	/** Set DragFocusCause attribute */
	void SetDragFocusCause(EFocusCause InDragFocusCause);

	/** Set Thickness attribute */
	void SetThickness(TAttribute<FVector2D> InThickness);

	/** Set SwipeBarAlwaysVisible attribute */
	void SetSwipeBarAlwaysVisible(bool InAlwaysVisible);

	/** Set SwipeBarTrackAlwaysVisible attribute */
	void SetSwipeBarTrackAlwaysVisible(bool InAlwaysVisible);

	/** Returns True when the SwipeBar should always be shown, else False */
	bool AlwaysShowSwipeBar() const;

	/** Allows external Swipeing panels to notify the SwipeBar when Swipeing begins. */
	virtual void BeginSwipeing();

	/** Allows external Swipeing panels to notify the SwipeBar when Swipeing ends. */
	virtual void EndSwipeing();

	SSwipeBar();

protected:
	
	/** Execute the on user Swipeed delegate */
	void ExecuteOnUserSwipeed( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent );

	/** We fade the Swipe track unless it is being hovered*/
	FSlateColor GetTrackOpacity() const;

	/** We always show a subtle Swipe thumb, but highlight it extra when dragging */
	FLinearColor GetThumbOpacity() const;

	/** @return the name of an image for the SwipeBar thumb based on whether the user is dragging or hovering it. */
	const FSlateBrush* GetDragThumbImage() const;

	/** The SwipeBar's visibility as specified by the user. Will be compounded with internal visibility rules. */
	TAttribute<EVisibility> UserVisibility;

	TSharedPtr<SImage> TopImage;
	TSharedPtr<SImage> BottomImage;
	TSharedPtr<SBorder> DragThumb;
	TSharedPtr<SSpacer> ThicknessSpacer;
	bool bDraggingThumb;
	TSharedPtr<SSwipeBarTrack> Track;
	FOnUserSwipeed OnUserSwipeed;
	float DragGrabOffset;
	EOrientation Orientation;
	bool bAlwaysShowSwipeBar;
	bool bAlwaysShowSwipeBarTrack;
	EFocusCause DragFocusCause;
	bool bHideWhenNotInUse;
	/*
	 * Holds whether or not to prevent throttling during mouse capture
	 * When true, the viewport will be updated with every single change to the value during dragging
	 */
	bool bPreventThrottling;
	bool bIsSwipeing;
	double LastInteractionTime;

	/** Image to use when the SwipeBar thumb is in its normal state */
	const FSlateBrush* NormalThumbImage;
	/** Image to use when the SwipeBar thumb is in its hovered state */
	const FSlateBrush* HoveredThumbImage;
	/** Image to use when the SwipeBar thumb is in its dragged state */
	const FSlateBrush* DraggedThumbImage;
	/** Background brush */
	const FSlateBrush* BackgroundBrush;
	/** Top brush */
	const FSlateBrush* TopBrush;
	/** Bottom brush */
	const FSlateBrush* BottomBrush;
};
