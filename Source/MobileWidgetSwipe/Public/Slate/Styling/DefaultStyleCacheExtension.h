// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateTypesExtension.h"

class FLazySingleton;

namespace UE::Slate::Private
{

/**
 * Single point of access for various default styles used in UMG for runtime and editor
 * with each style already having it's colors unlinked for reuse
 */
struct MOBILEWIDGETSWIPE_API FDefaultStyleCacheExtension
{
	/**
	 * Internal style default holder struct, used so that one can get a bundle of defaults relevant for runtime or editor
	 */
	struct FStyles
	{

		const FSwipeBoxStyle& GetSwipeBoxStyle() const { return SwipeBoxStyle; };
		const FSwipeBarStyle& GetSwipeBarStyle() const { return SwipeBarStyle; };


	private:
		friend FDefaultStyleCacheExtension;

		FSwipeBoxStyle SwipeBoxStyle;
		FSwipeBarStyle SwipeBarStyle;

		TMap<UScriptStruct*, FSlateWidgetStyle*> TypeInstanceMap;
	};

	/** Gets singleton and returns runtime styles from singleton */
	static const FStyles& GetRuntime() { return Get().Runtime; }
#if WITH_EDITOR
	/** Gets singleton and returns editor styles from singleton */
	static const FStyles& GetEditor() { return Get().Editor; }
#endif

private:

	/** Singleton getter, however private used since GetRuntime & GetEditor is preferred for styles */
	static FDefaultStyleCacheExtension& Get();

	friend ::FLazySingleton;

	FDefaultStyleCacheExtension();

	FStyles Runtime;

#if WITH_EDITOR
	FStyles Editor;
#endif
};

} //namespace UE::Slate::Private
