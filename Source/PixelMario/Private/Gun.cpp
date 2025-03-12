// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SkeletalMeshComponent.h"

AGun::AGun()
{
	ShootDelay = 1.0f;
	WeaponRange = 2000.0f;
	MouseAimSpread = 50.0f;
	AimReductionRate = 30.0f;
	MinAimSpread = 5.0f;
	MinFireRadius = 25.0f;
}

void AGun::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	AddAimSpread(-AimReductionRate);
	CurrentAimSpread = FMath::Clamp(CurrentAimSpread, MinAimSpread, MaxAimSpread);
}

FVector AGun::GetMuzzleFlashLocation()
{
	if (WeaponMesh)
	{
		FVector MuzzleFlashWorldLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));

		return MuzzleFlashWorldLocation;
	}

	return FVector::ZeroVector;
}

FTransform AGun::GetLeftHandIKData()
{
	if (WeaponMesh)
	{
		FTransform LeftHandIKData = WeaponMesh->GetSocketTransform(TEXT("LHIK"));

		return LeftHandIKData;
	}

	return FTransform::Identity;
}


void AGun::GunFire(AActor* Interactor)
{
    if (FireAnimation)
    {
        if (WeaponMesh)
        {
            USkeletalMeshComponent* GunMesh = WeaponMesh;
  
            GunMesh->PlayAnimation(FireAnimation, false);

			SetProceduralRecoilTransform();

			AddAimSpread(FireAimSpread);
        }
    }
}

void AGun::SetProceduralRecoilTransform()
{
	float RandomRoll = FMath::FRandRange(-1.0f, 1.0f);
	float RandomPitch = FMath::FRandRange(-0.4f, 0.4f);
	float RandomYaw = FMath::FRandRange(-0.8f, 0.8f);

	float RandomX = FMath::FRandRange(-0.1f, 0.1f);		// Left & Right
	float RandomY = FMath::FRandRange(-1.0f, -2.0f);	// Back
	float RandomZ = FMath::FRandRange(0.5f, 1.0f);		// Up

	FVector ProceduralRecoil_Location = FVector(RandomX * RecoilSide, RandomY * RecoilBack, RandomZ * RecoilUp);
	FRotator ProceduralRecoil_Rotation = FRotator(
		RandomRoll * Recoil_Rotation.Roll,
		RandomPitch * Recoil_Rotation.Pitch,
		RandomYaw * Recoil_Rotation.Yaw
	);

	FTransform ProceduralRecoil_Transform = FTransform(
		ProceduralRecoil_Rotation,
		ProceduralRecoil_Location,
		FVector(1.0f, 1.0f, 1.0f)
	);

	Recoil_Transform = ProceduralRecoil_Transform;

	if (OnChangedRecoilTransform.IsBound())
	{
		OnChangedRecoilTransform.Broadcast(Recoil_Transform);
	}


	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGun::ResetRecoilTransform, 0.1f, false);
}


float AGun::GetShootDelay()
{
	return ShootDelay;
}

void AGun::AddAimSpread(float Spread)
{
	CurrentAimSpread = FMath::Clamp(CurrentAimSpread + Spread, MinAimSpread, MaxAimSpread);
}

float AGun::GetCrosshairSpread()
{
	float CrosshairSpread = FMath::GetMappedRangeValueClamped(
		FVector2D(MinAimSpread, MaxAimSpread),
		FVector2D(MinAimSpread / 8.0f, MaxAimSpread / 18.0f),
		CurrentAimSpread
	);

	return CrosshairSpread;
}

void AGun::BindToPlayerMouseMovement(APlayerCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->OnMouseMoved.AddUObject(this, &AGun::AddMouseAimSpread);
	}
}

void AGun::ResetRecoilTransform()
{
	Recoil_Transform = FTransform::Identity;
}

void AGun::AddMouseAimSpread()
{
	AddAimSpread(MouseAimSpread);
}

float AGun::GetBiasedRandomRadius(float InRadius)
{
	float Probability = FMath::RandRange(0.0f, 1.0f);
	float OutRadius;

	if (Probability <= 0.1f)			// 10% È®·ü
	{
		OutRadius = FMath::RandRange(InRadius * 0.8f, InRadius * 1.0f);
	}

	else if (Probability <= 0.3f)		// 30% È®·ü
	{
		OutRadius = FMath::RandRange(InRadius * 0.4f, InRadius * 0.8f);
	}

	else									// 60% È®·ü
	{
		OutRadius = FMath::RandRange(InRadius * 0.0f, InRadius * 0.4f);
	}

	return OutRadius;
}


FVector AGun::GetLinetraceShootOffsetVector(FVector RightVector, FVector UpVector)
{
	float Radius = FMath::GetMappedRangeValueClamped(
		FVector2D(MinAimSpread, MaxAimSpread), 
		FVector2D(MinFireRadius, MaxFireRadius), 
		CurrentAimSpread
	);

	Radius = GetBiasedRandomRadius(Radius);

	float Theta = FMath::RandRange(0.0f, 360.0f);
	FVector OffsetVector = (FMath::Cos(Theta) * RightVector + FMath::Sin(Theta) * UpVector) * Radius;

	return OffsetVector;
}
