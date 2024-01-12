// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Widgets/Layout/SSwipeBox.h"
#include "Rendering/DrawElements.h"
#include "Types/SlateConstants.h"
#include "Layout/LayoutUtils.h"
#include "Widgets/SOverlay.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/Widgets/Layout/SSwipeBar.h"
#include "Widgets/Images/SImage.h"


void SSwipeBox::FSlot::Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs)
{
	TBasicLayoutWidgetSlot<FSlot>::Construct(SlotOwner, MoveTemp(InArgs));
	if (InArgs._MaxSize.IsSet())
	{
		SetMaxSize(MoveTemp(InArgs._MaxSize));
	}
	if (InArgs._SizeParam.IsSet())
	{
		SetSizeParam(MoveTemp(InArgs._SizeParam.GetValue()));
	}
}

void SSwipeBox::FSlot::RegisterAttributes(FSlateWidgetSlotAttributeInitializer& AttributeInitializer)
{
	TBasicLayoutWidgetSlot<FSlot>::RegisterAttributes(AttributeInitializer);
	SLATE_ADD_SLOT_ATTRIBUTE_DEFINITION_WITH_NAME(FSlot, AttributeInitializer, "Slot.MaxSize", MaxSize,
	                                              EInvalidateWidgetReason::Layout);
	SLATE_ADD_SLOT_ATTRIBUTE_DEFINITION_WITH_NAME(FSlot, AttributeInitializer, "Slot.SizeValue", SizeValue,
	                                              EInvalidateWidgetReason::Layout)
		.UpdatePrerequisite("Slot.MaxSize");
}

SSwipeBox::FSlot::FSlotArguments SSwipeBox::Slot()
{
	return FSlot::FSlotArguments(MakeUnique<FSlot>());
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void SSwipePanel::Construct(const FArguments& InArgs, const TArray<SSwipeBox::FSlot*>& InSlots)
{
	PhysicalOffset = 0;
	Children.Reserve(InSlots.Num());
	for (int32 SlotIndex = 0; SlotIndex < InSlots.Num(); ++SlotIndex)
	{
		Children.Add(InSlots[SlotIndex]);
	}
	Orientation = InArgs._Orientation;
	BackPadSwipeing = InArgs._BackPadSwipeing;
	FrontPadSwipeing = InArgs._FrontPadSwipeing;
	SlotStretchChildAsParentSize = InArgs._SlotStretchChildAsParentSize;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS


void SSwipePanel::Construct(const FArguments& InArgs, TArray<SSwipeBox::FSlot::FSlotArguments> InSlots)
{
	PhysicalOffset = 0;
	Children.AddSlots(MoveTemp(InSlots));
	Orientation = InArgs._Orientation;
	BackPadSwipeing = InArgs._BackPadSwipeing;
	FrontPadSwipeing = InArgs._FrontPadSwipeing;
	SlotStretchChildAsParentSize = InArgs._SlotStretchChildAsParentSize;
}

void SSwipePanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	const float SwipePadding = Orientation == Orient_Vertical
		                           ? AllottedGeometry.GetLocalSize().Y
		                           : AllottedGeometry.GetLocalSize().X;
	const float ChildrenOffset = -PhysicalOffset + (BackPadSwipeing ? SwipePadding : 0);
	const bool AllowShrink = false;

	if (Orientation == EOrientation::Orient_Horizontal)
	{
		ArrangeChildrenInStackOverride<EOrientation::Orient_Horizontal>(GSlateFlowDirection, this->Children,
		                                                                AllottedGeometry, ArrangedChildren,
		                                                                ChildrenOffset, AllowShrink,
		                                                                SlotStretchChildAsParentSize);
	}
	else
	{
		ArrangeChildrenInStackOverride<EOrientation::Orient_Vertical>(GSlateFlowDirection, this->Children,
		                                                              AllottedGeometry, ArrangedChildren,
		                                                              ChildrenOffset, AllowShrink,
		                                                              SlotStretchChildAsParentSize);
	}
}

FVector2D SSwipePanel::ComputeDesiredSize(float) const
{
	FVector2D ThisDesiredSize = FVector2D::ZeroVector;
	for (int32 SlotIndex = 0; SlotIndex < Children.Num(); ++SlotIndex)
	{
		const SSwipeBox::FSlot& ThisSlot = Children[SlotIndex];
		if (ThisSlot.GetWidget()->GetVisibility() != EVisibility::Collapsed)
		{
			const FVector2D ChildDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
			if (Orientation == Orient_Vertical)
			{
				ThisDesiredSize.X = FMath::Max(
					ChildDesiredSize.X + ThisSlot.GetPadding().GetTotalSpaceAlong<Orient_Horizontal>(),
					ThisDesiredSize.X);

				if (SlotStretchChildAsParentSize)
				{
					FGeometry PaintSpaceGeometry = GetPaintSpaceGeometry();
					ThisDesiredSize.Y += PaintSpaceGeometry.GetLocalSize().Y;
				}
				else
				{
					ThisDesiredSize.Y += ChildDesiredSize.Y + ThisSlot.GetPadding().GetTotalSpaceAlong<
						Orient_Vertical>();
				}
			}
			else
			{
				if (SlotStretchChildAsParentSize)
				{
					FGeometry PaintSpaceGeometry = GetPaintSpaceGeometry();

					ThisDesiredSize.X += PaintSpaceGeometry.GetLocalSize().X;
				}
				else
				{
					ThisDesiredSize.X += ChildDesiredSize.X + ThisSlot.GetPadding().GetTotalSpaceAlong<
						Orient_Horizontal>();
				}
				ThisDesiredSize.Y = FMath::Max(
					ChildDesiredSize.Y + ThisSlot.GetPadding().GetTotalSpaceAlong<Orient_Vertical>(),
					ThisDesiredSize.Y);
			}
		}
	}

	FVector2D::FReal SwipePadding = Orientation == Orient_Vertical
		                                ? GetTickSpaceGeometry().GetLocalSize().Y
		                                : GetTickSpaceGeometry().GetLocalSize().X;
	FVector2D::FReal& SizeSideToPad = Orientation == Orient_Vertical ? ThisDesiredSize.Y : ThisDesiredSize.X;
	SizeSideToPad += BackPadSwipeing ? SwipePadding : 0;
	SizeSideToPad += FrontPadSwipeing ? SwipePadding : 0;
	// UE_LOG(LogTemp, Warning, TEXT("ComputeDesiredSize: x: %f, y: %f"), ThisDesiredSize.X, ThisDesiredSize.Y)
	return ThisDesiredSize;
}

SSwipeBox::SSwipeBox()
{
	VerticalSwipeBarSlot = nullptr;
	bClippingProxy = true;
}

void SSwipeBox::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);

	Style = InArgs._Style;
	SwipeBarStyle = InArgs._SwipeBarStyle;
	DesiredSwipeOffset = 0;
	bIsSwipeing = false;
	bAnimateSwipe = false;
	AmountSwipeedWhileRightMouseDown = 0;
	PendingSwipeTriggerAmount = 0;
	bShowSoftwareCursor = false;
	SoftwareCursorPosition = FVector2f::ZeroVector;
	OnUserSwipeed = InArgs._OnUserSwipeed;
	Orientation = InArgs._Orientation;
	bSwipeToEnd = false;
	bIsSwipeingActiveTimerRegistered = false;
	bAllowsRightClickDragSwipeing = false;
	ConsumeMouseWheel = InArgs._ConsumeMouseWheel;
	TickSwipeDelta = 0;
	AllowOverSwipe = InArgs._AllowOverSwipe;
	SetAllowStickySwipe(InArgs._AllowStickySwipe);
	SlotStretchChildAsParentSize = InArgs._SlotStretchChildAsParentSize;
	BackPadSwipeing = InArgs._BackPadSwipeing;
	FrontPadSwipeing = InArgs._FrontPadSwipeing;
	bAnimateWheelSwipeing = InArgs._AnimateWheelSwipeing;
	WheelSwipeMultiplier = InArgs._WheelSwipeMultiplier;
	NavigationSwipePadding = InArgs._NavigationSwipePadding;
	NavigationDestination = InArgs._NavigationDestination;
	SwipeWhenFocusChanges = InArgs._SwipeWhenFocusChanges;
	bTouchPanningCapture = false;
	bVolatilityAlwaysInvalidatesPrepass = true;

	StickySwipe.SetBlendExp(InArgs._BlendExp);
	StickySwipe.SetEasing(InArgs._Easing);
	StickySwipe.SetSpeed(InArgs._Speed);


	if (InArgs._ExternalSwipebar.IsValid())
	{
		// An external Swipe bar was specified by the user
		SwipeBar = InArgs._ExternalSwipebar;
		SwipeBar->SetOnUserSwipeed(FOnUserSwipeed::CreateSP(this, &SSwipeBox::SwipeBar_OnUserSwipeed));
		bSwipeBarIsExternal = true;
	}
	else
	{
		// Make a Swipe bar 
		SwipeBar = ConstructSwipeBar();
		SwipeBar->SetDragFocusCause(InArgs._SwipeBarDragFocusCause);
		SwipeBar->SetThickness(InArgs._SwipeBarThickness);
		SwipeBar->SetUserVisibility(InArgs._SwipeBarVisibility);
		SwipeBar->SetSwipeBarAlwaysVisible(InArgs._SwipeBarAlwaysVisible);
		SwipeBarSlotPadding = InArgs._SwipeBarPadding;

		bSwipeBarIsExternal = false;
	}

	SAssignNew(SwipePanel, SSwipePanel, MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)))
		.Clipping(InArgs._Clipping)
		.Orientation(Orientation)
		.BackPadSwipeing(BackPadSwipeing)
		.FrontPadSwipeing(FrontPadSwipeing)
		.SlotStretchChildAsParentSize(SlotStretchChildAsParentSize);

	if (Orientation == Orient_Vertical)
	{
		ConstructVerticalLayout();
	}
	else
	{
		ConstructHorizontalLayout();
	}

	SwipeBar->SetState(0.0f, 1.0f);
}

