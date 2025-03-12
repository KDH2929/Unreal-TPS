// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.h"
#include "WeaponItem.generated.h"



UCLASS()
class PIXELMARIO_API AWeaponItem : public AItem
{
	GENERATED_BODY()
	
	AWeaponItem();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> WeaponClass;


public:
	
	EWeaponType GetWeaponType();
	TSubclassOf<AWeapon> GetWeaponClass();
};
