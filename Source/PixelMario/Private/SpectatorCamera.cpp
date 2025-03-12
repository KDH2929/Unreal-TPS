// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorCamera.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


ASpectatorCamera::ASpectatorCamera()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ChangeCam(TEXT("InputAction'/Game/ThirdPerson/Input/Actions/IA_FreeCam.IA_FreeCam'"));
	if (IA_ChangeCam.Succeeded())
	{
		ChangeCamAction = IA_ChangeCam.Object;
	}
}


void ASpectatorCamera::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ChangeCamAction, ETriggerEvent::Triggered, this, &ASpectatorCamera::ChangeCam);
	}
}

void ASpectatorCamera::ChangeCam()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);

	if (!FoundActors.IsEmpty())
	{
		AController* PlayerController = GetController();

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FoundActors[0]);


		if (PlayerCharacter)
		{
			PlayerCharacter->SetHUDVisibility(true);
			PlayerCharacter->SetCrosshairVisibility(true);					// 무기 없을 시 false로 처리됨

			PlayerController->Possess(PlayerCharacter);

			GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ASpectatorCamera::DestroyActor, 1.0f, false);
		}

	}
}

void ASpectatorCamera::DestroyActor()
{
	Destroy();
}