void SSwipeBox::OnClippingChanged()
{
	SwipePanel->SetClipping(Clipping);
}

TSharedPtr<SSwipeBar> SSwipeBox::ConstructSwipeBar()
{
	return TSharedPtr<SSwipeBar>(SNew(SSwipeBar)
		.Style(SwipeBarStyle)
		.Orientation(Orientation)
		.Padding(0.0f)
		.OnUserSwipeed(this, &SSwipeBox::SwipeBar_OnUserSwipeed));
}

void SSwipeBox::ConstructVerticalLayout()
{
	TSharedPtr<SHorizontalBox> PanelAndSwipebar;
	this->ChildSlot
	[
		SAssignNew(PanelAndSwipebar, SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.Padding(Style->VerticalSwipeedContentPadding)
			[
				// Swipe panel that presents the Swipeed content
				SwipePanel.ToSharedRef()
			]

			+ SOverlay::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Top)
			[
				// Shadow: Hint to Swipe up
				SNew(SImage)
				.Visibility(EVisibility::HitTestInvisible)
				.ColorAndOpacity(this, &SSwipeBox::GetStartShadowOpacity)
				.Image(&Style->TopShadowBrush)
			]

			+ SOverlay::Slot()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Bottom)
			[
				// Shadow: a hint to Swipe down
				SNew(SImage)
				.Visibility(EVisibility::HitTestInvisible)
				.ColorAndOpacity(this, &SSwipeBox::GetEndShadowOpacity)
				.Image(&Style->BottomShadowBrush)
			]
		]
	];

	VerticalSwipeBarSlot = nullptr;
	if (!bSwipeBarIsExternal)
	{
		PanelAndSwipebar->AddSlot()
		                .Padding(SwipeBarSlotPadding)
		                .AutoWidth()
		                .Expose(VerticalSwipeBarSlot)
		[
			SwipeBar.ToSharedRef()
		];
	}
}

