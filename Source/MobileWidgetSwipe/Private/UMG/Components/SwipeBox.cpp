// Copyright Epic Games, Inc. All Rights Reserved.

#include "UMG/Components/SwipeBox.h"
#include "Containers/Ticker.h"
#include "Slate/Styling/DefaultStyleCacheExtension.h"
#include "UMG/Components/SwipeBoxSlot.h"
#include "UObject/EditorObjectVersion.h"
#include "Styling/DefaultStyleCache.h"
#include "Styling/UMGCoreStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SwipeBox)

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// USwipeBox

USwipeBox::USwipeBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , Orientation(Orient_Vertical)
	  , SwipeBarVisibility(ESlateVisibility::Visible)
	  , ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	  , SwipeBarThickness(9.0f, 9.0f)
	  , SwipeBarPadding(2.0f)
	  , AlwaysShowSwipeBar(false)
	  , AlwaysShowSwipeBarTrack(false)
	  , AllowOverSwipe(true)
	  , AllowStickySwipe(true)
	  , BackPadSwipeing(false)
	  , FrontPadSwipeing(false)
	  , NavigationDestination(EDescendantScrollDestination::IntoView)
	  , NavigationSwipePadding(0.0f)
	  , SwipeWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
      , Easing(EEasingFunc::Linear)
      , BlendExp(2.0)
      , Speed(1.0)
{
	bIsVariable = false;

	SetVisibilityInternal(ESlateVisibility::Visible);
	SetClipping(EWidgetClipping::ClipToBounds);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	WidgetStyle = UE::Slate::Private::FDefaultStyleCacheExtension::GetRuntime().GetSwipeBoxStyle();
	WidgetBarStyle = UE::Slate::Private::FDefaultStyleCacheExtension::GetRuntime().GetSwipeBarStyle();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITOR
	if (IsEditorWidget())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		WidgetStyle = UE::Slate::Private::FDefaultStyleCacheExtension::GetEditor().GetSwipeBoxStyle();
		WidgetBarStyle = UE::Slate::Private::FDefaultStyleCacheExtension::GetEditor().GetSwipeBarStyle();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS

		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif // WITH_EDITOR
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	bAllowRightClickDragSwipeing = true;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void USwipeBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MySwipeBox.Reset();
}

UClass* USwipeBox::GetSlotClass() const
{
	return USwipeBoxSlot::StaticClass();
}

void USwipeBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MySwipeBox.IsValid())
	{
		CastChecked<USwipeBoxSlot>(InSlot)->BuildSlot(MySwipeBox.ToSharedRef());
	}
}

void USwipeBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MySwipeBox.IsValid() && InSlot->Content)
	{
		const TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MySwipeBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedRef<SWidget> USwipeBox::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MySwipeBox = SNew(SSwipeBox)
		.Style(&WidgetStyle)
		.SwipeBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationSwipePadding(NavigationSwipePadding)
		.SwipeWhenFocusChanges(SwipeWhenFocusChanges)
		.SlotStretchChildAsParentSize(SlotFillChildAsParentSize)
		.BackPadSwipeing(BackPadSwipeing)
		.FrontPadSwipeing(FrontPadSwipeing)
		.AnimateWheelSwipeing(bAnimateWheelSwipeing)
		.WheelSwipeMultiplier(WheelSwipeMultiplier)
		.Speed(Speed)
		.Easing(Easing)
		.BlendExp(BlendExp)
		.OnUserSwipeed(BIND_UOBJECT_DELEGATE(FOnUserSwipeed, SlateHandleUserSwipeed));
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	for (UPanelSlot* PanelSlot : Slots)
	{
		if (USwipeBoxSlot* TypedSlot = Cast<USwipeBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MySwipeBox.ToSharedRef());
		}
	}

	return MySwipeBox.ToSharedRef();
}

void USwipeBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MySwipeBox.IsValid())
	{
		return;
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MySwipeBox->SetSwipeOffset(DesiredSwipeOffset);
	MySwipeBox->SetOrientation(Orientation);
	MySwipeBox->SetSwipeBarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(SwipeBarVisibility));
	MySwipeBox->SetSwipeBarThickness(SwipeBarThickness);
	MySwipeBox->SetSwipeBarPadding(SwipeBarPadding);
	MySwipeBox->SetSwipeBarAlwaysVisible(AlwaysShowSwipeBar);
	MySwipeBox->SetSwipeBarTrackAlwaysVisible(AlwaysShowSwipeBarTrack);
	MySwipeBox->SetAllowOverSwipe(AllowOverSwipe ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	MySwipeBox->SetSwipeBarRightClickDragAllowed(bAllowRightClickDragSwipeing);
	MySwipeBox->SetAllowStickySwipe(AllowStickySwipe ? EAllowStickyswipe::Yes : EAllowStickyswipe::No);
	MySwipeBox->SetConsumeMouseWheel(ConsumeMouseWheel);
	MySwipeBox->SetAnimateWheelSwipeing(bAnimateWheelSwipeing);
	MySwipeBox->SetWheelSwipeMultiplier(WheelSwipeMultiplier);
	MySwipeBox->SetStyle(&WidgetStyle);
	MySwipeBox->InvalidateStyle();
	MySwipeBox->SetSwipeBarStyle(&WidgetBarStyle);
	MySwipeBox->InvalidateSwipeBarStyle();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

float USwipeBox::GetSwipeOffset() const
{
	if (MySwipeBox.IsValid())
	{
		return MySwipeBox->GetSwipeOffset();
	}

	return 0;
}

float USwipeBox::GetSwipeOffsetOfEnd() const
{
	if (MySwipeBox.IsValid())
	{
		return MySwipeBox->GetSwipeOffsetOfEnd();
	}

	return 0;
}

float USwipeBox::GetViewFraction() const
{
	if (MySwipeBox.IsValid())
	{
		return MySwipeBox->GetViewFraction();
	}

	return 0;
}

float USwipeBox::GetViewOffsetFraction() const
{
	if (MySwipeBox.IsValid())
	{
		return MySwipeBox->GetViewOffsetFraction();
	}

	return 0;
}

void USwipeBox::SetSwipeOffset(float NewSwipeOffset)
{
	DesiredSwipeOffset = NewSwipeOffset;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetSwipeOffset(NewSwipeOffset);
	}
}

void USwipeBox::SwipeToStart()
{
	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SwipeToStart();
	}
}

void USwipeBox::SwipeToEnd()
{
	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SwipeToEnd();
	}
}

void USwipeBox::SwipeWidgetIntoView(UWidget* WidgetToFind, bool AnimateSwipe,
                                    EDescendantScrollDestination InSwipeDestination, float Padding)
{
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MySwipeBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to Swipe which is necessary to avoid warnings/ensures 
		//       when we request to Swipe to a widget and later remove that widget!
		MySwipeBox->SwipeDescendantIntoView(SlateWidgetToFind, AnimateSwipe, InSwipeDestination, Padding);
	}
}

#if WITH_EDITORONLY_DATA

void USwipeBox::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);

	const bool bDeprecateThickness = Ar.IsLoading() && Ar.CustomVer(FEditorObjectVersion::GUID) <
		FEditorObjectVersion::ScrollBarThicknessChange;
	if (bDeprecateThickness)
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		// Set SwipeBarThickness property to previous default value.
		SwipeBarThickness.Set(5.0f, 5.0f);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	Super::Serialize(Ar);

	if (bDeprecateThickness)
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		// Implicit padding of 2 was removed, so SwipeBarThickness value must be incremented by 4.
		SwipeBarThickness += FVector2D(4.0f, 4.0f);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}
}

#endif // if WITH_EDITORONLY_DATA

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void USwipeBox::SetWidgetStyle(const FSwipeBoxStyle& NewWidgetStyle)
{
	WidgetStyle = NewWidgetStyle;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->InvalidateStyle();
	}
}

const FSwipeBoxStyle& USwipeBox::GetWidgetStyle() const
{
	return WidgetStyle;
}

void USwipeBox::SetWidgetBarStyle(const FSwipeBarStyle& NewWidgetBarStyle)
{
	WidgetBarStyle = NewWidgetBarStyle;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->InvalidateSwipeBarStyle();
	}
}

const FSwipeBarStyle& USwipeBox::GetWidgetBarStyle() const
{
	return WidgetBarStyle;
}

void USwipeBox::SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination)
{
	NavigationDestination = NewNavigationDestination;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetNavigationDestination(NewNavigationDestination);
	}
}

EDescendantScrollDestination USwipeBox::GetNavigationDestination() const
{
	return NavigationDestination;
}

void USwipeBox::SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel)
{
	ConsumeMouseWheel = NewConsumeMouseWheel;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetConsumeMouseWheel(NewConsumeMouseWheel);
	}
}

EConsumeMouseWheel USwipeBox::GetConsumeMouseWheel() const
{
	return ConsumeMouseWheel;
}

void USwipeBox::SetOrientation(EOrientation NewOrientation)
{
	Orientation = NewOrientation;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetOrientation(Orientation);
	}
}

