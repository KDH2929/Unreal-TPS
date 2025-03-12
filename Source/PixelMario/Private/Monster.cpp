// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h" 
#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/CapsuleComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"



// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MonsterAnimState = EMonsterAnimState::Default;

	
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	if (PawnSensingComponent)
	{
		PawnSensingComponent->SightRadius = 3000.0f;
		PawnSensingComponent->SetPeripheralVisionAngle(80.0f);
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AMonster::OnPawnDetected);
	}
	

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = 250.0f;
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	ChaseStopDistance = 1000;
	AttackRange = 100;
	AttackRadius = 30;
	AttackDelay = 3;
	bAttackEnable = true;


	MaxHP = 100;
	CurrentHP = MaxHP;
	Damage = 20;

	bIsDead = false;

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));

	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::World);
	HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_MonsterHPBar(TEXT("/Game/UI/BP_UI_MonsterHP.BP_UI_MonsterHP_C"));
	if (UI_MonsterHPBar.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_MonsterHPBar.Class);
		HPBarWidget->SetDrawSize(FVector2D(110.0f, 50.0f));
	}


	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Monster"));
	GetMesh()->SetCollisionProfileName(TEXT("Monster"));

}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	HPBarWidget->SetCastShadow(false);
}


// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float HPPercentage = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);

	if (HPBarWidget)
	{
		UUserWidget* UserWidget = Cast<UUserWidget>(HPBarWidget->GetWidget());

		UProgressBar* ProgressBar = Cast<UProgressBar>(UserWidget->GetWidgetFromName(TEXT("HPBar")));
		if (ProgressBar)
		{
			ProgressBar->SetPercent(HPPercentage);


			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
				if (PlayerCameraManager)
				{
					FVector WidgetWorldLocation = HPBarWidget->GetComponentLocation();
					FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

					FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WidgetWorldLocation, CameraLocation);

					HPBarWidget->SetWorldRotation(LookAtRotation);
				}
			}
		}
	}

}

void AMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 델리게이트는 한 번만 바인딩해야함. PostInitialize에서 처리하는게 좋다.
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMonster::OnAttackMontageEnded);
	}

}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	float ReceivedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP -= ReceivedDamage;

	if (CurrentHP <= 0 && !bIsDead)
	{
		CurrentHP = 0;

		Die();

		bIsDead = true;
	}

	return ReceivedDamage;
}

void AMonster::Attack()
{
	if (!bAttackEnable) return;

	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		bAttackEnable = false;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * AttackRange;

		ECollisionChannel TraceChannel = ECC_GameTraceChannel1;

		bool bHit = GetWorld()->SweepSingleByChannel(
			HitResult,
			Start,
			End,
			FQuat::Identity,
			TraceChannel,
			FCollisionShape::MakeSphere(AttackRadius),
			Params
		);


		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor)
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(HitActor);

				if (PlayerCharacter)
				{
					UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
				}
			}
		}


		// for debug

		FVector TraceVec = GetActorForwardVector() * AttackRange;
		FVector Center = GetActorLocation() + TraceVec * 0.5f;
		float HalfHeight = AttackRange * 0.5f + AttackRadius;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
		FColor DrawColor = bHit ? FColor::Green : FColor::Red;
		float DebugLifeTime = 5.0f;

		/*
		DrawDebugCapsule(
			GetWorld(),
			Center,
			HalfHeight,
			AttackRadius,
			CapsuleRot,
			DrawColor,
			false,
			DebugLifeTime
		);
		*/
		
		// Attack Montage
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);

	}	
}

EMonsterAnimState AMonster::GetAnimState()
{
	return MonsterAnimState;
}


void AMonster::OnPawnDetected(APawn* DetectedPawn)
{
	if (DetectedPawn)
	{
		if (DetectedPawn->IsA<APlayerCharacter>())
		{
			// UE_LOG(LogTemp, Warning, TEXT("PlayerChracter is detected"));

			AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(GetController());
			if (MonsterAIController)
			{
				MonsterAIController->GetBlackboardComponent()->SetValueAsObject(MonsterAIController->TargetActorKey, DetectedPawn);
			}
		}
	}
}


void AMonster::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		OnAttackEnd.Broadcast();
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AMonster::SetAttackEnable, AttackDelay, false);
	}
}

void AMonster::SetAttackEnable()
{
	bAttackEnable = true;
}

void AMonster::Die()
{
	MonsterAnimState = EMonsterAnimState::Dead;

	AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(GetController());
	if (MonsterAIController)
	{
		MonsterAIController->StopAI();
	}

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(5.0f);
}