void SSwipeBox::ConstructHorizontalLayout()
{
	TSharedPtr<SVerticalBox> PanelAndSwipebar;
	this->ChildSlot
	[
		SAssignNew(PanelAndSwipebar, SVerticalBox)

		+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.Padding(Style->HorizontalSwipeedContentPadding)
			[
				// Swipe panel that presents the Swipeed content
				SwipePanel.ToSharedRef()
			]

			+ SOverlay::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			[
				// Shadow: Hint to left
				SNew(SImage)
				.Visibility(EVisibility::HitTestInvisible)
				.ColorAndOpacity(this, &SSwipeBox::GetStartShadowOpacity)
				.Image(&Style->LeftShadowBrush)
			]

			+ SOverlay::Slot()
			  .HAlign(HAlign_Right)
			  .VAlign(VAlign_Fill)
			[
				// Shadow: a hint to Swipe right
				SNew(SImage)
				.Visibility(EVisibility::HitTestInvisible)
				.ColorAndOpacity(this, &SSwipeBox::GetEndShadowOpacity)
				.Image(&Style->RightShadowBrush)
			]
		]
	];

	HorizontalSwipeBarSlot = nullptr;
	if (!bSwipeBarIsExternal)
	{
		PanelAndSwipebar->AddSlot()
		                .Padding(SwipeBarSlotPadding)
		                .AutoHeight()
		                .Expose(HorizontalSwipeBarSlot)
		[
			SwipeBar.ToSharedRef()
		];
	}
}

/** Adds a slot to SSwipeBox */
SSwipeBox::FScopedWidgetSlotArguments SSwipeBox::AddSlot()
{
	return FScopedWidgetSlotArguments{MakeUnique<FSlot>(), SwipePanel->Children, INDEX_NONE};
}

/** Removes a slot at the specified location */
void SSwipeBox::RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove)
{
	SwipePanel->Children.Remove(WidgetToRemove);
}

void SSwipeBox::ClearChildren()
{
	SwipePanel->Children.Empty();
}

bool SSwipeBox::IsRightClickSwipeing() const
{
	return FSlateApplication::IsInitialized() && AmountSwipeedWhileRightMouseDown >= FSlateApplication::Get().
		GetDragTriggerDistance() && this->SwipeBar->IsNeeded();
}

float SSwipeBox::GetSwipeOffset() const
{
	return DesiredSwipeOffset;
}

float SSwipeBox::GetSwipeOffsetOfEnd() const
{
	const FGeometry SwipePanelGeometry = FindChildGeometry(CachedGeometry, SwipePanel.ToSharedRef());
	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());
	return FMath::Max(ContentSize - GetSwipeComponentFromVector(SwipePanelGeometry.Size), 0.0f);
}

float SSwipeBox::GetViewFraction() const
{
	const FGeometry SwipePanelGeometry = FindChildGeometry(CachedGeometry, SwipePanel.ToSharedRef());
	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());

	return FMath::Clamp<float>(GetSwipeComponentFromVector(CachedGeometry.GetLocalSize()) > 0
		                           ? GetSwipeComponentFromVector(SwipePanelGeometry.Size) / ContentSize
		                           : 1, 0.0f, 1.0f);
}

float SSwipeBox::GetViewOffsetFraction() const
{
	const FGeometry SwipePanelGeometry = FindChildGeometry(CachedGeometry, SwipePanel.ToSharedRef());
	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());

	const float ViewFraction = GetViewFraction();
	return FMath::Clamp(DesiredSwipeOffset / ContentSize, 0.f, 1.f - ViewFraction);
}

void SSwipeBox::SetSwipeOffset(float NewSwipeOffset)
{
	DesiredSwipeOffset = NewSwipeOffset;
	bSwipeToEnd = false;

	Invalidate(EInvalidateWidget::Layout);
}

void SSwipeBox::SwipeToStart()
{
	SetSwipeOffset(0);
}

void SSwipeBox::SwipeToEnd()
{
	bSwipeToEnd = true;

	Invalidate(EInvalidateWidget::Layout);
}

void SSwipeBox::SwipeDescendantIntoView(const TSharedPtr<SWidget>& WidgetToSwipeIntoView, bool InAnimateSwipe,
                                        EDescendantScrollDestination InDestination, float InSwipePadding)
{
	SwipeIntoViewRequest = [this, WidgetToSwipeIntoView, InAnimateSwipe, InDestination, InSwipePadding
		](FGeometry AllottedGeometry)
		{
			InternalSwipeDescendantIntoView(AllottedGeometry, WidgetToSwipeIntoView, InAnimateSwipe, InDestination,
			                                InSwipePadding);
		};

	// if (AllowStickySwipe == EAllowStickyswipe::No)
	// {
		BeginInertialSwipeing();
	// }
}

bool SSwipeBox::InternalSwipeDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind,
                                                bool InAnimateSwipe, EDescendantScrollDestination InDestination,
                                                float InSwipePadding)
{
	// We need to safely find the one WidgetToFind among our descendants.
	TSet<TSharedRef<SWidget>> WidgetsToFind;
	{
		if (WidgetToFind.IsValid())
		{
			WidgetsToFind.Add(WidgetToFind.ToSharedRef());
		}
	}
	TMap<TSharedRef<SWidget>, FArrangedWidget> Result;

	FindChildGeometries(MyGeometry, WidgetsToFind, Result);

	if (WidgetToFind.IsValid())
	{
		FArrangedWidget* WidgetGeometry = Result.Find(WidgetToFind.ToSharedRef());
		if (!WidgetGeometry)
		{
			UE_LOG(LogSlate, Warning, TEXT("Unable to Swipe to descendant as it's not a child of the Swipebox"));
		}
		else
		{
			float SwipeOffset = 0.0f;
			if (InDestination == EDescendantScrollDestination::TopOrLeft)
			{
				// Calculate how much we would need to Swipe to bring this to the top/left of the Swipe box
				const float WidgetPosition = GetSwipeComponentFromVector(
					MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()));
				const float MyPosition = InSwipePadding;
				SwipeOffset = WidgetPosition - MyPosition;
			}
			else if (InDestination == EDescendantScrollDestination::BottomOrRight)
			{
				// Calculate how much we would need to Swipe to bring this to the bottom/right of the Swipe box
				const float WidgetPosition = GetSwipeComponentFromVector(
					MyGeometry.AbsoluteToLocal(
						WidgetGeometry->Geometry.GetAbsolutePosition() + WidgetGeometry->Geometry.GetAbsoluteSize()) -
					MyGeometry.GetLocalSize());
				const float MyPosition = InSwipePadding;
				SwipeOffset = WidgetPosition - MyPosition;
			}
			else if (InDestination == EDescendantScrollDestination::Center)
			{
				// Calculate how much we would need to Swipe to bring this to the top/left of the Swipe box
				const float WidgetPosition = GetSwipeComponentFromVector(
					MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()) + (WidgetGeometry->
						Geometry.GetLocalSize() / 2));
				const float MyPosition = GetSwipeComponentFromVector(MyGeometry.GetLocalSize() * FVector2f(0.5f, 0.5f));
				SwipeOffset = WidgetPosition - MyPosition;
			}
			else
			{
				const float WidgetStartPosition = GetSwipeComponentFromVector(
					MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()));
				const float WidgetEndPosition = WidgetStartPosition + GetSwipeComponentFromVector(
					WidgetGeometry->Geometry.GetLocalSize());
				const float ViewStartPosition = InSwipePadding;
				const float ViewEndPosition = GetSwipeComponentFromVector(MyGeometry.GetLocalSize() - InSwipePadding);

				const float ViewDelta = (ViewEndPosition - ViewStartPosition);
				const float WidgetDelta = (WidgetEndPosition - WidgetStartPosition);

				if (WidgetStartPosition < ViewStartPosition)
				{
					SwipeOffset = WidgetStartPosition - ViewStartPosition;
				}
				else if (WidgetEndPosition > ViewEndPosition)
				{
					SwipeOffset = (WidgetEndPosition - ViewDelta) - ViewStartPosition;
				}
			}

			if (SwipeOffset != 0.0f)
			{
				DesiredSwipeOffset = SwipePanel->PhysicalOffset;
				SwipeBy(MyGeometry, SwipeOffset, EAllowOverscroll::No, InAnimateSwipe);
			}

			return true;
		}
	}

	return false;
}