EOrientation USwipeBox::GetOrientation() const
{
	return Orientation;
}

void USwipeBox::SetSwipeBarVisibility(ESlateVisibility NewSwipeBarVisibility)
{
	SwipeBarVisibility = NewSwipeBarVisibility;

	if (MySwipeBox.IsValid())
	{
		switch (SwipeBarVisibility)
		{
		case ESlateVisibility::Collapsed: MySwipeBox->SetSwipeBarVisibility(EVisibility::Collapsed);
			break;
		case ESlateVisibility::Hidden: MySwipeBox->SetSwipeBarVisibility(EVisibility::Hidden);
			break;
		case ESlateVisibility::HitTestInvisible: MySwipeBox->SetSwipeBarVisibility(EVisibility::HitTestInvisible);
			break;
		case ESlateVisibility::SelfHitTestInvisible: MySwipeBox->SetSwipeBarVisibility(
				EVisibility::SelfHitTestInvisible);
			break;
		case ESlateVisibility::Visible: MySwipeBox->SetSwipeBarVisibility(EVisibility::Visible);
			break;
		}
	}
}

ESlateVisibility USwipeBox::GetSwipeBarVisibility() const
{
	return SwipeBarVisibility;
}

void USwipeBox::SetSwipeBarThickness(const FVector2D& NewSwipeBarThickness)
{
	SwipeBarThickness = NewSwipeBarThickness;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetSwipeBarThickness(SwipeBarThickness);
	}
}

FVector2D USwipeBox::GetSwipeBarThickness() const
{
	return SwipeBarThickness;
}

void USwipeBox::SetSwipeBarPadding(const FMargin& NewSwipeBarPadding)
{
	SwipeBarPadding = NewSwipeBarPadding;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetSwipeBarPadding(SwipeBarPadding);
	}
}

FMargin USwipeBox::GetSwipeBarPadding() const
{
	return SwipeBarPadding;
}

void USwipeBox::SetAlwaysShowSwipeBar(bool NewAlwaysShowSwipeBar)
{
	AlwaysShowSwipeBar = NewAlwaysShowSwipeBar;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetSwipeBarAlwaysVisible(AlwaysShowSwipeBar);
	}
}

bool USwipeBox::IsAlwaysShowSwipeBar() const
{
	return AlwaysShowSwipeBar;
}

void USwipeBox::SetAllowOverSwipe(bool NewAllowOverSwipe)
{
	AllowOverSwipe = NewAllowOverSwipe;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetAllowOverSwipe(AllowOverSwipe ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	}
}

bool USwipeBox::IsAllowOverSwipe() const
{
	return AllowOverSwipe;
}

void USwipeBox::SetAllowStickySwipe(bool NewAllowStickySwipe)
{
	AllowStickySwipe = NewAllowStickySwipe;

	if (MySwipeBox.IsValid())
	{
		MySwipeBox->SetAllowStickySwipe(AllowStickySwipe ? EAllowStickyswipe::Yes : EAllowStickyswipe::No);
	}
}

bool USwipeBox::IsAllowStickySwipe() const
{
	return AllowStickySwipe;
}

bool USwipeBox::IsSlotFillChildAsParentSize() const
{
	return SlotFillChildAsParentSize;
}

void USwipeBox::SetAnimateWheelSwipeing(bool bShouldAnimateWheelSwipeing)
{
	bAnimateWheelSwipeing = bShouldAnimateWheelSwipeing;
	if (MySwipeBox)
	{
		MySwipeBox->SetAnimateWheelSwipeing(bShouldAnimateWheelSwipeing);
	}
}

bool USwipeBox::IsAnimateWheelSwipeing() const
{
	return bAnimateWheelSwipeing;
}

EEasingFunc::Type USwipeBox::GetEasing() const
{
	return Easing;
}

void USwipeBox::SetEasing(EEasingFunc::Type NewEasing)
{
	this->Easing = NewEasing;
	if (MySwipeBox)
	{
		MySwipeBox->SetEasing(NewEasing);
	}
}

float USwipeBox::GetBlendExp() const
{
	return BlendExp;
}

void USwipeBox::SetBlendExp(float NewBlendExp)
{
	this->BlendExp = NewBlendExp;
	if (MySwipeBox)
	{
		MySwipeBox->SetBlendExp(NewBlendExp);
	}
}

float USwipeBox::GetSpeed() const
{
	return Speed;
}

void USwipeBox::SetSpeed(float NewSpeed)
{
	this->Speed = NewSpeed;
	if (MySwipeBox)
	{
		MySwipeBox->SetSpeed(NewSpeed);
	}
}

