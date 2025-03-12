// Fill out your copyright notice in the Description page of Project Settings.


#include "GunInterface.h"

// Add default functionality here for any IWeaponInterface functions that are not pure virtual.


FVector IGunInterface::GetMuzzleFlashLocation()
{
	return FVector::ZeroVector;
}

FTransform IGunInterface::GetLeftHandIKData()
{
	return FTransform::Identity;
}

void IGunInterface::GunFire(AActor* Interactor)
{
}

float IGunInterface::GetShootDelay()
{
	return 0.0f;
}