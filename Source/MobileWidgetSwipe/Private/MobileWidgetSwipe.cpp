// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobileWidgetSwipe.h"

#include "WidgetSwipeStyle.h"

#define LOCTEXT_NAMESPACE "FMobileWidgetSwipeModule"

void FMobileWidgetSwipeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	if (GIsEditor)
	{
		FWidgetSwipeStyle::Initialize();
	}
#endif
}

void FMobileWidgetSwipeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	#if WITH_EDITOR
	if (GIsEditor)
	{
		FWidgetSwipeStyle::Shutdown();
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMobileWidgetSwipeModule, MobileWidgetSwipe)