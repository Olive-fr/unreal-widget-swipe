// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Framework/Layout/Stickyswipe.h"

float FStickyswipe::PageWidth(500.0f);
float FStickyswipe::Looseness(50.0f);
float FStickyswipe::OvershootLooseMax(100.0f);
float FStickyswipe::OvershootBounceRate(1500.0f);

FStickyswipe::FStickyswipe()
	: StickyswipeAmount(0.0f)
	  , CurrentPageId(0)
{
}

// Code used before tick, it's called every tick while the user interract
float FStickyswipe::SwipeBy(const FGeometry& AllottedGeometry, float LocalDeltaScroll, bool InActionFromUser)
{
	UserSwiped = true;
	const float ScreenDeltaSwipe = LocalDeltaScroll / AllottedGeometry.Scale;

	const float ValueBeforeDeltaApplied = StickyswipeAmount;
	StickyswipeAmount += ScreenDeltaSwipe;
	
	// Don't allow an interaction to change from positive <-> negative overscroll
	const bool bCrossedOverscrollBoundary = FMath::Sign(ValueBeforeDeltaApplied) != FMath::Sign(StickyswipeAmount);
	/*if (bCrossedOverscrollBoundary && ValueBeforeDeltaApplied != 0.f)
	{
		StickyswipeAmount = 0.f;
	}*/

	SwipeAmountSinceUserMove += LocalDeltaScroll;

	if (InActionFromUser)
	{
		StickyToOriginInProgress = false;
		CurrentMode = EStickyMode::StickyToOrigin;
		TargetPoint = GetPageSwipeValue(AllottedGeometry, CurrentPageId);
	}
	// UE_LOG(LogTemp, Warning, TEXT("SwipeBy StickyswipeAmount: %f, SwipeAmountTotal: %f"), StickyswipeAmount, SwipeAmountTotal)

	return ValueBeforeDeltaApplied - StickyswipeAmount;
}

// Code used in tick to get the effective offset in the component to display
float FStickyswipe::GetStickyswipe(const FGeometry& AllottedGeometry) const
{
	// UE_LOG(LogTemp, Warning, TEXT("FStickyswipe::GetStickyswipe"))

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

			float Shift = AdaptedLooseness * FMath::Loge(
				FMath::Abs(SwipeAmountSinceUserMove) + AdaptedLooseness);
			// Scale the log of the current overscroll and shift the origin so that the overscroll the resistance is continuous.
			const float AbsElasticOverscroll = Shift - OriginShift; // + TargetPoint;

			const float AbsOverscroll = FMath::Sign(SwipeAmountSinceUserMove) * AbsElasticOverscroll;
			EffectiveSwipe = AbsOverscroll * AllottedGeometry.Scale + TargetPoint;
			UE_LOG(LogTemp, Warning,
			       TEXT("GetStickyswipe:StickyToOrigin EffectiveSwipe: %f, StickyswipeAmount: %f, AbsElasticOverscroll: %f, OriginShift: %f, Shift: %f, TargetPoint: %f"),
			       EffectiveSwipe, StickyswipeAmount, AbsElasticOverscroll, OriginShift, Shift, TargetPoint)
		}
		break;
	case EStickyMode::StickyToTarget:
		{
			EffectiveSwipe = StickyswipeAmount; // * AllottedGeometry.Scale;
			UE_LOG(LogTemp, Warning, TEXT("GetStickyswipe:StickyToTarget, EffectiveSwipe: %f, currentPage: %d"), EffectiveSwipe, CurrentPageId)
		}
		break;
	}
	
	EffectiveSwipe = FMath::Clamp(EffectiveSwipe, 0, MaxValue);
	return EffectiveSwipe;
}

