// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster.h"

const FName AMonsterAIController::TargetActorKey(TEXT("TargetActor"));
const FName AMonsterAIController::IsInAttackRangeKey(TEXT("IsInAttackRange"));
const FName AMonsterAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AMonsterAIController::SpawnedPosKey(TEXT("SpawnedPos"));


AMonsterAIController::AMonsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/BP/BP_Monster/MonsterAI/BB_Monster.BB_Monster"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}


	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/BP/BP_Monster/MonsterAI/BT_Monster.BT_Monster"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* BlackboardComponent = Blackboard.Get();

	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		this->Blackboard = BlackboardComponent;
		BlackboardComponent->SetValueAsVector(SpawnedPosKey, InPawn->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("AIController couldn't run behavior tree"));
		}
	}
}

void AMonsterAIController::StopAI()
{
	if (UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(GetBrainComponent()))
	{
		BehaviorComp->StopTree();
	}

	StopMovement();

}
