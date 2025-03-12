// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"

AWeaponItem::AWeaponItem()
{
	ItemType = EItemType::Weapon;
}

EWeaponType AWeaponItem::GetWeaponType()
{
	return WeaponType;
}

TSubclassOf<AWeapon> AWeaponItem::GetWeaponClass()
{
	return WeaponClass;
}