void SSwipeBox::SetStyle(const FSwipeBoxStyle* InStyle)
{
	if (Style != InStyle)
	{
		Style = InStyle;
		InvalidateStyle();
	}
}

void SSwipeBox::SetSwipeBarStyle(const FSwipeBarStyle* InBarStyle)
{
	if (InBarStyle != SwipeBarStyle)
	{
		SwipeBarStyle = InBarStyle;
		if (!bSwipeBarIsExternal && SwipeBar.IsValid())
		{
			SwipeBar->SetStyle(SwipeBarStyle);
		}
	}
}

void SSwipeBox::InvalidateStyle()
{
	Invalidate(EInvalidateWidgetReason::Layout);
}

void SSwipeBox::InvalidateSwipeBarStyle()
{
	if (SwipeBar.IsValid())
	{
		SwipeBar->InvalidateStyle();
	}
}

EOrientation SSwipeBox::GetOrientation()
{
	return Orientation;
}

void SSwipeBox::SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination)
{
	NavigationDestination = NewNavigationDestination;
}

void SSwipeBox::SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel)
{
	ConsumeMouseWheel = NewConsumeMouseWheel;
}

void SSwipeBox::SetOrientation(EOrientation InOrientation)
{
	if (Orientation != InOrientation)
	{
		Orientation = InOrientation;
		if (!bSwipeBarIsExternal)
		{
			SwipeBar = ConstructSwipeBar();
		}
		SwipePanel->SetOrientation(Orientation);
		if (Orientation == Orient_Vertical)
		{
			ConstructVerticalLayout();
		}
		else
		{
			ConstructHorizontalLayout();
		}
	}
}

void SSwipeBox::SetSwipeBarVisibility(EVisibility InVisibility)
{
	SwipeBar->SetUserVisibility(InVisibility);
}

void SSwipeBox::SetSwipeBarAlwaysVisible(bool InAlwaysVisible)
{
	SwipeBar->SetSwipeBarAlwaysVisible(InAlwaysVisible);
}

void SSwipeBox::SetSwipeBarTrackAlwaysVisible(bool InAlwaysVisible)
{
	SwipeBar->SetSwipeBarTrackAlwaysVisible(InAlwaysVisible);
}

void SSwipeBox::SetSwipeBarThickness(UE::Slate::FDeprecateVector2DParameter InThickness)
{
	SwipeBar->SetThickness(InThickness);
}

void SSwipeBox::SetSwipeBarPadding(const FMargin& InPadding)
{
	SwipeBarSlotPadding = InPadding;

	if (Orientation == Orient_Vertical)
	{
		if (VerticalSwipeBarSlot)
		{
			VerticalSwipeBarSlot->SetPadding(SwipeBarSlotPadding);
		}
	}
	else
	{
		if (HorizontalSwipeBarSlot)
		{
			HorizontalSwipeBarSlot->SetPadding(SwipeBarSlotPadding);
		}
	}
}

void SSwipeBox::SetSwipeBarRightClickDragAllowed(bool bIsAllowed)
{
	bAllowsRightClickDragSwipeing = bIsAllowed;
}

EActiveTimerReturnType SSwipeBox::UpdateInertialSwipe(double InCurrentTime, float InDeltaTime)
{
	if (AllowStickySwipe != EAllowStickyswipe::No)
	{
		bool bKeepTicking = bIsSwipeing;

		if (bIsSwipeing)
		{
			InertialSwipeManager.UpdateScrollVelocity(InDeltaTime);
			const float SwipeVelocityLocal = InertialSwipeManager.GetScrollVelocity() / CachedGeometry.Scale;

			if (SwipeVelocityLocal != 0.f)
			{
				if (CanUseInertialSwipe(SwipeVelocityLocal))
				{
					bKeepTicking = true;
					SwipeBy(CachedGeometry, SwipeVelocityLocal * InDeltaTime, AllowOverSwipe, false);
				}
				else
				{
					InertialSwipeManager.ClearScrollVelocity();
				}
			}
		}

		if (AllowOverSwipe == EAllowOverscroll::Yes)
		{
			// If we are currently in overSwipe, the list will need refreshing.
			// Do this before UpdateOverSwipe, as that could cause GetOverSwipe() to be 0
			if (OverSwipe.GetOverscroll(CachedGeometry) != 0.0f)
			{
				bKeepTicking = true;
			}

			OverSwipe.UpdateOverscroll(InDeltaTime);
		}

		TickSwipeDelta = 0.f;

		if (!bKeepTicking)
		{
			bIsSwipeing = false;
			bIsSwipeingActiveTimerRegistered = false;
			Invalidate(EInvalidateWidget::LayoutAndVolatility);
			UpdateInertialSwipeHandle.Reset();
		}
	return bKeepTicking ? EActiveTimerReturnType::Continue : EActiveTimerReturnType::Stop;
	}
return EActiveTimerReturnType::Stop;
}

