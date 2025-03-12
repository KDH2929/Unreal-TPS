// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Monster_FindPatrolPos.h"
#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Monster.h"

UBTTask_Monster_FindPatrolPos::UBTTask_Monster_FindPatrolPos()			// »ý¼ºÀÚ
{
	NodeName = TEXT("FindPatolPos");
}


EBTNodeResult::Type UBTTask_Monster_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());

	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector StartLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMonsterAIController::SpawnedPosKey);

	FNavLocation NextPatrolPoint;
	float Range = 500.0f;

	if (NavSystem->GetRandomPointInNavigableRadius(StartLocation, Range, NextPatrolPoint))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AMonsterAIController::PatrolPosKey, NextPatrolPoint.Location);
		return EBTNodeResult::Succeeded;
	}

	else
	{
		return EBTNodeResult::Failed;
	}
}
