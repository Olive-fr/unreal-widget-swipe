// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Framework/Layout/Stickyswipe.h"

#include "Kismet/KismetMathLibrary.h"

float FStickyswipe::OvershootLooseMax(100.0f);
float FStickyswipe::OvershootBounceRate(1500.0f);

FStickyswipe::FStickyswipe()
	: Easing(EEasingFunc::CircularInOut),
	  BlendExp(2.0),
	  Speed(1.0),
	  CurrentPageId(0),
	  ScreenPercentDistanceUserChangePage(0.15),
	  Looseness(200),
	  Orientation(),
	  LastAllottedGeometry(nullptr),
	  StickyswipeAmount(0.0f),
	  StickyswipeAmountStart(0),
	  AlphaTarget(0),
	  SwipeAmountSinceUserMove(0),
	  TargetPoint(0),
	  MaxValue(0)
{
}

// Code used before tick, it's called every tick while the user interract
float FStickyswipe::SwipeBy(const FGeometry& AllottedGeometry, float LocalDeltaScroll, bool InActionFromUser)
{
	UserSwiped = true;
	const float ScreenDeltaSwipe = LocalDeltaScroll / AllottedGeometry.Scale;

	const float ValueBeforeDeltaApplied = StickyswipeAmount;
	StickyswipeAmount += ScreenDeltaSwipe;

	SwipeAmountSinceUserMove += LocalDeltaScroll;

	if (InActionFromUser)
	{
		StickyToOriginInProgress = false;
		CurrentMode = EStickyMode::StickyToOrigin;
		TargetPoint = GetPageSwipeValue(&AllottedGeometry, CurrentPageId);
	}

	return ValueBeforeDeltaApplied - StickyswipeAmount;
}

// Code used in tick to get the effective offset in the component to display
float FStickyswipe::GetStickyswipe(const FGeometry& AllottedGeometry) const
{
	float EffectiveSwipe;

	switch (CurrentMode)
	{
	case EStickyMode::StickyToOrigin:
		{
			const float AdaptedLooseness = FStickyswipe::Looseness;
			// We apply the resistance when observers attempt to read the overscroll.
			// First we calculate the origin shift for the looseness factor, by default,
			// straight up Log would be too tight, so we scale the result, the problem is that shifts
			// the inflection point of the log function, so we subtract the shift amount to correct it.
			const float OriginShift = AdaptedLooseness * FMath::Loge(AdaptedLooseness);

			const float Shift = AdaptedLooseness * FMath::Loge(
				FMath::Abs(SwipeAmountSinceUserMove) + AdaptedLooseness);
			// Scale the log of the current overscroll and shift the origin so that the overscroll the resistance is continuous.
			const float AbsElasticOverscroll = Shift - OriginShift;

			const float AbsOverscroll = FMath::Sign(SwipeAmountSinceUserMove) * AbsElasticOverscroll;
			EffectiveSwipe = AbsOverscroll * AllottedGeometry.Scale + TargetPoint;
		}
		break;
	case EStickyMode::StickyToTarget:
		{
			EffectiveSwipe = StickyswipeAmount;
		}
		break;
	default:
		EffectiveSwipe = 0;
		break;
	}

	EffectiveSwipe = FMath::Clamp(EffectiveSwipe, 0, MaxValue);
	return EffectiveSwipe;
}

