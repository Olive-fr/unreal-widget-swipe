// Copyright Epic Games, Inc. All Rights Reserved.

#include "UMG/Components/SwipeBoxSlot.h"
#include "Components/Widget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SwipeBoxSlot)

/////////////////////////////////////////////////////
// USwipeBoxSlot

USwipeBoxSlot::USwipeBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	Size = FSlateChildSize(ESlateSizeRule::Fill);
}

void USwipeBoxSlot::BuildSlot(TSharedRef<SSwipeBox> SwipeBox)
{
	auto SizeMax = SwipeBox->GetDesiredSize();
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	SwipeBox->AddSlot()
        .Padding(Padding)
        .HAlign(HorizontalAlignment)
        .VAlign(VerticalAlignment)
        .Expose(Slot)
        .MaxSize(SizeMax.X)
        .SizeParam(UWidget::ConvertSerializedSizeParamToRuntime(Size))
	[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
FMargin USwipeBoxSlot::GetPadding() const
{
	return Slot ? Slot->GetPadding() : Padding;
}

void USwipeBoxSlot::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( Slot )
	{
		Slot->SetPadding(InPadding);
	}
}

FSlateChildSize USwipeBoxSlot::GetSize() const
{
	return Size;
}

void USwipeBoxSlot::SetSize(FSlateChildSize InSize)
{
	Size = InSize;
	if (Slot)
	{
		Slot->SetSizeParam(UWidget::ConvertSerializedSizeParamToRuntime(InSize));
	}
}

EHorizontalAlignment USwipeBoxSlot::GetHorizontalAlignment() const
{
	return Slot ? Slot->GetHorizontalAlignment() : HorizontalAlignment.GetValue();
}

void USwipeBoxSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( Slot )
	{
		Slot->SetHorizontalAlignment(InHorizontalAlignment);
	}
}

EVerticalAlignment USwipeBoxSlot::GetVerticalAlignment() const
{
	return Slot ? Slot->GetVerticalAlignment() : VerticalAlignment.GetValue();
}

void USwipeBoxSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if (Slot)
	{
		Slot->SetVerticalAlignment(InVerticalAlignment);
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

void USwipeBoxSlot::SynchronizeProperties()
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	SetPadding(Padding);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	SetSize(Size);
}

void USwipeBoxSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}

