// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterAIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Monster.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);


UENUM(BlueprintType)
enum class EMonsterAnimState : uint8
{
	Default      UMETA(DisplayName = "Default"),
	Dead		   UMETA(DisplayName = "Dead"),
};


UCLASS()
class PIXELMARIO_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChaseStopDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* HPBarWidget;


	bool bAttackEnable;
	bool bIsDead;

	EMonsterAnimState MonsterAnimState;
	FTimerHandle AttackTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	EMonsterAnimState GetAnimState();

	FOnAttackEndDelegate OnAttackEnd;



private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPawnSensingComponent* PawnSensingComponent;
	
	UFUNCTION()					// ��������Ʈ�� ���ε��ϱ� ���� �Լ��� UFUNCTION ��ũ�� ����ؾ���
	void OnPawnDetected(APawn* DetectedPawn);



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UFUNCTION()						// ���������� ��������Ʈ ���ε��� UFUNCTION ��ũ�� �ʿ�
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SetAttackEnable();
	void Die();
	
};