// Code used in tick after the GetStickyswipe to update values if an animation is present
bool FStickyswipe::UpdateStickyswipe(const TSharedRef<SPanel> Panel, const FGeometry& AllottedGeometry,
                                     float InDeltaTime)
{
	bool SwipeValidated = false;
	const auto Children = Panel->GetChildren();
	LastAllottedGeometry = &AllottedGeometry;

	if (Children->Num() < 1)
	{
		return false;
	}

	LastChildId = Children->Num() - 1;

	if (UserSwipeEnd || FunctionSwiped)
	{
		float Size;
		switch (Orientation)
		{
		case Orient_Horizontal: Size = AllottedGeometry.GetLocalSize().X;
			break;
		case Orient_Vertical: Size = AllottedGeometry.GetLocalSize().Y;
			break;
		default: Size = 0;
		}

		// If the user released the interraction, compute the page swipe logic
		if (FunctionSwiped)
		{
			TSharedRef<SWidget> Widget = Children->GetChildAt(CurrentPageId);
			if (FunctionSwipedWithAnimation)
			{
				CurrentMode = EStickyMode::StickyToTarget;
				float NewStickyswipeAmount = GetStickyswipe(AllottedGeometry);
				TargetPoint = GetPageSwipeValue(&AllottedGeometry, CurrentPageId);

				StickyswipeAmount = NewStickyswipeAmount;
				StickyswipeAmountStart = StickyswipeAmount;
				AlphaTarget = 0;
			}
			else
			{
				CurrentMode = EStickyMode::StickyToOrigin;
				TargetPoint = GetPageSwipeValue(&AllottedGeometry, CurrentPageId);
				StickyswipeAmount = TargetPoint;
			}
			if (FunctionSwipedThrowEvent)
			{
				SwipeValidated = true;
			}
		} else if (FMath::Abs(SwipeAmountSinceUserMove) > Size * ScreenPercentDistanceUserChangePage)
		{
			float NewStickyswipeAmount = GetStickyswipe(AllottedGeometry);
			CurrentMode = EStickyMode::StickyToTarget;

			int NewCurrentPageId = SwipeAmountSinceUserMove > 0 ? CurrentPageId + 1 : CurrentPageId - 1;
			NewCurrentPageId = FMath::Clamp(NewCurrentPageId, 0, LastChildId);
			CurrentPageId = NewCurrentPageId;
			
			TSharedRef<SWidget> Widget = Children->GetChildAt(CurrentPageId);
			StickyswipeAmount = NewStickyswipeAmount;
			StickyswipeAmountStart = StickyswipeAmount;
			AlphaTarget = 0;

			TargetPoint = GetPageSwipeValue(&AllottedGeometry, CurrentPageId);
			SwipeValidated = true;
		}
	}


	switch (CurrentMode)
	{
	case EStickyMode::StickyToOrigin:
		{
			if (UserSwipeEnd)
			{
				StickyToOriginInProgress = true;
			}
			if (StickyToOriginInProgress)
			{
				const float PullForce = FMath::Abs(SwipeAmountSinceUserMove) + 1.0f;
				const float EasedDelta = FStickyswipe::OvershootBounceRate * InDeltaTime * FMath::Max(
					1.0f, PullForce / FStickyswipe::OvershootLooseMax);

				if (FMath::IsNearlyEqual(StickyswipeAmount, TargetPoint, 0.01))
				{
					StickyToOriginInProgress = false;
				}

				if (SwipeAmountSinceUserMove > 0)
				{
					StickyswipeAmount = StickyswipeAmount - EasedDelta;
				}
				else
				{
					StickyswipeAmount = StickyswipeAmount + EasedDelta;
				}
			}

			break;
		}
	case EStickyMode::StickyToTarget:
		{
			AlphaTarget += InDeltaTime * Speed;
			StickyswipeAmount = UKismetMathLibrary::Ease(StickyswipeAmountStart, TargetPoint, AlphaTarget, Easing,
			                                             BlendExp);
			if (AlphaTarget >= 1.0)
			{
				CurrentMode = EStickyMode::StickyToOrigin;
			}
			break;
		}
	}

	MaxValue = GetPageSwipeValue(&AllottedGeometry, LastChildId);
	StickyswipeAmount = FMath::Clamp(StickyswipeAmount, 0, MaxValue);


	if (UserSwipeEnd)
	{
		UserSwipeEnd = false;
		SwipeAmountSinceUserMove = 0;
	}
	FunctionSwiped = false;
	FunctionSwipedWithAnimation = false;
	FunctionSwipedThrowEvent = false;
	UserSwiped = false;

	return SwipeValidated;
}

void FStickyswipe::OnUserReleaseInterraction()
{
	UserSwipeEnd = true;
}

bool FStickyswipe::ShouldApplyStickyswipe() const
{
	return true;
}

void FStickyswipe::ResetStickyswipe()
{
	StickyswipeAmount = 0.0f;
}

float FStickyswipe::GetPageSwipeValue(const FGeometry* AllottedGeometry, int PageId) const
{
	switch (Orientation)
	{
	case Orient_Horizontal: return AllottedGeometry->Size.X * PageId - PageId;
	case Orient_Vertical: return AllottedGeometry->Size.Y * PageId - PageId;
	default: return 0;
	}
}

void FStickyswipe::SetCurrentPage(int NewPage, bool ThrowEvent, bool PlayAnimation)
{
	this->CurrentPageId = NewPage;
	FunctionSwiped = true;
	FunctionSwipedWithAnimation = PlayAnimation;
	FunctionSwipedThrowEvent = ThrowEvent;
}

float FStickyswipe::GetDistanceFromEnd() const
{
	if (LastAllottedGeometry && LastChildId > -1)
	{
		return GetPageSwipeValue(LastAllottedGeometry, LastChildId) - StickyswipeAmount;
	}
	return 1000000000;
}

float FStickyswipe::GetScreenPercentDistanceUserChangePage() const
{
	return ScreenPercentDistanceUserChangePage;
}

void FStickyswipe::SetScreenPercentDistanceUserChangePage(float InScreenPercentDistanceUserChangePage)
{
	this->ScreenPercentDistanceUserChangePage = InScreenPercentDistanceUserChangePage;
}

float FStickyswipe::GetLooseness() const
{
	return Looseness;
}

void FStickyswipe::SetLooseness(float InLooseness)
{
	this->Looseness = InLooseness;
}
