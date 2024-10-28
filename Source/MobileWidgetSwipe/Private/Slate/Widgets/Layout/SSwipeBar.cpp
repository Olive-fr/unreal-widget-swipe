// Copyright Epic Games, Inc. All Rights Reserved.
 
#include "Slate/Widgets/Layout/SSwipeBar.h"
#include "Slate/Widgets/Layout/SSwipeBarTrack.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/Application/SlateApplication.h"

void SSwipeBar::Construct(const FArguments& InArgs)
{
	OnUserSwipeed = InArgs._OnUserSwipeed;
	Orientation = InArgs._Orientation;
	DragFocusCause = InArgs._DragFocusCause;
	UserVisibility = InArgs._Visibility;

	check(InArgs._Style);
	SetStyle(InArgs._Style);

	const TAttribute<FVector2D> Thickness = InArgs._Thickness.IsSet() ? InArgs._Thickness : FVector2D(InArgs._Style->Thickness, InArgs._Style->Thickness);

	EHorizontalAlignment HorizontalAlignment = Orientation == Orient_Vertical ? HAlign_Center : HAlign_Fill;
	EVerticalAlignment VerticalAlignment = Orientation == Orient_Vertical ? VAlign_Fill : VAlign_Center;

	bHideWhenNotInUse = InArgs._HideWhenNotInUse;
	bPreventThrottling = InArgs._PreventThrottling;
	bIsSwipeing = false;
	LastInteractionTime = 0;

	SBorder::Construct( SBorder::FArguments()
		.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
		.Padding(InArgs._Padding)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				SNew(SBorder)
				.BorderImage(BackgroundBrush)
				.HAlign(HorizontalAlignment)
				.VAlign(VerticalAlignment)
				.Padding(0.f)
				[
					SAssignNew(Track, SSwipeBarTrack)
					.Orientation(InArgs._Orientation)
					.TopSlot()
					[
						SNew(SBox)
						.HAlign(HorizontalAlignment)
						.VAlign(VerticalAlignment)
						[
							SAssignNew(TopImage, SImage)
							.Image(TopBrush)
						]
					]
					.ThumbSlot()
					[
						SAssignNew(DragThumb, SBorder)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(0.f)
						[
							SAssignNew(ThicknessSpacer, SSpacer)
							.Size(Thickness)
						]
					]
					.BottomSlot()
					[
						SNew(SBox)
						.HAlign(HorizontalAlignment)
						.VAlign(VerticalAlignment)
						[
							SAssignNew(BottomImage, SImage)
							.Image(BottomBrush)
						]
					]
				]
			]
		]
	);

	SetEnabled(TAttribute<bool>( Track.ToSharedRef(), &SSwipeBarTrack::IsNeeded ));
	SetSwipeBarAlwaysVisible(InArgs._AlwaysShowSwipeBar);
	bAlwaysShowSwipeBarTrack = InArgs._AlwaysShowSwipeBarTrack;
}

void SSwipeBar::SetOnUserSwipeed( const FOnUserSwipeed& InHandler )
{
	OnUserSwipeed = InHandler;
}

void SSwipeBar::SetState( float InOffsetFraction, float InThumbSizeFraction, bool bCallOnUserSwipeed )
{
	if ( Track->DistanceFromTop() != InOffsetFraction || Track->GetThumbSizeFraction() != InThumbSizeFraction )
	{
		// Note that the maximum offset depends on how many items fit per screen
		// It is 1.0f-InThumbSizeFraction.
		Track->SetSizes(InOffsetFraction, InThumbSizeFraction);
		GetVisibilityAttribute().UpdateValue();

		LastInteractionTime = FSlateApplication::Get().GetCurrentTime();
		if (bCallOnUserSwipeed)
		{
			OnUserSwipeed.ExecuteIfBound(InOffsetFraction);
		}
	}
}

void SSwipeBar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBorder::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	DragThumb->SetBorderImage(GetDragThumbImage());
	DragThumb->SetColorAndOpacity(GetThumbOpacity());

	TopImage->SetColorAndOpacity(GetTrackOpacity());
	BottomImage->SetColorAndOpacity(GetTrackOpacity());
}

