// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Framework/Layout/Stickyswipe.h"

#include "Kismet/KismetMathLibrary.h"

float FStickyswipe::PageWidth(500.0f);
float FStickyswipe::Looseness(50.0f);
float FStickyswipe::OvershootLooseMax(100.0f);
float FStickyswipe::OvershootBounceRate(1500.0f);

FStickyswipe::FStickyswipe()
	: StickyswipeAmount(0.0f)
	  , CurrentPageId(0)
, Easing(EEasingFunc::CircularInOut)
, BlendExp(2.0)
, Speed(1.0)
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
		TargetPoint = GetPageSwipeValue(AllottedGeometry, CurrentPageId);
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
void FStickyswipe::UpdateStickyswipe(const TSharedRef<SPanel> Panel, const FGeometry& AllottedGeometry,
                                     float InDeltaTime)
{
	const auto Children = Panel->GetChildren();
	
	if (Children->Num() < 1)
	{
		return ;
	}
	TSharedRef<SWidget> Widget = Children->GetChildAt(CurrentPageId);

	if (UserSwipeEnd)
	{
		const int ChildrenNb = Panel->GetChildren()->Num();
		float SizeX = AllottedGeometry.GetLocalSize().X; // FIXME it's not only horizontal
		// If the user released the interraction, compute the page swipe logic
		if (FMath::Abs(SwipeAmountSinceUserMove) > SizeX * 0.15) // FIXME 0.15 to the percentage 
		{
			float NewStickyswipeAmount = GetStickyswipe(AllottedGeometry);
			CurrentMode = EStickyMode::StickyToTarget;
			// The user traveled 15% of the geometry, the movement is validated
			int NewCurrentPageId = SwipeAmountSinceUserMove > 0 ? CurrentPageId + 1 : CurrentPageId - 1;
			NewCurrentPageId = FMath::Clamp(NewCurrentPageId, 0, ChildrenNb - 1); //FIXME 10 to the max page

			CurrentPageId = NewCurrentPageId;
			Widget = Children->GetChildAt(CurrentPageId);
			StickyswipeAmount = NewStickyswipeAmount;
			StickyswipeAmountStart = StickyswipeAmount;
			AlphaTarget = 0;
			// TargetPoint = StickyswipeAmount + Widget->GetCachedGeometry().Position.X / AllottedGeometry.Scale;
			TargetPoint = GetPageSwipeValue(AllottedGeometry, CurrentPageId);
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
			//TargetPoint = StickyswipeAmount + Widget->GetCachedGeometry().Position.X / AllottedGeometry.Scale;
			
			// TargetPoint = GetPageSwipeValue(AllottedGeometry, CurrentPageId);
			// StickyswipeAmount = FMath::FInterpTo(StickyswipeAmount, TargetPoint, InDeltaTime, 3.0);
			// if (FMath::IsNearlyEqual(StickyswipeAmount, TargetPoint, 0.01))
			// {
			// 	CurrentMode = EStickyMode::StickyToOrigin;
			// }
			//TargetPoint = GetPageSwipeValue(AllottedGeometry, CurrentPageId);
			AlphaTarget += InDeltaTime * Speed;
			StickyswipeAmount = UKismetMathLibrary::Ease(StickyswipeAmountStart, TargetPoint, AlphaTarget, Easing, BlendExp);
			if (AlphaTarget >= 1.0)
			{
				CurrentMode = EStickyMode::StickyToOrigin;
			}
			break;
		}
	}
	
	MaxValue = GetPageSwipeValue(AllottedGeometry, Children->Num() - 1);
	StickyswipeAmount = FMath::Clamp(StickyswipeAmount, 0, MaxValue);


	if (UserSwipeEnd)
	{
		UserSwipeEnd = false;
		SwipeAmountSinceUserMove = 0;
	}
	UserSwiped = false;
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

float FStickyswipe::GetPageSwipeValue(const FGeometry& AllottedGeometry, int PageId)
{
	return AllottedGeometry.Size.X * PageId - PageId;
}
