// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.h"
#include "Gun.h"
#include "PlayerCharacterAnim.generated.h"

/**
 *
 */
UCLASS()
class PIXELMARIO_API UPlayerCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

	UPlayerCharacterAnim();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	void GetRecoilTransform(FTransform InRecoilTransform);

public:
	void BindToGun(AGun* Gun);									// Delegate


private:

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* MoveComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	EAnimState AnimState;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool ShouldMove;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool IsFalling;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FTransform LHIKData;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float CharacterDirection;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FTransform Recoil;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FTransform RecoilTransform;

};