FReply SSwipeBar::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsNeeded())
	{
		FGeometry ThumbGeometry = FindChildGeometry(MyGeometry, DragThumb.ToSharedRef());

		if (DragThumb->IsDirectlyHovered())
		{
			// Clicking on the Swipebar drag thumb
			if( Orientation == Orient_Horizontal )
			{
				DragGrabOffset = ThumbGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X;
			}
			else
			{
				DragGrabOffset = ThumbGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).Y;
			}

			bDraggingThumb = true;
		}
		else if (OnUserSwipeed.IsBound())
		{
			// Clicking in the non drag thumb area of the Swipebar
			DragGrabOffset = Orientation == Orient_Horizontal ? (ThumbGeometry.GetLocalSize().X * 0.5f) : (ThumbGeometry.GetLocalSize().Y * 0.5f);
			bDraggingThumb = true;

			ExecuteOnUserSwipeed( MyGeometry, MouseEvent );
		}
	}

	if( bDraggingThumb )
	{
		FReply ReturnReply = FReply::Handled().CaptureMouse(AsShared()).SetUserFocus(AsShared(), DragFocusCause);

		if (bPreventThrottling)
		{
			ReturnReply.PreventThrottling();
		}

		return ReturnReply;
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SSwipeBar::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
	{
		bDraggingThumb = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SSwipeBar::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if ( this->HasMouseCapture() )
	{
		if (!MouseEvent.GetCursorDelta().IsZero())
		{
			if (OnUserSwipeed.IsBound())
			{
				ExecuteOnUserSwipeed(MyGeometry, MouseEvent);
			}
			return FReply::Handled();
		}
	}
	
	return FReply::Unhandled();
}

void SSwipeBar::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SBorder::OnMouseEnter(MyGeometry, MouseEvent);
	LastInteractionTime = FSlateApplication::Get().GetCurrentTime();
}

void SSwipeBar::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SBorder::OnMouseLeave(MouseEvent);
	LastInteractionTime = FSlateApplication::Get().GetCurrentTime();
}

void SSwipeBar::ExecuteOnUserSwipeed( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	const int32 AxisId = (Orientation == Orient_Horizontal) ? 0 : 1;
	const FGeometry TrackGeometry = FindChildGeometry( MyGeometry, Track.ToSharedRef() );
	const float UnclampedOffsetInTrack = TrackGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() )[ AxisId ] - DragGrabOffset;
	const float TrackSizeBiasedByMinThumbSize = TrackGeometry.GetLocalSize()[ AxisId ] - Track->GetMinThumbSize();
	const float ThumbOffsetInTrack = FMath::Clamp( UnclampedOffsetInTrack, 0.0f, TrackSizeBiasedByMinThumbSize );
	const float ThumbOffset = ThumbOffsetInTrack / TrackSizeBiasedByMinThumbSize;
	OnUserSwipeed.ExecuteIfBound( ThumbOffset );
}

bool SSwipeBar::IsNeeded() const
{
	return Track->IsNeeded();
}

float SSwipeBar::DistanceFromTop() const
{
	return Track->DistanceFromTop();
}

float SSwipeBar::DistanceFromBottom() const
{
	return Track->DistanceFromBottom();
}

float SSwipeBar::ThumbSizeFraction() const
{
	return Track->GetThumbSizeFraction();
}

SSwipeBar::SSwipeBar()
	: bDraggingThumb(false),
	  DragGrabOffset(0.0f),
	  Orientation(),
	  bAlwaysShowSwipeBar(false),
	  bAlwaysShowSwipeBarTrack(false),
	  DragFocusCause(),
	  bHideWhenNotInUse(false),
	  bPreventThrottling(false),
	  bIsSwipeing(false),
	  LastInteractionTime(0),
	  NormalThumbImage(nullptr),
	  HoveredThumbImage(nullptr),
	  DraggedThumbImage(nullptr),
	  BackgroundBrush(nullptr),
	  TopBrush(nullptr),
	  BottomBrush(nullptr)
{
}

FSlateColor SSwipeBar::GetTrackOpacity() const
{
	if (bDraggingThumb || IsHovered())
	{
		return FLinearColor(1,1,1,1);
	}
	else if ((bAlwaysShowSwipeBarTrack  && Track->GetThumbSizeFraction() > KINDA_SMALL_NUMBER) || AlwaysShowSwipeBar())
	{
		return FLinearColor(1, 1, 1, 0.5f);
	}
	else
	{
		return FLinearColor(1,1,1,0);
	}
}