// Code used in tick after the GetStickyswipe to update values if an animation is present
void FStickyswipe::UpdateStickyswipe(const TSharedRef<SPanel> Panel, const FGeometry& AllottedGeometry,
                                     float InDeltaTime)
{
	auto Children = Panel->GetChildren();
	auto Child0 = Children->GetChildAt(0);
	auto Child1 = Children->GetChildAt(1);

	if (UserSwipeEnd)
	{
		const int ChildrenNb = Panel->GetChildren()->Num();
		float SizeX = AllottedGeometry.GetLocalSize().X; // FIXME it's not only horizontal
		// If the user released the interraction, compute the page swipe logic
		UE_LOG(LogTemp, Warning, TEXT("|StickyswipeAmount:%f| %f > Size:%f * 0.25: %s"),
		       FMath::Abs(SwipeAmountSinceUserMove),
		       SwipeAmountSinceUserMove, SizeX,
		       FMath::Abs(SwipeAmountSinceUserMove) > SizeX * 0.25?TEXT("true"):TEXT("false"))
		if (FMath::Abs(SwipeAmountSinceUserMove) > SizeX * 0.15) // FIXME 0.25 to the percentage 
		{
			float NewStickyswipeAmount = GetStickyswipe(AllottedGeometry);
			CurrentMode = EStickyMode::StickyToTarget;
			// The user traveled 25% of the geometry, the movement is validated
			int NewCurrentPageId = SwipeAmountSinceUserMove > 0 ? CurrentPageId + 1 : CurrentPageId - 1;
			NewCurrentPageId = FMath::Clamp(NewCurrentPageId, 0, ChildrenNb - 1); //FIXME 10 to the max page

			if (CurrentPageId != NewCurrentPageId)
			{
				int SwipeAmount = CurrentPageId - NewCurrentPageId;
			}

			CurrentPageId = NewCurrentPageId;
			TSharedRef<SWidget> Widget = Children->GetChildAt(CurrentPageId);
			StickyswipeAmount = NewStickyswipeAmount;
			TargetPoint = StickyswipeAmount + Widget->GetCachedGeometry().Position.X / AllottedGeometry.Scale;
			UE_LOG(LogTemp, Warning, TEXT("SWITCH TO TARGET and RESET SwipeAmountTotal"))
		}
	}

	TSharedRef<SWidget> Widget = Children->GetChildAt(CurrentPageId);
	TSharedRef<SWidget> LastWidget = Children->GetChildAt(Children->Num() - 1);

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
				// StickyswipeAmount = TargetPoint;

				const float PullForce = FMath::Abs(SwipeAmountSinceUserMove) + 1.0f;
				const float EasedDelta = FStickyswipe::OvershootBounceRate * InDeltaTime * FMath::Max(
					1.0f, PullForce / FStickyswipe::OvershootLooseMax);

				if (FMath::IsNearlyEqual(StickyswipeAmount, TargetPoint, 0.01))
				{
					StickyToOriginInProgress = false;
					UE_LOG(LogTemp, Warning, TEXT("FMath::IsNearlyEqual(%f, %f, 0.01) true"), StickyswipeAmount,
					       TargetPoint)
				}

				if (SwipeAmountSinceUserMove > 0)
				{
					// StickyswipeAmount = FMath::Max(0.0f, StickyswipeAmount - EasedDelta);
					StickyswipeAmount = StickyswipeAmount - EasedDelta;
				}
				else
				{
					// StickyswipeAmount = FMath::Min(0.0f, StickyswipeAmount + EasedDelta);
					StickyswipeAmount = StickyswipeAmount + EasedDelta;
				}

				UE_LOG(LogTemp, Warning, TEXT("TO ORIGIN: CurrentPageId:%d, New: %f, EasedDelta: %f, TargetPoint: %f"),
				       CurrentPageId,
				       /*OldStickyswipeAmount,*/ StickyswipeAmount, EasedDelta, TargetPoint)
			}

			break;
		}
	case EStickyMode::StickyToTarget:
		{
			TargetPoint =
				// FMath::Clamp(
				StickyswipeAmount + Widget->GetCachedGeometry().Position.X / AllottedGeometry.Scale;
				// , 0, StickyswipeAmount + LastWidget->GetCachedGeometry().Position.X / AllottedGeometry.Scale);
			StickyswipeAmount = FMath::FInterpTo(StickyswipeAmount, TargetPoint, InDeltaTime, 3.0);
			UE_LOG(LogTemp, Warning,
			       TEXT("TO TARGET: CurrentPageId:%d, Old: %f, StickyswipeAmount: %f, TargetPoint: %f"),
			       CurrentPageId, Widget->GetCachedGeometry().Position.X, StickyswipeAmount, TargetPoint)
			if (FMath::IsNearlyEqual(StickyswipeAmount, TargetPoint, 0.01))
			{
				CurrentMode = EStickyMode::StickyToOrigin;
				UE_LOG(LogTemp, Warning, TEXT("SWITCH TO ORIGIN"))
			}
			break;
		}
	}
	
	MaxValue = GetPageSwipeValue(AllottedGeometry, Children->Num() - 1);//.Size.X * (Children->Num() - 1) - Children->Num() + 1;//StickyswipeAmount + Widget->GetCachedGeometry().Position.X / AllottedGeometry.Scale;
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
	// UE_LOG(LogTemp, Warning, TEXT("FStickyswipe::ShouldApplyStickyswipe"))
	// const bool bShouldApplyOverscroll =
	// 	// We can scroll past the edge of the list only if we are at the edge
	// 	( bIsAtStartOfList && ScrollDelta < 0 ) || ( bIsAtEndOfList && ScrollDelta > 0 ) ||
	// 	// ... or if we are already past the edge and are scrolling in the opposite direction.
	// 	( OverscrollAmount > 0 && ScrollDelta < 0 ) || ( OverscrollAmount < 0 && ScrollDelta > 0 );
	//
	// return bShouldApplyOverscroll;
	return true;
}

void FStickyswipe::ResetStickyswipe()
{
	// UE_LOG(LogTemp, Warning, TEXT("FStickyswipe::ResetStickyswipe"))
	StickyswipeAmount = 0.0f;
}

float FStickyswipe::GetPageSwipeValue(const FGeometry& AllottedGeometry, int PageId)
{
	return AllottedGeometry.Size.X * PageId - PageId;
}
