// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GunInterface.generated.h"


UINTERFACE(MinimalAPI)
class UGunInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PIXELMARIO_API IGunInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	virtual FVector GetMuzzleFlashLocation();
	virtual FTransform GetLeftHandIKData();
	virtual void GunFire(AActor* Interactor);
	virtual float GetShootDelay();

};
