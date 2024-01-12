// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**  */
class FWidgetSwipeStyle
{
public:

	static MOBILEWIDGETSWIPE_API void Initialize();

	static MOBILEWIDGETSWIPE_API void Shutdown();

	/** reloads textures used by slate renderer */
	static MOBILEWIDGETSWIPE_API void ReloadTextures();

	/** @return The Slate style set for the UMG Style */
	static MOBILEWIDGETSWIPE_API const ISlateStyle& Get();

	static MOBILEWIDGETSWIPE_API FName GetStyleSetName();

private:

	static TSharedRef< class FSlateStyleSet > Create();

private:

	static TSharedPtr< class FSlateStyleSet > WidgetSwipeStyleInstance;
};
