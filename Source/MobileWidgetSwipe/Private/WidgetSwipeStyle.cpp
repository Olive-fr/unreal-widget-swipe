// Copyright Epic Games, Inc. All Rights Reserved.

#include "WidgetSwipeStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/CoreStyle.h"
#include "Slate/Styling/SlateTypesExtension.h"


TSharedPtr< FSlateStyleSet > FWidgetSwipeStyle::WidgetSwipeStyleIconsInstance = NULL;
TSharedPtr< FSlateStyleSet > FWidgetSwipeStyle::WidgetSwipeStyleSlateInstance = NULL;

void FWidgetSwipeStyle::Initialize()
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		if ( !WidgetSwipeStyleIconsInstance.IsValid() )
		{
			WidgetSwipeStyleIconsInstance = CreateIconsStyle();
			FSlateStyleRegistry::RegisterSlateStyle( *WidgetSwipeStyleIconsInstance );
		
		}
	}
#endif

	if ( !WidgetSwipeStyleSlateInstance.IsValid() )
	{
		WidgetSwipeStyleSlateInstance = CreateSlateStyle();
		FSlateStyleRegistry::RegisterSlateStyle( *WidgetSwipeStyleSlateInstance );
	}
}

void FWidgetSwipeStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *WidgetSwipeStyleIconsInstance );
	ensure( WidgetSwipeStyleIconsInstance.IsUnique() );
	WidgetSwipeStyleIconsInstance.Reset();

	FSlateStyleRegistry::UnRegisterSlateStyle( *WidgetSwipeStyleSlateInstance );
	ensure( WidgetSwipeStyleSlateInstance.IsUnique() );
	WidgetSwipeStyleSlateInstance.Reset();
}

FName FWidgetSwipeStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("WidgetSwipeStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush(Style->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

TSharedRef< FSlateStyleSet > FWidgetSwipeStyle::CreateIconsStyle()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("WidgetSwipeStyle"));

		Style->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("MobileWidgetSwipe/Resources/Icons"));
	
	
		Style->Set("ClassIcon.SwipeBox", new IMAGE_BRUSH_SVG(TEXT("SwipeBox"), Icon16x16));
		Style->Set("ClassIcon.ScrollBar", new IMAGE_BRUSH_SVG(TEXT("ScrollBar"), Icon16x16));
	
		Style->Set("ClassThumbnail.SwipeBox", new IMAGE_BRUSH_SVG(TEXT("Swipebox_64"), Icon64x64));
		Style->Set("ClassThumbnail.ScrollBar", new IMAGE_BRUSH_SVG(TEXT("ScrollBar_64"), Icon64x64));
	
	return Style;
}
TSharedRef< FSlateStyleSet > FWidgetSwipeStyle::CreateSlateStyle()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("WidgetSwipeSlateStyle"));
	Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	Style->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	
	Style->Set("SwipeBox", FSwipeBoxStyle()
		.SetTopShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowTop", FVector2f(16.f, 8.f), FMargin(0.5f, 1.f, 0.5f, 0.f)))
		.SetBottomShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowBottom", FVector2f(16.f, 8.f), FMargin(0.5f, 0.f, 0.5f, 1.f)))
		.SetLeftShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowLeft", FVector2f(8.f, 16.f), FMargin(1.f, 0.5f, 0.f, 0.5f)))
		.SetRightShadowBrush(BOX_BRUSH("Common/ScrollBoxShadowRight", FVector2f(8.f, 16.f), FMargin(0.f, 0.5f, 1.f, 0.5f)))
		.SetBarThickness(37.0)
	);
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH

void FWidgetSwipeStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const TSharedPtr<ISlateStyle> FWidgetSwipeStyle::Get()
{
	return WidgetSwipeStyleSlateInstance;
}