void SSwipeBox::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	CachedGeometry = AllottedGeometry;

	if (AllowStickySwipe == EAllowStickyswipe::No && (bTouchPanningCapture && (FSlateApplication::Get().GetCurrentTime()
		- LastSwipeTime) > 0.10))
	{
		InertialSwipeManager.ClearScrollVelocity();
	}

	// If we needed a widget to be Swipeed into view, make that happen.
	if (SwipeIntoViewRequest)
	{
		SwipeIntoViewRequest(AllottedGeometry);
		SwipeIntoViewRequest = nullptr;
	}

	const FGeometry SwipePanelGeometry = FindChildGeometry(AllottedGeometry, SwipePanel.ToSharedRef());
	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());

	if (bSwipeToEnd)
	{
		DesiredSwipeOffset = FMath::Max(ContentSize - GetSwipeComponentFromVector(SwipePanelGeometry.GetLocalSize()),
		                                0.0f);
		bSwipeToEnd = false;
	}

	// If this Swipe box has no size, do not compute a view fraction because it will be wrong and causes pop in when the size is available
	const float ViewFraction = GetViewFraction();
	const float TargetViewOffset = GetViewOffsetFraction();

	const float CurrentViewOffset = bAnimateSwipe
		                                ? FMath::FInterpTo(SwipeBar->DistanceFromTop(), TargetViewOffset, InDeltaTime,
		                                                   15.f)
		                                : TargetViewOffset;

	// Update the Swipebar with the clamped version of the offset
	float NewPhysicalOffset = GetSwipeComponentFromVector(CurrentViewOffset * SwipePanel->GetDesiredSize());
	if (AllowOverSwipe == EAllowOverscroll::Yes)
	{
		NewPhysicalOffset += OverSwipe.GetOverscroll(AllottedGeometry);
	}
	if (AllowStickySwipe == EAllowStickyswipe::Yes)
	{
		NewPhysicalOffset += StickySwipe.GetStickyswipe(AllottedGeometry);
		StickySwipe.UpdateStickyswipe(SwipePanel.ToSharedRef(), CachedGeometry, InDeltaTime);
	}

	const bool bWasSwipeing = bIsSwipeing;
	bIsSwipeing = !FMath::IsNearlyEqual(NewPhysicalOffset, SwipePanel->PhysicalOffset, 0.001f);

	SwipePanel->PhysicalOffset = NewPhysicalOffset;

	if (bWasSwipeing && !bIsSwipeing)
	{
		Invalidate(EInvalidateWidget::Layout);
	}

	SwipeBar->SetState(CurrentViewOffset, ViewFraction);
	if (!SwipeBar->IsNeeded())
	{
		// We cannot Swipe, so ensure that there is no offset.
		SwipePanel->PhysicalOffset = 0.0f;
	}
}

bool SSwipeBox::ComputeVolatility() const
{
	return bIsSwipeing || IsRightClickSwipeing();
}

FReply SSwipeBox::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsTouchEvent() && !bFingerOwningTouchInteraction.IsSet())
	{
		// Clear any inertia 
		if (AllowStickySwipe == EAllowStickyswipe::No)
		 {
			InertialSwipeManager.ClearScrollVelocity();
		 }
		// We have started a new interaction; track how far the user has moved since they put their finger down.
		AmountSwipeedWhileRightMouseDown = 0;
		PendingSwipeTriggerAmount = 0;
		// Someone put their finger down in this list, so they probably want to drag the list.
		bFingerOwningTouchInteraction = MouseEvent.GetPointerIndex();

		Invalidate(EInvalidateWidget::Layout);
	}
	return FReply::Unhandled();
}

FReply SSwipeBox::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (/*AllowStickySwipe == EAllowStickyswipe::No && */!bFingerOwningTouchInteraction.IsSet())
	{
		EndInertialSwipeing();
	}

	if (MouseEvent.IsTouchEvent())
	{
		return FReply::Handled();
	}
	else
	{
		if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && SwipeBar->IsNeeded() &&
			bAllowsRightClickDragSwipeing)
		{
			AmountSwipeedWhileRightMouseDown = 0;

			Invalidate(EInvalidateWidget::Layout);

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply SSwipeBox::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bAllowsRightClickDragSwipeing)
	{
		if (/*AllowStickySwipe == EAllowStickyswipe::No && */!bIsSwipeingActiveTimerRegistered && IsRightClickSwipeing())
		{
			// Register the active timer to handle the inertial Swipeing
			CachedGeometry = MyGeometry;
			BeginInertialSwipeing();
		}

		AmountSwipeedWhileRightMouseDown = 0;

		Invalidate(EInvalidateWidget::Layout);

		FReply Reply = FReply::Handled().ReleaseMouseCapture();
		bShowSoftwareCursor = false;

		// If we have mouse capture, snap the mouse back to the closest location that is within the panel's bounds
		if (HasMouseCapture())
		{
			FSlateRect PanelScreenSpaceRect = MyGeometry.GetLayoutBoundingRect();
			FVector2f CursorPosition = MyGeometry.LocalToAbsolute(SoftwareCursorPosition);

			FIntPoint BestPositionInPanel(
				FMath::RoundToInt(FMath::Clamp(CursorPosition.X, PanelScreenSpaceRect.Left,
				                               PanelScreenSpaceRect.Right)),
				FMath::RoundToInt(FMath::Clamp(CursorPosition.Y, PanelScreenSpaceRect.Top, PanelScreenSpaceRect.Bottom))
			);

			Reply.SetMousePos(BestPositionInPanel);
		}

		return Reply;
	}

	return FReply::Unhandled();
}

