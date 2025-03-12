// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonCharacter.h"
#include "Inventory.h"
#include "InputActionValue.h"
#include "UI_HUD.h"
#include "UI_DynamicCrosshair.h"
#include "Weapon.h"
#include "WeaponItem.h"
#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMouseMoved);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float);

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	Default      UMETA(DisplayName = "Default"),
	Dead		   UMETA(DisplayName = "Dead"),
	Pistol        UMETA(DisplayName = "Pistol"),
	Rifle          UMETA(DisplayName = "Rifle"),
};

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	FirstPerson  UMETA(DisplayName = "FirstPerson"),
	ThirdPerson  UMETA(DisplayName = "ThirdPerson"),
	ThirdPersonWithGun UMETA(DisplayName = "ThirdPersonWithGun")
};


UCLASS()
class PIXELMARIO_API APlayerCharacter : public AThirdPersonCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	void EquipWeapon(TSubclassOf<AWeapon> WeaponClass);
	void EquipWeapon(AWeaponItem* WeaponItem);
	void DropWeapon();

	FOnMouseMoved OnMouseMoved;
	FOnHPChanged OnHPChanged;

	UFUNCTION(BlueprintCallable)
	EAnimState GetAnimState();

	UFUNCTION(BlueprintCallable)
	ECameraState GetCameraState();

	UFUNCTION(BlueprintCallable)
	void SetCameraState(ECameraState InCameraState);


	UFUNCTION(BlueprintCallable)
	void SetCurrentCamera(UCameraComponent* InCameraComponent);


	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandIKData();


	UFUNCTION(BlueprintCallable)
	AWeapon* GetCurrentWeapon();

	void SetCrosshairVisibility(bool bActive);
	void SetHUDVisibility(bool bActive);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP;


private:
	UPROPERTY()
	AInventory* Inventory;

	UPROPERTY()
	AWeapon* CurrentWeapon;


	UCameraComponent* CurrentCamera;

	EAnimState AnimState;
	ECameraState CameraState;
	FName WeaponSocketName;
	

	bool bAutoFireEnabled = true;
	bool bFireEnabled = true;
	bool bZoomIn = false;
	bool bIsDead = false;
	FTimerHandle FireTimerHandle;
	FTimerHandle DeadTimerHandle;



private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ItemThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LeftMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum1_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum2_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum3_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum4_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum5_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* KeyNum6_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeFreeCam_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseScroll_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ItemConsume_Action;


protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Interact(const FInputActionValue& Value);
	virtual void ItemThrow(const FInputActionValue& Value);
	virtual void InventorySlot1Change(const FInputActionValue& Value);
	virtual void InventorySlot2Change(const FInputActionValue& Value);
	virtual void InventorySlot3Change(const FInputActionValue& Value);
	virtual void InventorySlot4Change(const FInputActionValue& Value);
	virtual void InventorySlot5Change(const FInputActionValue& Value);
	virtual void InventorySlot6Change(const FInputActionValue& Value);
	void OnInventoryWeaponRemoved(TSubclassOf<AWeapon> RemovedWeapon);					// 인벤토리에서 무기 제거시 확인해야할 사항
	void ChangeWeaponByIndex(int Index);
	void SetAnimStateByWeaponType(EWeaponType InWeaponType);
	void SetCameraStateByWeaponType(EWeaponType InWeaponType);
	void ChangeCamera();
	void Attack();
	void Fire();
	void HitscanFire();
	void SetFireEnabled();
	void SetAutoFireDisabled();
	void ZoomIn();
	void ZoomOut();
	void MouseScrollZoom(const FInputActionValue& Value);
	void ChangeFreeCam();
	void ItemConsume();
	void Die();
	void CreateDeadUI();



private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UI_HUDClass;

	UPROPERTY()
	UUI_HUD* UI_HUD;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UI_CrosshairClass;

	UPROPERTY()
	UUI_DynamicCrosshair* UI_Crosshair;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UI_SniperZoomClass;

	UPROPERTY()
	UUserWidget* UI_SniperZoom;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UI_DeadClass;

	UPROPERTY()
	UUserWidget* UI_Dead;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bone", meta = (AllowPrivateAccess = "true"))
	FName RightHandBoneName;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraWithGun;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonCameraBoomWithGun;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;


	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FirstPersonCameraBoom;


	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Material, meta = (AllowPrivateAccess = "true"))
	UMaterial* BulletDecal;
	
};
