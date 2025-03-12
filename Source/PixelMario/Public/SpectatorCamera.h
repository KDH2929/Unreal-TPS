// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "SpectatorCamera.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API ASpectatorCamera : public ASpectatorPawn
{
	GENERATED_BODY()

	ASpectatorCamera();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ChangeCam();


private:

	class UInputAction* ChangeCamAction;

	FTimerHandle DestroyTimerHandle;
	void DestroyActor();
};