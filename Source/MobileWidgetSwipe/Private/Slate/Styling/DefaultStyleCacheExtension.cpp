// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slate/Styling/DefaultStyleCacheExtension.h"

#include "Misc/LazySingleton.h"
#include "Styling/SlateTypes.h"
#include "Styling/UMGCoreStyle.h"

#if WITH_EDITOR
#include "Styling/CoreStyle.h"
#endif

namespace UE::Slate::Private
{

FDefaultStyleCacheExtension& FDefaultStyleCacheExtension::Get()
{
	return TLazySingleton<FDefaultStyleCacheExtension>::Get();
}

FDefaultStyleCacheExtension::FDefaultStyleCacheExtension()
{
	if (!IsRunningDedicatedServer())
	{
		Runtime.SwipeBarStyle = FUMGCoreStyle::Get().GetWidgetStyle<FSwipeBarStyle>("SwipeBar");
		Runtime.SwipeBarStyle.UnlinkColors();

		Runtime.SwipeBoxStyle = FUMGCoreStyle::Get().GetWidgetStyle<FSwipeBoxStyle>("SwipeBox");
		Runtime.SwipeBoxStyle.UnlinkColors();


#if WITH_EDITOR
		Editor.SwipeBarStyle = FCoreStyle::Get().GetWidgetStyle<FSwipeBarStyle>("SwipeBar");
		Editor.SwipeBarStyle.UnlinkColors();

		Editor.SwipeBoxStyle = FCoreStyle::Get().GetWidgetStyle<FSwipeBoxStyle>("SwipeBox");
		Editor.SwipeBoxStyle.UnlinkColors();
#endif
	}
}

} //namespace UE::Slate::Private
