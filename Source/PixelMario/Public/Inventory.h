// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "Weapon.h"
#include "Inventory.generated.h"



DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponRemoved, TSubclassOf<AWeapon> WeaponClass);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryIndexChanged, int index);


USTRUCT(BlueprintType)
struct FItemInfo {
	GENERATED_BODY()

public:

	int32 Quantity;
	UTexture2D* ItemTexture;
};


UCLASS()
class PIXELMARIO_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	


	AInventory();

	UPROPERTY()
	TArray<TSubclassOf<AItem>> Items;

	UPROPERTY()
	TMap<FString, FItemInfo> ItemInfos;
	

	UFUNCTION(BlueprintCallable)
	int AddItem(AItem* NewItem);


	UFUNCTION(BlueprintCallable)
	bool RemoveItemByIndex(int Index);


	UFUNCTION(BlueprintCallable)
	TSubclassOf<AItem> GetItemByIndex(int Index);


	UFUNCTION(BlueprintCallable)
	void SetMaxInventorySize(int InSize);


	UFUNCTION(BlueprintCallable)
	int GetMaxInventorySize();


	UFUNCTION(BlueprintCallable)
	int GetCurrentInventorySize();


	UFUNCTION(BlueprintCallable)
	void SetCurrentInventoryIndex(int Index);

	UFUNCTION(BlueprintCallable)
	int GetCurrentInventoryIndex();

	UFUNCTION(BlueprintCallable)
	EItemType GetItemTypeByIndex(int Index);

	UFUNCTION(BlueprintCallable)
	void Clear();


	UFUNCTION(BlueprintCallable)
	void PrintAllItemInfos();

	UFUNCTION(BlueprintCallable)
	void PrintAllItems();


	// Delegate 관련
	FOnWeaponRemoved OnWeaponRemoved;
	FOnInventoryUpdated OnInventoryUpdated;
	FOnInventoryIndexChanged OnInventoryIndexChanged;
	void NotifyInventoryUpdated();							// BroadCast 수행



private:

	int MaxInventorySize;
	int CurrentItemNum = 0;
	int CurrentInventoryIndex = 0;

	int FindEmptySlot() const;

};
