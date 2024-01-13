// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
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
	bool UpdateStickyswipe(const TSharedRef<SPanel> Panel, const FGeometry& AllottedGeometry, float InDeltaTime);

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

	EEasingFunc::Type GetEasing() const
	{
		return Easing;
	}

	void SetEasing(EEasingFunc::Type NewEasing)
	{
		this->Easing = NewEasing;
	}

	float GetBlendExp() const
	{
		return BlendExp;
	}

	void SetBlendExp(float NewBlendExp)
	{
		this->BlendExp = NewBlendExp;
	}

	float GetSpeed() const
	{
		return Speed;
	}

	void SetSpeed(float NewSpeed)
	{
		this->Speed = NewSpeed;
	}

	int GetCurrentPage() const
	{
		return CurrentPageId;
	}

	void SetCurrentPage(int NewPage, bool ThrowEvent, bool PlayAnimation);
	
	EOrientation GetOrientation() const
	{
		return Orientation;
	}

	void SetOrientation(EOrientation NewOrientation)
	{
		this->Orientation = NewOrientation;
	}
	
	float GetDistanceFromEnd() const;

	float GetScreenPercentDistanceUserChangePage() const;
	void SetScreenPercentDistanceUserChangePage(float InScreenPercentDistanceUserChangePage);
	float GetLooseness() const;
	void SetLooseness(float InLooseness);

private:
	float GetPageSwipeValue(const FGeometry* AllottedGeometry, const int PageId) const;
	/** How much we've over-scrolled above/below the beginning/end of the list, stored in log form */
	
	EEasingFunc::Type Easing;
	float BlendExp;
	float Speed;
	int CurrentPageId;
	float ScreenPercentDistanceUserChangePage;
	/** The amount to scale the logarithm by to make it more loose */
	float Looseness;
	EOrientation Orientation;

	const FGeometry* LastAllottedGeometry;
	int32 LastChildId = -1;
	
	float StickyswipeAmount;
	float StickyswipeAmountStart;
	float AlphaTarget;
	float SwipeAmountSinceUserMove;

	float TargetPoint;
	float MaxValue;
	EStickyMode CurrentMode = EStickyMode::StickyToOrigin;

	bool UserSwipeEnd = false;
	bool UserSwiped = false;
	bool FunctionSwiped = false;
	bool FunctionSwipedWithAnimation = false;
	bool FunctionSwipedThrowEvent = false;
	bool StickyToOriginInProgress = false;
};
