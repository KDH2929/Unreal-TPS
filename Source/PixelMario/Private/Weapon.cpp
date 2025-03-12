// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Camera/CameraComponent.h"

AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	WeaponCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("WeaponCamera"));
	WeaponCamera->SetupAttachment(RootComponent);
}


FName AWeapon::GetWeaponSocketName()
{
	return WeaponSocket;
}


EWeaponType AWeapon::GetWeaponType()
{
	return WeaponType;
}

float AWeapon::GetWeaponRange()
{
	return WeaponRange;
}

float AWeapon::GetWeaponDamage()
{
	return Damage;
}

UCameraComponent* AWeapon::GetWeaponCamera()
{
	return WeaponCamera;
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (WeaponMesh->DoesSocketExist("AimDownSight"))
	{
		WeaponCamera->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
		WeaponCamera->AttachToComponent(
					WeaponMesh, 
					FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true),
					"AimDownSight"
		);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("AimDownSight Socket is invalid!"));
	}
}
