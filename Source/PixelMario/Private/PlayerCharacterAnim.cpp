// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterAnim.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Gun.h"


UPlayerCharacterAnim::UPlayerCharacterAnim()
{
	PlayerCharacter = nullptr;
	MoveComponent = nullptr;
	AnimState = EAnimState::Default;
	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	ShouldMove = false;
	IsFalling = false;
	LHIKData = FTransform::Identity;
	Pitch = 0.0f;
	CharacterDirection = 0.0f;
	Recoil = FTransform::Identity;
	RecoilTransform = FTransform::Identity;
}

void UPlayerCharacterAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* PawnOwner = TryGetPawnOwner();

	if (PawnOwner)
	{
		PlayerCharacter = Cast<APlayerCharacter>(PawnOwner);
		MoveComponent = PlayerCharacter->GetCharacterMovement();
		AnimState = PlayerCharacter->GetAnimState();
	}

}

void UPlayerCharacterAnim::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PlayerCharacter)
	{
		// 움직임 제어
		Velocity = PlayerCharacter->GetVelocity();
		GroundSpeed = Velocity.Size2D();

		FVector CurrentAcceleration = MoveComponent->GetCurrentAcceleration();

		ShouldMove = ((GroundSpeed > 3.0f) && (CurrentAcceleration != FVector::ZeroVector));
		IsFalling = MoveComponent->IsFalling();
		
		AnimState = PlayerCharacter->GetAnimState();
		LHIKData = PlayerCharacter->GetLeftHandIKData();

		// Pitch 처리
		FRotator ControlRotation = PlayerCharacter->GetControlRotation();
		
		if (ControlRotation.Pitch >= 180.0f)
		{
			Pitch = ControlRotation.Pitch - 360.0f;
		}

		else
		{
			Pitch = ControlRotation.Pitch;
		}

		// 총 소지시 이동에서 사용
		CharacterDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, PlayerCharacter->GetActorRotation());

	}

	else
	{
		APawn* PawnOwner = TryGetPawnOwner();

		if (PawnOwner)
		{
			PlayerCharacter = Cast<APlayerCharacter>(PawnOwner);
			MoveComponent = PlayerCharacter->GetCharacterMovement();
			AnimState = PlayerCharacter->GetAnimState();
		}
	}
}

void UPlayerCharacterAnim::BindToGun(AGun* Gun)
{
	if (Gun)
	{
		Gun->OnChangedRecoilTransform.AddUObject(this, &UPlayerCharacterAnim::GetRecoilTransform);
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Red, FString("Delegate binding to Gun is failed"));
	}
}

void UPlayerCharacterAnim::GetRecoilTransform(FTransform InRecoilTransform)
{
	RecoilTransform = InRecoilTransform;

	FVector Location = InRecoilTransform.GetLocation();
	FRotator Rotation = InRecoilTransform.GetRotation().Rotator();
	FVector Scale = InRecoilTransform.GetScale3D();

}
