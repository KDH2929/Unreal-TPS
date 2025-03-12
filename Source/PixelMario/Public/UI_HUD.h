// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI_HUD.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API UUI_HUD : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void BindToInventory(class AInventory* In_Inventory);

	UFUNCTION()
	void BindToCharacter(class APlayerCharacter* Character);

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class AInventory> Inventory;

	virtual void NativeConstruct() override;


	// UI ¿ä¼Òµé
	UPROPERTY()
	class UProgressBar* HPBar;

	UPROPERTY()
	TArray<UTextBlock*> ItemNames;

	UPROPERTY()
	TArray<UImage*> ItemImages;

	UPROPERTY()
	TArray<UImage*> SlotSelectors;

	
private:
	void UpdateInventoryUI();
	void UpdateHPBar(float HPPercentage);
	void UpdateSlotIndex(int Index);

};