FLinearColor SSwipeBar::GetThumbOpacity() const
{
	if (Track->GetThumbSizeFraction() <= 0.0f)
	{
		return FLinearColor(1, 1, 1, 0);
	}
	else if ( bDraggingThumb || IsHovered() )
	{
		return FLinearColor(1,1,1,1);
	}
	else
	{
		if ( bHideWhenNotInUse && !bAlwaysShowSwipeBar )
		{
			const double LastInteractionDelta = bIsSwipeing ? 0 : ( FSlateApplication::Get().GetCurrentTime() - LastInteractionTime );

			float ThumbOpacity = FMath::Lerp(1.0f, 0.0f, FMath::Clamp((float)( ( LastInteractionDelta - 0.2 ) / 0.2 ), 0.0f, 1.0f));
			return FLinearColor(1, 1, 1, ThumbOpacity);
		}
		else 
		{
			return FLinearColor(1, 1, 1, 0.75f);
		}
	}
}

void SSwipeBar::BeginSwipeing()
{
	bIsSwipeing = true;
}

void SSwipeBar::EndSwipeing()
{
	bIsSwipeing = false;
	LastInteractionTime = FSlateApplication::Get().GetCurrentTime();
}

const FSlateBrush* SSwipeBar::GetDragThumbImage() const
{
	if ( bDraggingThumb )
	{
		return DraggedThumbImage;
	}
	else if (IsHovered())
	{
		return HoveredThumbImage;
	}
	else
	{
		return NormalThumbImage;
	}
}

EVisibility SSwipeBar::ShouldBeVisible() const
{
	if ( this->HasMouseCapture() )
	{
		return EVisibility::Visible;
	}
	else if( Track->IsNeeded() )
	{
		return UserVisibility.Get();
	}
	else
	{
		return EVisibility::Collapsed;
	}
}

bool SSwipeBar::IsSwipeing() const
{
	return bDraggingThumb;
}

EOrientation SSwipeBar::GetOrientation() const
{
	return Orientation;
}

void SSwipeBar::SetStyle(const FSwipeBarStyle* InStyle)
{
	const FSwipeBarStyle* Style = InStyle;

	if (Style == nullptr)
	{
		FArguments Defaults;
		Style = Defaults._Style;
	}

	check(Style);

	NormalThumbImage = &Style->NormalThumbImage;
	HoveredThumbImage = &Style->HoveredThumbImage;
	DraggedThumbImage = &Style->DraggedThumbImage;

	if (Orientation == Orient_Vertical)
	{
		BackgroundBrush = &Style->VerticalBackgroundImage;
		TopBrush = &Style->VerticalTopSlotImage;
		BottomBrush = &Style->VerticalBottomSlotImage;
	}
	else
	{
		BackgroundBrush = &Style->HorizontalBackgroundImage;
		TopBrush = &Style->HorizontalTopSlotImage;
		BottomBrush = &Style->HorizontalBottomSlotImage;
	}

	InvalidateStyle();
}

void SSwipeBar::InvalidateStyle()
{
	Invalidate(EInvalidateWidgetReason::Layout);
}

void SSwipeBar::SetDragFocusCause(EFocusCause InDragFocusCause)
{
	DragFocusCause = InDragFocusCause;
}

void SSwipeBar::SetThickness(TAttribute<FVector2D> InThickness)
{
	ThicknessSpacer->SetSize(InThickness);
}

void SSwipeBar::SetSwipeBarAlwaysVisible(bool InAlwaysVisible)
{
	bAlwaysShowSwipeBar = InAlwaysVisible;

	if ( InAlwaysVisible )
	{
		SetVisibility(EVisibility::Visible);
	}
	else
	{
		SetVisibility(TAttribute<EVisibility>(SharedThis(this), &SSwipeBar::ShouldBeVisible));
	}

	Track->SetIsAlwaysVisible(InAlwaysVisible);
}

void SSwipeBar::SetSwipeBarTrackAlwaysVisible(bool InAlwaysVisible)
{
	// Doesn't need to be invalidated here, tick updates these values.
	bAlwaysShowSwipeBarTrack = InAlwaysVisible;
}

bool SSwipeBar::AlwaysShowSwipeBar() const
{
	return bAlwaysShowSwipeBar;
}
