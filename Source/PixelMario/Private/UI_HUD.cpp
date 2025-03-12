// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_HUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PlayerCharacter.h"
#include "Inventory.h"


void UUI_HUD::BindToInventory(AInventory* In_Inventory)
{
	if (In_Inventory)
	{
		In_Inventory->OnInventoryUpdated.AddUObject(this, &UUI_HUD::UpdateInventoryUI);
		In_Inventory->OnInventoryIndexChanged.AddUObject(this, &UUI_HUD::UpdateSlotIndex);

		Inventory = In_Inventory;

		UpdateInventoryUI();
		UpdateSlotIndex(Inventory->GetCurrentInventoryIndex());
	}
}

void UUI_HUD::BindToCharacter(APlayerCharacter* Character)
{
	if (Character)
	{
		Character->OnHPChanged.AddUObject(this, &UUI_HUD::UpdateHPBar);

		UpdateHPBar(Character->CurrentHP / Character->MaxHP);
	}
}

void UUI_HUD::NativeConstruct()
{
	Super::NativeConstruct();

	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("BP_HPBar")));
	HPBar->SetPercent(1.0f);


	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot1_Txt"))));
	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot2_Txt"))));
	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot3_Txt"))));
	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot4_Txt"))));
	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot5_Txt"))));
	ItemNames.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("Slot6_Txt"))));

	for (UTextBlock* ItemName : ItemNames)
	{
		if (ItemName)
		{
			ItemName->SetText(FText::FromString(TEXT("0")));
		}
	}



	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot1_Img"))));
	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot2_Img"))));
	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot3_Img"))));
	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot4_Img"))));
	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot5_Img"))));
	ItemImages.Add(Cast<UImage>(GetWidgetFromName(TEXT("Slot6_Img"))));


	for (UImage* ItemImage : ItemImages)
	{
		if (ItemImage)
		{
			FSlateBrush TransparentBrush;
			TransparentBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
			ItemImage->SetBrush(TransparentBrush);
		}
	}


	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector1_Img"))));
	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector2_Img"))));
	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector3_Img"))));
	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector4_Img"))));
	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector5_Img"))));
	SlotSelectors.Add(Cast<UImage>(GetWidgetFromName(TEXT("SlotSelector6_Img"))));


}

void UUI_HUD::UpdateInventoryUI()
{

	if (!Inventory.IsValid())  // Inventory가 유효하지 않을 시
	{
		// 모든 슬롯에 대해 No Item으로 설정
		for (UTextBlock* ItemName : ItemNames)
		{
			if (ItemName)
			{
				ItemName->SetText(FText::FromString(TEXT("0")));
			}
		}

		for (UImage* ItemImage : ItemImages)
		{
			if (ItemImage)
			{
				FSlateBrush TransparentBrush;
				TransparentBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
				ItemImage->SetBrush(TransparentBrush);
			}
		}

		return;
	}

	const TArray<TSubclassOf<AItem>>& Items = Inventory->Items;
	const TMap<FString, FItemInfo>& ItemInfos = Inventory->ItemInfos;

	
	for (int32 i = 0; i < ItemNames.Num(); ++i)
	{
		// 아이템이 유효한 인덱스인지 체크
		if (Items.IsValidIndex(i) && Items[i] != nullptr)
		{
			FString ItemName = Items[i]->GetName();
			if (ItemInfos.Contains(ItemName))
			{
				int32 Quantity = ItemInfos[ItemName].Quantity;
				FString ItemText = FString::Printf(TEXT("%d"), Quantity);
				ItemNames[i]->SetText(FText::FromString(ItemText));
			}
			else  // ItemInfos에 해당 아이템이 없는 경우
			{
				ItemNames[i]->SetText(FText::FromString(TEXT("0")));
			}
		}
		else  // 유효하지 않은 아이템이거나 아이템이 없는 경우
		{
			ItemNames[i]->SetText(FText::FromString(TEXT("0")));
		}
	}



	for (int32 i = 0; i < ItemImages.Num(); ++i)
	{
		if (Items.IsValidIndex(i) && Items[i] != nullptr)
		{
			FString ItemName = Items[i]->GetName();
			if (ItemInfos.Contains(ItemName))
			{
				UTexture2D* ItemTexture = ItemInfos[ItemName].ItemTexture;
				if (ItemTexture != nullptr)
				{
					FSlateBrush TextureBrush;
					TextureBrush.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
					ItemImages[i]->SetBrush(TextureBrush);
					ItemImages[i]->SetBrushFromTexture(ItemTexture);
				}

				else
				{
					FSlateBrush TransparentBrush;
					TransparentBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
					ItemImages[i]->SetBrush(TransparentBrush);
				}
			}

			else
			{
				FSlateBrush TransparentBrush;
				TransparentBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
				ItemImages[i]->SetBrush(TransparentBrush);
			}
		}
		else
		{
			FSlateBrush TransparentBrush;
			TransparentBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
			ItemImages[i]->SetBrush(TransparentBrush);
		}
	}
	
}

void UUI_HUD::UpdateHPBar(float HPPercentage)
{
	if (HPBar)
	{
		HPBar->SetPercent(HPPercentage);
	}
}

void UUI_HUD::UpdateSlotIndex(int Index)
{
	for (int32 i = 0; i < SlotSelectors.Num(); ++i)
	{
		if (SlotSelectors[i])
		{
			if (i == Index)
			{
				SlotSelectors[i]->SetVisibility(ESlateVisibility::Visible);
			}

			else
			{
				SlotSelectors[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

}