FReply SSwipeBox::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const float SwipeByAmountScreen = GetSwipeComponentFromVector(MouseEvent.GetCursorDelta());
	const float SwipeByAmountLocal = SwipeByAmountScreen / MyGeometry.Scale;

	if (MouseEvent.IsTouchEvent())
	{
		FReply Reply = FReply::Unhandled();

		if (!bTouchPanningCapture)
		{
			if (bFingerOwningTouchInteraction.IsSet() && MouseEvent.IsTouchEvent() && !HasMouseCapture())
			{
				PendingSwipeTriggerAmount += SwipeByAmountScreen;

				if (FMath::Abs(PendingSwipeTriggerAmount) > FSlateApplication::Get().GetDragTriggerDistance())
				{
					bTouchPanningCapture = true;
					SwipeBar->BeginSwipeing();

					// The user has moved the list some amount; they are probably
					// trying to Swipe. From now on, the list assumes the user is Swipeing
					// until they lift their finger.
					Reply = FReply::Handled().CaptureMouse(AsShared());
				}
				else
				{
					Reply = FReply::Handled();
				}
			}
		}
		else
		{
			if (bFingerOwningTouchInteraction.IsSet() && HasMouseCaptureByUser(
				MouseEvent.GetUserIndex(), MouseEvent.GetPointerIndex()))
			{
				LastSwipeTime = FSlateApplication::Get().GetCurrentTime();
				if (AllowStickySwipe == EAllowStickyswipe::No)
				{
					InertialSwipeManager.AddScrollSample(-SwipeByAmountScreen,
					                                     FSlateApplication::Get().GetCurrentTime());
				}

				SwipeBy(MyGeometry, -SwipeByAmountLocal, EAllowOverscroll::Yes, false, true);

				Reply = FReply::Handled();
			}
		}

		return Reply;
	}
	else
	{
		if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && bAllowsRightClickDragSwipeing)
		{
			// If Swipeing with the right mouse button, we need to remember how much we Swipeed.
			// If we did not Swipe at all, we will bring up the context menu when the mouse is released.
			AmountSwipeedWhileRightMouseDown += FMath::Abs(SwipeByAmountScreen);

			// Has the mouse moved far enough with the right mouse button held down to start capturing
			// the mouse and dragging the view?
			if (IsRightClickSwipeing())
			{
				if (AllowStickySwipe == EAllowStickyswipe::No)
				{
					InertialSwipeManager.AddScrollSample(-SwipeByAmountScreen, FPlatformTime::Seconds());
				}
				const bool bDidSwipe = SwipeBy(MyGeometry, -SwipeByAmountLocal, AllowOverSwipe, false, true);

				FReply Reply = FReply::Handled();

				// Capture the mouse if we need to
				if (HasMouseCapture() == false)
				{
					Reply.CaptureMouse(AsShared()).UseHighPrecisionMouseMovement(AsShared());
					SoftwareCursorPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
					bShowSoftwareCursor = true;
				}

				// Check if the mouse has moved.
				if (bDidSwipe)
				{
					SetSwipeComponentOnVector(SoftwareCursorPosition,
					                          GetSwipeComponentFromVector(SoftwareCursorPosition) + SwipeByAmountLocal);
				}

				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

void SSwipeBox::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsTouchEvent())
	{
		if (!bFingerOwningTouchInteraction.IsSet())
		{
			// If we currently do not have touch capture, allow this widget to begin Swipeing on pointer enter events
			// if it comes from a child widget
			if (MyGeometry.IsUnderLocation(MouseEvent.GetLastScreenSpacePosition()))
			{
				bFingerOwningTouchInteraction = MouseEvent.GetPointerIndex();
			}
		}
	}
}

void SSwipeBox::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (HasMouseCapture() == false)
	{
		// No longer Swipeing (unless we have mouse capture)
		if (AmountSwipeedWhileRightMouseDown != 0)
		{
			AmountSwipeedWhileRightMouseDown = 0;
			Invalidate(EInvalidateWidget::Layout);
		}

		if (MouseEvent.IsTouchEvent())
		{
			bFingerOwningTouchInteraction.Reset();
		}
	}
}

FReply SSwipeBox::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if ((SwipeBar->IsNeeded() && ConsumeMouseWheel != EConsumeMouseWheel::Never) || ConsumeMouseWheel ==
		EConsumeMouseWheel::Always)
	{
		// Make sure Swipe velocity is cleared so it doesn't fight with the mouse wheel input
		if (AllowStickySwipe == EAllowStickyswipe::No)
		{
			InertialSwipeManager.ClearScrollVelocity();
		}

		const bool bSwipeWasHandled = SwipeBy(
			MyGeometry, -MouseEvent.GetWheelDelta() * GetGlobalScrollAmount() * WheelSwipeMultiplier,
			EAllowOverscroll::No, bAnimateWheelSwipeing, true);

		if (bSwipeWasHandled && !bIsSwipeingActiveTimerRegistered)
		{
			// Register the active timer to handle the inertial Swipeing
			CachedGeometry = MyGeometry;
			// if (AllowStickySwipe == EAllowStickyswipe::No)
			// {
				BeginInertialSwipeing();
			// }
		}

		return bSwipeWasHandled ? FReply::Handled() : FReply::Unhandled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

bool SSwipeBox::SwipeBy(const FGeometry& AllottedGeometry, float LocalSwipeAmount, EAllowOverscroll OverSwipeing,
                        bool InAnimateSwipe, bool InActionFromUser)
{
	Invalidate(EInvalidateWidget::LayoutAndVolatility);

	bAnimateSwipe = InAnimateSwipe;

	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());
	const FGeometry SwipePanelGeometry = FindChildGeometry(AllottedGeometry, SwipePanel.ToSharedRef());

	const float PreviousSwipeOffset = DesiredSwipeOffset;

	if (LocalSwipeAmount != 0)
	{
		const float SwipeMin = 0.0f;
		const float SwipeMax = FMath::Max(ContentSize - GetSwipeComponentFromVector(SwipePanelGeometry.GetLocalSize()),
		                                  0.0f);

		if (AllowOverSwipe == EAllowOverscroll::Yes && OverSwipeing == EAllowOverscroll::Yes && OverSwipe.
			ShouldApplyOverscroll(DesiredSwipeOffset == 0, DesiredSwipeOffset == SwipeMax, LocalSwipeAmount))
		{
			float result = OverSwipe.ScrollBy(AllottedGeometry, LocalSwipeAmount);
			UE_LOG(LogTemp, Warning, TEXT("OverSwipe amount: %f, AnimateSwipe: %s"), result,
			       InAnimateSwipe?TEXT("true"):TEXT("false"))
		}
		else
		{
			if (AllowStickySwipe == EAllowStickyswipe::Yes && StickySwipe.ShouldApplyStickyswipe())
			{
				StickySwipe.SwipeBy(AllottedGeometry, LocalSwipeAmount, InActionFromUser);
				// UE_LOG(LogTemp, Warning, TEXT("Sticky DesiredSwipeOffset amount: %f, AnimateSwipe: %s"), DesiredSwipeOffset, InAnimateSwipe?TEXT("true"):TEXT("false"))
				// DesiredSwipeOffset = FMath::Clamp(DesiredSwipeOffset + LocalSwipeAmount, SwipeMin, SwipeMax);
			}
			else
			{
				// UE_LOG(LogTemp, Warning, TEXT("DesiredSwipeOffset amount: %f, AnimateSwipe: %s"), DesiredSwipeOffset, InAnimateSwipe?TEXT("true"):TEXT("false"))
				DesiredSwipeOffset = FMath::Clamp(DesiredSwipeOffset + LocalSwipeAmount, SwipeMin, SwipeMax);
			}
		}
	}

	OnUserSwipeed.ExecuteIfBound(DesiredSwipeOffset);

	return ConsumeMouseWheel == EConsumeMouseWheel::Always || DesiredSwipeOffset != PreviousSwipeOffset;
}

