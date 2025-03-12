// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "UI_DynamicCrosshair.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API UUI_DynamicCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;

public:

	void SetCrosshairSpread(float Spread);

protected:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	UPROPERTY(meta = (BindWidget))
	UBorder* TopCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* BottomCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* LeftCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* RightCrosshair;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairThickness;

private:

	void InitializeCrosshair(UBorder* Crosshair);
	void SetCrosshair(UBorder* Crosshair, FVector2D InSize, FVector2D InPos);
};
