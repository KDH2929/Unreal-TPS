// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
	Shotgun	UMETA(DisplayName = "Shotgun")
};



UCLASS()
class PIXELMARIO_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	UFUNCTION(BlueprintCallable)
	FName GetWeaponSocketName();

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType();

	UFUNCTION(BlueprintCallable)
	float GetWeaponRange();

	UFUNCTION(BlueprintCallable)
	float GetWeaponDamage();


	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetWeaponCamera();


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* WeaponCamera;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	float WeaponRange;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	float Damage;


protected:

	virtual void OnConstruction(const FTransform& Transform) override;

};