FCursorReply SSwipeBox::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	if (IsRightClickSwipeing())
	{
		// We hide the native cursor as we'll be drawing the software EMouseCursor::GrabHandClosed cursor
		return FCursorReply::Cursor(EMouseCursor::None);
	}
	else
	{
		return FCursorReply::Unhandled();
	}
}

FReply SSwipeBox::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	CachedGeometry = MyGeometry;

	if (HasMouseCaptureByUser(InTouchEvent.GetUserIndex(), InTouchEvent.GetPointerIndex()))
	{
		SwipeBar->EndSwipeing();
		Invalidate(EInvalidateWidget::Layout);

		// if (AllowStickySwipe == EAllowStickyswipe::No)
		// {
			BeginInertialSwipeing();
		// }
		if (AllowStickySwipe == EAllowStickyswipe::Yes)
		{
			StickySwipe.OnUserReleaseInterraction();
		}

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SSwipeBox::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	SCompoundWidget::OnMouseCaptureLost(CaptureLostEvent);
	AmountSwipeedWhileRightMouseDown = 0;
	PendingSwipeTriggerAmount = 0;
	bFingerOwningTouchInteraction.Reset();
	bTouchPanningCapture = false;
}

FNavigationReply SSwipeBox::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	TSharedPtr<SWidget> FocusedChild;
	int32 FocusedChildIndex = -1;
	int32 FocusedChildDirection = 0;

	// Find the child with focus currently so that we can find the next logical child we're going to move to.
	TPanelChildren<SSwipeBox::FSlot>& Children = SwipePanel->Children;
	for (int32 SlotIndex = 0; SlotIndex < Children.Num(); ++SlotIndex)
	{
		if (Children[SlotIndex].GetWidget()->HasUserFocus(InNavigationEvent.GetUserIndex()).IsSet() ||
			Children[SlotIndex].GetWidget()->HasUserFocusedDescendants(InNavigationEvent.GetUserIndex()))
		{
			FocusedChild = Children[SlotIndex].GetWidget();
			FocusedChildIndex = SlotIndex;
			break;
		}
	}

	if (FocusedChild.IsValid())
	{
		if (Orientation == Orient_Vertical)
		{
			switch (InNavigationEvent.GetNavigationType())
			{
			case EUINavigation::Up:
				FocusedChildDirection = -1;
				break;
			case EUINavigation::Down:
				FocusedChildDirection = 1;
				break;
			default:
				// If we don't handle this direction in our current orientation we can 
				// just allow the behavior of the boundary rule take over.
				return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
			}
		}
		else // Orient_Horizontal
		{
			switch (InNavigationEvent.GetNavigationType())
			{
			case EUINavigation::Left:
				FocusedChildDirection = -1;
				break;
			case EUINavigation::Right:
				FocusedChildDirection = 1;
				break;
			default:
				// If we don't handle this direction in our current orientation we can 
				// just allow the behavior of the boundary rule take over.
				return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
			}
		}

		// If the focused child index is in a valid range we know we can successfully focus
		// the new child we're moving focus to.
		if (FocusedChildDirection != 0)
		{
			TSharedPtr<SWidget> NextFocusableChild;

			// Search in the direction we need to move for the next focusable child of the Swipebox.
			for (int32 ChildIndex = FocusedChildIndex + FocusedChildDirection; ChildIndex >= 0 && ChildIndex < Children.
			     Num(); ChildIndex += FocusedChildDirection)
			{
				TSharedPtr<SWidget> PossiblyFocusableChild = GetKeyboardFocusableWidget(
					Children[ChildIndex].GetWidget());
				if (PossiblyFocusableChild.IsValid())
				{
					NextFocusableChild = PossiblyFocusableChild;
					break;
				}
			}

			// If we found a focusable child, Swipe to it, and shift focus.
			if (NextFocusableChild.IsValid())
			{
				InternalSwipeDescendantIntoView(MyGeometry, NextFocusableChild, false, NavigationDestination,
				                                NavigationSwipePadding);
				return FNavigationReply::Explicit(NextFocusableChild);
			}
		}
	}

	return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
}

void SSwipeBox::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
                                const FFocusEvent& InFocusEvent)
{
	if (SwipeWhenFocusChanges != EScrollWhenFocusChanges::NoScroll)
	{
		if (NewWidgetPath.IsValid() && NewWidgetPath.ContainsWidget(this))
		{
			SwipeDescendantIntoView(NewWidgetPath.GetLastWidget(),
			                        SwipeWhenFocusChanges == EScrollWhenFocusChanges::AnimatedScroll ? true : false,
			                        NavigationDestination, NavigationSwipePadding);
		}
	}
}

TSharedPtr<SWidget> SSwipeBox::GetKeyboardFocusableWidget(TSharedPtr<SWidget> InWidget)
{
	if (EVisibility::DoesVisibilityPassFilter(InWidget->GetVisibility(), EVisibility::Visible))
	{
		if (InWidget->SupportsKeyboardFocus())
		{
			return InWidget;
		}
		else
		{
			FChildren* Children = InWidget->GetChildren();
			for (int32 i = 0; i < Children->Num(); ++i)
			{
				TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(i);
				TSharedPtr<SWidget> FoucusableWidget = GetKeyboardFocusableWidget(ChildWidget);
				if (FoucusableWidget.IsValid() && EVisibility::DoesVisibilityPassFilter(
					FoucusableWidget->GetVisibility(), EVisibility::Visible))
				{
					return FoucusableWidget;
				}
			}
		}
	}
	return nullptr;
}

