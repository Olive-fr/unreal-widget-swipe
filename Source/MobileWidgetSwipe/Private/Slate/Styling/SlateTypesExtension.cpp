// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Styling/SlateTypesExtension.h"
#include "Brushes/SlateNoResource.h"
#include "Styling/StyleColors.h"
#include "Widgets/InvalidateWidgetReason.h"
#include "Widgets/SWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SlateTypesExtension)


FSwipeBarStyle::FSwipeBarStyle()
	: HorizontalBackgroundImage(FSlateNoResource())
	, VerticalBackgroundImage(FSlateNoResource())
	, VerticalTopSlotImage(FSlateNoResource())
	, HorizontalTopSlotImage(FSlateNoResource())
	, VerticalBottomSlotImage(FSlateNoResource())
	, HorizontalBottomSlotImage(FSlateNoResource())
	, NormalThumbImage(FSlateNoResource())
	, HoveredThumbImage(FSlateNoResource())
	, DraggedThumbImage(FSlateNoResource())
	, Thickness(16.0f)
{
}

void FSwipeBarStyle::GetResources( TArray< const FSlateBrush* >& OutBrushes ) const
{
	OutBrushes.Add( &HorizontalBackgroundImage );
	OutBrushes.Add( &VerticalBackgroundImage );
	OutBrushes.Add( &VerticalTopSlotImage);
	OutBrushes.Add( &HorizontalTopSlotImage);
	OutBrushes.Add( &VerticalBottomSlotImage);
	OutBrushes.Add( &HorizontalBottomSlotImage);
	OutBrushes.Add( &NormalThumbImage );
	OutBrushes.Add( &HoveredThumbImage );
	OutBrushes.Add( &DraggedThumbImage );
}

const FName FSwipeBarStyle::TypeName( TEXT("FSwipeBarStyle") );

const FSwipeBarStyle& FSwipeBarStyle::GetDefault()
{
	static FSwipeBarStyle Default;
	return Default;
}



FSwipeBoxStyle::FSwipeBoxStyle()
	: BarThickness(9.0f)
{
}

void FSwipeBoxStyle::GetResources( TArray< const FSlateBrush* >& OutBrushes ) const
{
	OutBrushes.Add( &TopShadowBrush );
	OutBrushes.Add( &BottomShadowBrush );
	OutBrushes.Add( &LeftShadowBrush );
	OutBrushes.Add( &RightShadowBrush );
}

const FName FSwipeBoxStyle::TypeName( TEXT("FSwipeBoxStyle") );

const FSwipeBoxStyle& FSwipeBoxStyle::GetDefault()
{
	static FSwipeBoxStyle Default;
	return Default;
}
