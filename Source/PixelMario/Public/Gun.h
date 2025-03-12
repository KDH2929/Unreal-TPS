// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GunInterface.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimSequence.h"
#include "PlayerCharacter.h"
#include "Gun.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedRecoilTransform, FTransform RecoilTransform);

UCLASS()
class PIXELMARIO_API AGun : public AWeapon, public IGunInterface
{
	GENERATED_BODY()

	AGun();

public:

	FOnChangedRecoilTransform OnChangedRecoilTransform;
	FVector GetLinetraceShootOffsetVector(FVector RightVector, FVector UpVector);

private:
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* FireAnimation;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float ShootDelay;


	// 사격 시 에임 Spread
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float FireAimSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float AimReductionRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float MouseAimSpread;

	// UI 에임의 Spread 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float MinAimSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float MaxAimSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float CurrentAimSpread;


	// 극좌표계 기반 라인트레이스 사격의 반지름 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float MinFireRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float MaxFireRadius;




	// Recoil Location = 본을 얼마만큼 이동시킬지 가중치		(X, Y, Z)
	// Recoil Rotation = 본을 얼마만큼 회전시킬지 가중치      (Roll, Pitch, Yaw)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float RecoilUp;			// Z

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float RecoilBack;		// Y

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	float RecoilSide;		// X

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunInfo", meta = (AllowPrivateAccess = "true"))
	FRotator Recoil_Rotation;
	

	UPROPERTY()
	FTransform Recoil_Transform;

	UPROPERTY()
	FTimerHandle TimerHandle;


public:

	UCameraComponent* AimDownSightCamera;


public:
	
	virtual FVector GetMuzzleFlashLocation() override;
	virtual FTransform GetLeftHandIKData() override;
	virtual void GunFire(AActor* Interactor) override;
	virtual float GetShootDelay() override;
	void AddAimSpread(float Spread);
	float GetCrosshairSpread();
	void BindToPlayerMouseMovement(APlayerCharacter* PlayerCharacter);

private:
	void SetProceduralRecoilTransform();
	void ResetRecoilTransform();

	void AddMouseAimSpread();

	float GetBiasedRandomRadius(float InRadius);

};
