// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SwipeBox.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Layout/Margin.h"
#include "Components/SlateWrapperTypes.h"
#include "Slate/Widgets/Layout/SSwipeBox.h"
#include "Components/PanelSlot.h"

#include "SwipeBoxSlot.generated.h"

/** The Slot for the USwipeBox, contains the widget that are Swipeable */
UCLASS()
class MOBILEWIDGETSWIPE_API USwipeBoxSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()

private:

	/** How much space this slot should occupy in the direction of the panel. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = "Layout|SwipeBox Slot", meta = (AllowPrivateAccess = "true", DisplayAfter = "Padding"))
	FSlateChildSize Size;

public:
	
	UE_DEPRECATED(5.1, "Direct access to Padding is deprecated. Please use the getter or setter.")
	/** The padding area between the slot and the content it contains. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layout|SwipeBox Slot")
	FMargin Padding;

	UE_DEPRECATED(5.1, "Direct access to HorizontalAlignment is deprecated. Please use the getter or setter.")
	/** The alignment of the object horizontally. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layout|SwipeBox Slot")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	UE_DEPRECATED(5.1, "Direct access to VerticalAlignment is deprecated. Please use the getter or setter.")
	/** The alignment of the object vertically. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|SwipeBox Slot")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;

public:

	FMargin GetPadding() const;

	UFUNCTION(BlueprintCallable, Category="Layout|SwipeBox Slot")
	void SetPadding(FMargin InPadding);

	FSlateChildSize GetSize() const;

	void SetSize(FSlateChildSize InSize);

	EHorizontalAlignment GetHorizontalAlignment() const;

	UFUNCTION(BlueprintCallable, Category="Layout|SwipeBox Slot")
	void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment);

	EVerticalAlignment GetVerticalAlignment() const;

	UFUNCTION(BlueprintCallable, Category = "Layout|SwipeBox Slot")
	void SetVerticalAlignment(EVerticalAlignment InVerticalAlignment);

public:

	//~ UPanelSlot interface
	virtual void SynchronizeProperties() override;
	//~ End of UPanelSlot interface

	/** Builds the underlying FSlot for the Slate layout panel. */
	void BuildSlot(TSharedRef<SSwipeBox> SwipeBox);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:

	/** A raw pointer to the slot to allow us to adjust the size, padding...etc at runtime. */
	SSwipeBox::FSlot* Slot;
};