int USwipeBox::GetCurrentPage() const
{
	if (MySwipeBox)
	{
		return MySwipeBox->GetCurrentPage();
	}
	return -1;
}

void USwipeBox::SetCurrentPage(int32 NewCurrentPage)
{
	if (MySwipeBox)
	{
		MySwipeBox->SetCurrentPage(NewCurrentPage);
	}
}

void USwipeBox::SetWheelSwipeMultiplier(float NewWheelSwipeMultiplier)
{
	WheelSwipeMultiplier = NewWheelSwipeMultiplier;
	if (MySwipeBox)
	{
		MySwipeBox->SetWheelSwipeMultiplier(NewWheelSwipeMultiplier);
	}
}

float USwipeBox::GetWheelSwipeMultiplier() const
{
	return WheelSwipeMultiplier;
}

void USwipeBox::SetSwipeWhenFocusChanges(EScrollWhenFocusChanges NewSwipeWhenFocusChanges)
{
	SwipeWhenFocusChanges = NewSwipeWhenFocusChanges;
	if (MySwipeBox)
	{
		MySwipeBox->SetSwipeWhenFocusChanges(NewSwipeWhenFocusChanges);
	}
}

EScrollWhenFocusChanges USwipeBox::GetSwipeWhenFocusChanges() const
{
	return SwipeWhenFocusChanges;
}

void USwipeBox::EndInertialSwipeing()
{
	if (MySwipeBox.IsValid())
	{
		MySwipeBox->EndInertialSwipeing();
	}
}

void USwipeBox::SetAlwaysShowSwipeBarTrack(bool NewAlwaysShowSwipeBarTrack)
{
	AlwaysShowSwipeBarTrack = NewAlwaysShowSwipeBarTrack;
	if (MySwipeBox)
	{
		MySwipeBox->SetSwipeBarTrackAlwaysVisible(AlwaysShowSwipeBarTrack);
	}
}

bool USwipeBox::IsAlwaysShowSwipeBarTrack() const
{
	return AlwaysShowSwipeBarTrack;
}

float USwipeBox::GetNavigationSwipePadding() const
{
	return NavigationSwipePadding;
}

void USwipeBox::SetAllowRightClickDragSwipeing(bool bShouldAllowRightClickDragSwipeing)
{
	AlwaysShowSwipeBarTrack = bShouldAllowRightClickDragSwipeing;
	if (MySwipeBox)
	{
		MySwipeBox->SetSwipeBarRightClickDragAllowed(AlwaysShowSwipeBarTrack);
	}
}

bool USwipeBox::IsAllowRightClickDragSwipeing() const
{
	return AlwaysShowSwipeBarTrack;
}

bool USwipeBox::IsFrontPadSwipeing() const
{
	return FrontPadSwipeing;
}

bool USwipeBox::IsBackPadSwipeing() const
{
	return BackPadSwipeing;
}

void USwipeBox::InitBackPadSwipeing(bool InBackPadSwipeing)
{
	ensureMsgf(!MySwipeBox.IsValid(), TEXT("The widget is already created."));
	BackPadSwipeing = InBackPadSwipeing;
}

void USwipeBox::InitFrontPadSwipeing(bool InFrontPadSwipeing)
{
	ensureMsgf(!MySwipeBox.IsValid(), TEXT("The widget is already created."));
	FrontPadSwipeing = InFrontPadSwipeing;
}

void USwipeBox::InitNavigationSwipePadding(float InNavigationSwipePadding)
{
	ensureMsgf(!MySwipeBox.IsValid(), TEXT("The widget is already created."));
	NavigationSwipePadding = InNavigationSwipePadding;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS

void USwipeBox::SlateHandleUserSwipeed(float CurrentOffset)
{
	OnUserSwipeed.Broadcast(CurrentOffset);
}

#if WITH_EDITOR

const FText USwipeBox::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

void USwipeBox::OnDescendantSelectedByDesigner(UWidget* DescendantWidget)
{
	UWidget* SelectedChild = UWidget::FindChildContainingDescendant(this, DescendantWidget);
	if (SelectedChild)
	{
		SwipeWidgetIntoView(SelectedChild, true);

		if (TickHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
			TickHandle.Reset();
		}
	}
}

void USwipeBox::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	// because we get a deselect before we get a select, we need to delay this call until we're sure we didn't Swipe to another widget.
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) -> bool
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_USwipeBox_SwipeToStart_LambdaTick);
		this->SwipeToStart();
		return false;
	}));
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
