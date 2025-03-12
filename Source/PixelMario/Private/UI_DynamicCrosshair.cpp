// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_DynamicCrosshair.h"

void UUI_DynamicCrosshair::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeCrosshair(TopCrosshair);
    InitializeCrosshair(BottomCrosshair);
    InitializeCrosshair(LeftCrosshair);
    InitializeCrosshair(RightCrosshair);

    CrosshairThickness = 4.0f;
    CrosshairLength = 20.0f;
}



void UUI_DynamicCrosshair::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
    Super::NativeTick(Geometry, InDeltaTime);

    FVector2D CrosshairSize; 
    FVector2D CrosshairPos; 

    CrosshairSize = FVector2D(CrosshairThickness, CrosshairLength);
    CrosshairPos = FVector2D((-CrosshairThickness / 2), (CrosshairSpread));
    SetCrosshair(BottomCrosshair, CrosshairSize, CrosshairPos);


    CrosshairSize = FVector2D(CrosshairThickness, CrosshairLength);
    CrosshairPos = FVector2D((-CrosshairThickness / 2), (-CrosshairLength - CrosshairSpread));
    SetCrosshair(TopCrosshair, CrosshairSize, CrosshairPos);


    CrosshairSize = FVector2D(CrosshairLength, CrosshairThickness);
    CrosshairPos = FVector2D((-CrosshairLength - CrosshairSpread), (-CrosshairThickness / 2));
    SetCrosshair(LeftCrosshair, CrosshairSize, CrosshairPos);


    CrosshairSize = FVector2D(CrosshairLength, CrosshairThickness);
    CrosshairPos = FVector2D((CrosshairSpread), (-CrosshairThickness / 2));
    SetCrosshair(RightCrosshair, CrosshairSize, CrosshairPos);
   
}

void UUI_DynamicCrosshair::SetCrosshairSpread(float Spread)
{
    CrosshairSpread = Spread;
}

void UUI_DynamicCrosshair::InitializeCrosshair(UBorder* Crosshair)
{
    if (Crosshair)
    {
        Crosshair->SetBrushColor(FLinearColor::White);
        Crosshair->SetPadding(1.0f);

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot))
        {
            // 앵커 설정
            CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f)); // 중심을 기준으로 설정

            // 위치 설정
            CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f)); // 중심 위치

            // 크기 설정
            CanvasSlot->SetSize(FVector2D(0.0f, 0.0f)); // 너비와 높이
        }
    }
}


void UUI_DynamicCrosshair::SetCrosshair(UBorder* Crosshair, FVector2D InSize, FVector2D InPos)
{
    if (Crosshair)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot))
        {
            CanvasSlot->SetSize(InSize);
            CanvasSlot->SetPosition(InPos);
        }
    }
}