int32 SSwipeBox::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                         FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                         bool bParentEnabled) const
{
	int32 NewLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId,
	                                            InWidgetStyle, bParentEnabled);

	if (!bShowSoftwareCursor)
	{
		return NewLayerId;
	}

	const FSlateBrush* Brush = FCoreStyle::Get().GetBrush(TEXT("SoftwareCursor_Grab"));
	const FVector2f CursorSize = Brush->ImageSize / AllottedGeometry.Scale;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		++NewLayerId,
		AllottedGeometry.
		ToPaintGeometry(CursorSize, FSlateLayoutTransform(SoftwareCursorPosition - (CursorSize * .5f))),
		Brush
	);

	return NewLayerId;
}

void SSwipeBox::SwipeBar_OnUserSwipeed(float InSwipeOffsetFraction)
{
	bAnimateSwipe = false;

	const float ContentSize = GetSwipeComponentFromVector(SwipePanel->GetDesiredSize());
	const FGeometry SwipePanelGeometry = FindChildGeometry(CachedGeometry, SwipePanel.ToSharedRef());

	// Clamp to max Swipe offset
	DesiredSwipeOffset = FMath::Min(InSwipeOffsetFraction * ContentSize,
	                                ContentSize - GetSwipeComponentFromVector(SwipePanelGeometry.GetLocalSize()));
	OnUserSwipeed.ExecuteIfBound(DesiredSwipeOffset);

	Invalidate(EInvalidateWidget::Layout);
}

const float ShadowFadeDistance = 32.0f;

FSlateColor SSwipeBox::GetStartShadowOpacity() const
{
	// The shadow should only be visible when the user needs a hint that they can Swipe up.
	const float ShadowOpacity = FMath::Clamp(SwipePanel->PhysicalOffset / ShadowFadeDistance, 0.0f, 1.0f);

	return FLinearColor(1.0f, 1.0f, 1.0f, ShadowOpacity);
}

FSlateColor SSwipeBox::GetEndShadowOpacity() const
{
	// The shadow should only be visible when the user needs a hint that they can Swipe down.
	const float ShadowOpacity = (SwipeBar->DistanceFromBottom() * GetSwipeComponentFromVector(
		SwipePanel->GetDesiredSize()) / ShadowFadeDistance);

	return FLinearColor(1.0f, 1.0f, 1.0f, ShadowOpacity);
}

bool SSwipeBox::CanUseInertialSwipe(float SwipeAmount) const
{
	const auto CurrentOverSwipe = OverSwipe.GetOverscroll(CachedGeometry);

	// We allow sampling for the inertial Swipe if we are not in the overSwipe region,
	// Or if we are Swipeing outwards of the overSwipe region
	return AllowStickySwipe == EAllowStickyswipe::No && (CurrentOverSwipe == 0.f || FMath::Sign(CurrentOverSwipe) !=
		FMath::Sign(SwipeAmount));
}

EAllowOverscroll SSwipeBox::GetAllowOverSwipe() const
{
	return AllowOverSwipe;
}

void SSwipeBox::SetAllowOverSwipe(EAllowOverscroll NewAllowOverSwipe)
{
	AllowOverSwipe = NewAllowOverSwipe;

	if (AllowOverSwipe == EAllowOverscroll::No)
	{
		OverSwipe.ResetOverscroll();
	}
}

EAllowStickyswipe SSwipeBox::GetAllowStickySwipe() const
{
	return AllowStickySwipe;
}

void SSwipeBox::SetAllowStickySwipe(EAllowStickyswipe NewAllowStickySwipe)
{
	AllowStickySwipe = NewAllowStickySwipe;

	if (AllowStickySwipe == EAllowStickyswipe::No)
	{
		StickySwipe.ResetStickyswipe();
	}
}

EEasingFunc::Type SSwipeBox::GetEasing() const
{
	return StickySwipe.GetEasing();
}

void SSwipeBox::SetEasing(EEasingFunc::Type NewEasing)
{
	StickySwipe.SetEasing(NewEasing);
}

float SSwipeBox::GetBlendExp() const
{
	return StickySwipe.GetBlendExp();
}

void SSwipeBox::SetBlendExp(float NewBlendExp)
{
	StickySwipe.SetBlendExp(NewBlendExp);
}

float SSwipeBox::GetSpeed() const
{
	return StickySwipe.GetSpeed();
}

void SSwipeBox::SetSpeed(float NewSpeed)
{
	StickySwipe.SetSpeed(NewSpeed);
}

int SSwipeBox::GetCurrentPage() const
{
	return StickySwipe.GetCurrentPage();
}

void SSwipeBox::SetCurrentPage(int NewPage)
{
	StickySwipe.SetCurrentPage(NewPage);
}

void SSwipeBox::SetAnimateWheelSwipeing(bool bInAnimateWheelSwipeing)
{
	bAnimateWheelSwipeing = bInAnimateWheelSwipeing;
}

void SSwipeBox::SetWheelSwipeMultiplier(float NewWheelSwipeMultiplier)
{
	WheelSwipeMultiplier = NewWheelSwipeMultiplier;
}

void SSwipeBox::SetSwipeWhenFocusChanges(EScrollWhenFocusChanges NewSwipeWhenFocusChanges)
{
	SwipeWhenFocusChanges = NewSwipeWhenFocusChanges;
}

void SSwipeBox::BeginInertialSwipeing()
{
	if (AllowStickySwipe != EAllowStickyswipe::No && !UpdateInertialSwipeHandle.IsValid())
	{
		bIsSwipeing = true;
		bIsSwipeingActiveTimerRegistered = true;
		UpdateInertialSwipeHandle = RegisterActiveTimer(
			0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SSwipeBox::UpdateInertialSwipe));
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

void SSwipeBox::EndInertialSwipeing()
{
	if (AllowStickySwipe != EAllowStickyswipe::No)
	{
	bIsSwipeing = false;
	bIsSwipeingActiveTimerRegistered = false;
	Invalidate(EInvalidateWidget::LayoutAndVolatility);
	if (UpdateInertialSwipeHandle.IsValid())
	{
		UnRegisterActiveTimer(UpdateInertialSwipeHandle.ToSharedRef());
		UpdateInertialSwipeHandle.Reset();
	}

	// Zero the Swipe velocity so the panel stops immediately on mouse down, even if the user does not drag
	InertialSwipeManager.ClearScrollVelocity();
	}
}
