// Copyright Epic Games, Inc. All Rights Reserved.

#include "WidgetSwipeStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"


TSharedPtr< FSlateStyleSet > FWidgetSwipeStyle::WidgetSwipeStyleInstance = NULL;

void FWidgetSwipeStyle::Initialize()
{
	if ( !WidgetSwipeStyleInstance.IsValid() )
	{
		WidgetSwipeStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle( *WidgetSwipeStyleInstance );
	}
}

void FWidgetSwipeStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *WidgetSwipeStyleInstance );
	ensure( WidgetSwipeStyleInstance.IsUnique() );
	WidgetSwipeStyleInstance.Reset();
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

TSharedRef< FSlateStyleSet > FWidgetSwipeStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("WidgetSwipeStyle"));
	Style->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("MobileWidgetSwipe/Resources/Icons"));
	
	
	Style->Set("ClassIcon.SwipeBox", new IMAGE_BRUSH_SVG(TEXT("SwipeBox"), Icon16x16));
	Style->Set("ClassIcon.ScrollBar", new IMAGE_BRUSH_SVG(TEXT("ScrollBar"), Icon16x16));
	
	Style->Set("ClassThumbnail.SwipeBox", new IMAGE_BRUSH_SVG(TEXT("Swipebox_64"), Icon64x64));
	Style->Set("ClassThumbnail.ScrollBar", new IMAGE_BRUSH_SVG(TEXT("ScrollBar_64"), Icon64x64));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH

void FWidgetSwipeStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FWidgetSwipeStyle::Get()
{
	return *WidgetSwipeStyleInstance;
}
