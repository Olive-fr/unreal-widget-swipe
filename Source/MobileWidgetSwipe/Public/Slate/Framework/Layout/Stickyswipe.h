// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Platform.h"
#include "Widgets/SPanel.h"
#include "Layout/Geometry.h"

struct FGeometry;
//
enum class EAllowStickyswipe : uint8
{
	Yes,
	No
};
enum class EStickyMode : uint8
{
	StickyToOrigin,
	StickyToTarget
};

/**
 * Handles overscroll management.
 */
struct MOBILEWIDGETSWIPE_API FStickyswipe
{
public:

	static float PageWidth;
	/** The amount to scale the logarithm by to make it more loose */
	static float Looseness;
	/** The "max" used to perform the interpolation snap back, and make it faster the further away it is. */
	static float OvershootLooseMax;
	/** The bounce back rate when the overscroll stops. */
	static float OvershootBounceRate;

	FStickyswipe();

	/** @return The Amount actually swiped */
	float SwipeBy(const FGeometry& AllottedGeometry, float LocalDeltaScroll, bool InActionFromUser);

	/** How far the widget is uncentered. */
	float GetStickyswipe(const FGeometry& AllottedGeometry) const;

	/** Ticks the stickyscroll manager so it can animate. */
	void UpdateStickyswipe(const TSharedRef<SPanel> Panel, const FGeometry& AllottedGeometry, float InDeltaTime);

	void OnUserReleaseInterraction();

	/**
	 * Should ScrollDelta be applied to stickyscroll or to regular item scrolling.
	 *
	 * @param bIsAtStartOfList  Are we at the very beginning of the list (i.e. showing the first item at the top of the view)?
	 * @param bIsAtEndOfList    Are we showing the last item on the screen completely?
	 * @param ScrollDelta       How much the user is trying to scroll in Slate Units.
	 *
	 * @return true if the user's scrolling should be applied toward overscroll.
	 */
	bool ShouldApplyStickyswipe() const;
	
	/** Resets the stickyscroll amout. */
	void ResetStickyswipe();
private:

	float GetPageSwipeValue(const FGeometry& AllottedGeometry, const int PageId);
	/** How much we've over-scrolled above/below the beginning/end of the list, stored in log form */
	float StickyswipeAmount;
	float SwipeAmountSinceUserMove;

	float TargetPoint;
	float MaxValue;
	int CurrentPageId;

	EStickyMode CurrentMode = EStickyMode::StickyToOrigin;

	bool UserSwipeEnd = false;
	bool UserSwiped = false;
	bool StickyToOriginInProgress = false;
};
