// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "WeaponItem.h"

// Sets default values
AInventory::AInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxInventorySize = 6;
	CurrentItemNum = 0;
	CurrentInventoryIndex = 0;

	Items.SetNum(MaxInventorySize);			// 기본적으로 아이템 배열은 nullptr로 초기화
}

int AInventory::AddItem(AItem* NewItem)
{
	if (NewItem == nullptr) return INDEX_NONE;

	TSubclassOf<AItem> NewItemClass = NewItem->GetClass();
	FString NewItemName = NewItemClass->GetName();
	FItemInfo* Info = ItemInfos.Find(NewItemName);

	// 해당 아이템이 인벤토리에 존재하지 않았다면
	if (Info == nullptr)
	{
		int32 EmptySlot = FindEmptySlot();

		if (EmptySlot != INDEX_NONE)  // 빈 공간이 있으면
		{
			Items[EmptySlot] = NewItemClass;

			FItemInfo NewItemInfo;
			NewItemInfo.Quantity = 1;
			NewItemInfo.ItemTexture = NewItem->GetItemTexture();
			ItemInfos.Add(NewItemName, NewItemInfo);

			CurrentItemNum++;

			NotifyInventoryUpdated();
			return EmptySlot;
		}

		else
		{
			// 인벤토리가 꽉 찼을 경우
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("Inventory is full"));
			return INDEX_NONE;
		}
	}

	else  // 해당 아이템이 이미 인벤토리에 존재한다면
	{
		// 아이템 수량 증가
		ItemInfos[NewItemName].Quantity++;

		NotifyInventoryUpdated();


		int ItemIndex = Items.Find(NewItemClass);

		return ItemIndex;
	}

	// 아이템 추가 실패
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Adding Item failed"));
	return INDEX_NONE;
}


bool AInventory::RemoveItemByIndex(int Index)
{
	if (Items.IsValidIndex(Index))				// 인덱스의 유효성 체크
	{				
		TSubclassOf<AItem> ItemClass = Items[Index];
		
		if (ItemClass)										// 아이템의 유효성 체크
		{
			FString ItemName = ItemClass->GetName();

			if (ItemInfos.Contains(ItemName))
			{
				ItemInfos[ItemName].Quantity--;

				if (ItemInfos[ItemName].Quantity <= 0)
				{

					if (ItemClass->IsChildOf(AWeaponItem::StaticClass()))
					{
						AWeaponItem* DroppedWeaponItem = Cast<AWeaponItem>(ItemClass->GetDefaultObject());
						TSubclassOf<AWeapon> DroppedWeaponClass = DroppedWeaponItem->GetWeaponClass();

						if (OnWeaponRemoved.IsBound())
						{
							OnWeaponRemoved.Broadcast(DroppedWeaponClass);
						}
	
					}


					// 아이템 수량이 0 이하이면 빈 슬롯으로 설정
					ItemInfos.Remove(ItemName);
					Items[Index] = nullptr;  
					CurrentItemNum--;
				}


				NotifyInventoryUpdated();

				return true;
			}
		}

		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Item is nullptr"));
			return false;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Item Index is wrong"));

	return false;
}


TSubclassOf<AItem> AInventory::GetItemByIndex(int Index)
{
	return Items[Index];
}



void AInventory::Clear()
{
	Items.Empty();
	ItemInfos.Empty();
	CurrentItemNum = 0;
}

void AInventory::SetMaxInventorySize(int InSize)
{
	MaxInventorySize = InSize;
	Items.SetNum(MaxInventorySize);
}

int AInventory::GetMaxInventorySize()
{
	return MaxInventorySize;
}

int AInventory::GetCurrentInventorySize()
{
	return CurrentItemNum;
}

void AInventory::SetCurrentInventoryIndex(int Index)
{
	CurrentInventoryIndex = Index;
	OnInventoryIndexChanged.Broadcast(Index);
}

int AInventory::GetCurrentInventoryIndex()
{
	return CurrentInventoryIndex;
}

EItemType AInventory::GetItemTypeByIndex(int Index)
{
	if (Items.IsValidIndex(Index))				// 인덱스의 유효성 체크
	{
		TSubclassOf<AItem> ItemClass = Items[Index];

		if (ItemClass)									// 아이템의 유효성 체크
		{
			AItem* Item = Cast<AItem>(ItemClass->GetDefaultObject());
			
			return Item->GetItemType();

		}
	}

	return EItemType::Invalid;
}

void AInventory::PrintAllItemInfos()			// for Debug
{
	for (const TPair<FString, FItemInfo>& ItemPair : ItemInfos)
	{
		FString ItemName = ItemPair.Key;
		FItemInfo ItemInfo = ItemPair.Value;

		int32 ItemQuantity = ItemInfo.Quantity;

		FString DebugText = FString::Printf(TEXT("Item: %s, Quantity: %d"), *ItemName, ItemQuantity);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, DebugText);
	}
}

void AInventory::PrintAllItems()					// for Debug
{

	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{

		TSubclassOf<AItem> CurrentItemClass = Items[Index];
		FString ItemName = CurrentItemClass ? CurrentItemClass->GetName() : TEXT("Unknown");

		if (CurrentItemClass == nullptr) 
		{
			FString DebugMessage = FString::Printf(TEXT("Index: %d, Item is null"), Index);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, DebugMessage);
		}

		else
		{
			FString DebugMessage = FString::Printf(TEXT("Index: %d, Item Actor is : %s"), Index, *CurrentItemClass->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, DebugMessage);
		}


		FItemInfo* ItemInfo = ItemInfos.Find(ItemName);
		int32 ItemQuantity = ItemInfo ? ItemInfo->Quantity : 0;


		FString DebugMessage = FString::Printf(TEXT("Index: %d, Item: %s, Quantity: %d"), Index, *ItemName, ItemQuantity);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, DebugMessage);
	}
}


int AInventory::FindEmptySlot() const
{
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (Items[Index] == nullptr)  // 빈 슬롯을 찾으면 반환
		{
			return Index;
		}
	}
	return INDEX_NONE;				// 빈 공간이 없으면 -1 반환
}


void AInventory::NotifyInventoryUpdated()
{
	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}
}