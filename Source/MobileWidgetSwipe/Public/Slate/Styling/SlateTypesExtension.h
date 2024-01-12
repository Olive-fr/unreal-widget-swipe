#pragma once


#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "SlateTypesExtension.generated.h"

/**
 * Represents the appearance of an SSwipeBox
 */
USTRUCT(BlueprintType)
struct MOBILEWIDGETSWIPE_API FSwipeBoxStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FSwipeBoxStyle();

	virtual ~FSwipeBoxStyle() {}

	virtual void GetResources( TArray< const FSlateBrush* >& OutBrushes ) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FSwipeBoxStyle& GetDefault();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	float BarThickness;
	FSwipeBoxStyle& SetBarThickness(float InBarThickness) { BarThickness = InBarThickness; return *this; }

	/** Brush used to draw the top shadow of a SwipeBox */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush TopShadowBrush;
	FSwipeBoxStyle& SetTopShadowBrush( const FSlateBrush& InTopShadowBrush ){ TopShadowBrush = InTopShadowBrush; return *this; }

	/** Brush used to draw the bottom shadow of a SwipeBox */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush BottomShadowBrush;
	FSwipeBoxStyle& SetBottomShadowBrush( const FSlateBrush& InBottomShadowBrush ){ BottomShadowBrush = InBottomShadowBrush; return *this; }

	/** Brush used to draw the left shadow of a SwipeBox */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush LeftShadowBrush;
	FSwipeBoxStyle& SetLeftShadowBrush(const FSlateBrush& InLeftShadowBrush)
	{
		LeftShadowBrush = InLeftShadowBrush;
		return *this;
	}

	/** Brush used to draw the right shadow of a SwipeBox */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush RightShadowBrush;
	FSwipeBoxStyle& SetRightShadowBrush(const FSlateBrush& InRightShadowBrush)
	{
		RightShadowBrush = InRightShadowBrush;
		return *this;
	}

	/** Padding Swipe panel that presents the Swipeed content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FMargin HorizontalSwipeedContentPadding = FMargin(0.0f, 0.0f, 1.0f, 0.0f);
	FSwipeBoxStyle& SetHorizontalSwipeedContentPadding(const FMargin& InPadding)
	{
		HorizontalSwipeedContentPadding = InPadding;
		return *this;
	}

	/** Padding Swipe panel that presents the Swipeed content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FMargin VerticalSwipeedContentPadding = FMargin(0.0f, 0.0f, 0.0f, 1.0f);
	FSwipeBoxStyle& SetVerticalSwipeedContentPadding(const FMargin& InPadding)
	{
		VerticalSwipeedContentPadding = InPadding;
		return *this;
	}

	/**
	 * Unlinks all colors in this style.
	 * @see FSlateColor::Unlink
	 */
	void UnlinkColors()
	{
		TopShadowBrush.UnlinkColors();
		BottomShadowBrush.UnlinkColors();
		LeftShadowBrush.UnlinkColors();
		RightShadowBrush.UnlinkColors();
	}
};


/**
 * Represents the appearance of an SSwipeBar
 */
USTRUCT(BlueprintType)
struct MOBILEWIDGETSWIPE_API FSwipeBarStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FSwipeBarStyle();

	virtual ~FSwipeBarStyle() {}

	virtual void GetResources( TArray< const FSlateBrush* >& OutBrushes ) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	static const FSwipeBarStyle& GetDefault();

	/** Background image to use when the Swipebar is oriented horizontally */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush HorizontalBackgroundImage;
	FSwipeBarStyle& SetHorizontalBackgroundImage( const FSlateBrush& InHorizontalBackgroundImage ){ HorizontalBackgroundImage = InHorizontalBackgroundImage; return *this; }

	/** Background image to use when the Swipebar is oriented vertically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush VerticalBackgroundImage;
	FSwipeBarStyle& SetVerticalBackgroundImage( const FSlateBrush& InVerticalBackgroundImage ){ VerticalBackgroundImage = InVerticalBackgroundImage; return *this; }

	/** The image to use to represent the track above the thumb when the Swipebar is oriented vertically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush VerticalTopSlotImage;
	FSwipeBarStyle& SetVerticalTopSlotImage(const FSlateBrush& Value){ VerticalTopSlotImage = Value; return *this; }

	/** The image to use to represent the track above the thumb when the Swipebar is oriented horizontally */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush HorizontalTopSlotImage;
	FSwipeBarStyle& SetHorizontalTopSlotImage(const FSlateBrush& Value){ HorizontalTopSlotImage = Value; return *this; }

	/** The image to use to represent the track below the thumb when the Swipebar is oriented vertically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush VerticalBottomSlotImage;
	FSwipeBarStyle& SetVerticalBottomSlotImage(const FSlateBrush& Value){ VerticalBottomSlotImage = Value; return *this; }

	/** The image to use to represent the track below the thumb when the Swipebar is oriented horizontally */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush HorizontalBottomSlotImage;
	FSwipeBarStyle& SetHorizontalBottomSlotImage(const FSlateBrush& Value){ HorizontalBottomSlotImage = Value; return *this; }

	/** Image to use when the Swipebar thumb is in its normal state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush NormalThumbImage;
	FSwipeBarStyle& SetNormalThumbImage( const FSlateBrush& InNormalThumbImage ){ NormalThumbImage = InNormalThumbImage; return *this; }

	/** Image to use when the Swipebar thumb is in its hovered state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush HoveredThumbImage;
	FSwipeBarStyle& SetHoveredThumbImage( const FSlateBrush& InHoveredThumbImage ){ HoveredThumbImage = InHoveredThumbImage; return *this; }

	/** Image to use when the Swipebar thumb is in its dragged state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush DraggedThumbImage;
	FSwipeBarStyle& SetDraggedThumbImage( const FSlateBrush& InDraggedThumbImage ){ DraggedThumbImage = InDraggedThumbImage; return *this; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	float Thickness;
	FSwipeBarStyle& SetThickness(float InThickness) { Thickness = InThickness; return *this; }

	/**
	 * Unlinks all colors in this style.
	 * @see FSlateColor::Unlink
	 */
	void UnlinkColors()
	{
		HorizontalBackgroundImage.UnlinkColors();
		VerticalBackgroundImage.UnlinkColors();
		VerticalTopSlotImage.UnlinkColors();
		HorizontalTopSlotImage.UnlinkColors();
		VerticalBottomSlotImage.UnlinkColors();
		HorizontalBottomSlotImage.UnlinkColors();
		NormalThumbImage.UnlinkColors();
		HoveredThumbImage.UnlinkColors();
		DraggedThumbImage.UnlinkColors();
	}
};
