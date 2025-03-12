// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Monster_CheckRange.h"
#include "MonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster.h"

UBTService_Monster_CheckRange::UBTService_Monster_CheckRange()
{
	NodeName = TEXT("CheckRangeWithTarget");
	Interval = 0.5f;
}

void UBTService_Monster_CheckRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}


	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMonsterAIController::TargetActorKey));
	if (TargetActor == nullptr)
	{
		return;
	}


	AMonster* ControllingMonster = Cast<AMonster>(ControllingPawn);
	if (ControllingMonster == nullptr)
	{
		return;
	}


	FVector ControllingPawnLocation = ControllingPawn->GetActorLocation();
	FVector TargetActorLocation = TargetActor->GetActorLocation();

	float Distance = FVector::Dist(ControllingPawnLocation, TargetActorLocation);
	
	float ChaseStopDistance = ControllingMonster->ChaseStopDistance;
	float AttackRange = ControllingMonster->AttackRange;

	if (Distance >= ChaseStopDistance)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMonsterAIController::TargetActorKey, nullptr);
	}
	

	if (Distance <= AttackRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMonsterAIController::IsInAttackRangeKey, true);
	}

	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMonsterAIController::IsInAttackRangeKey, false);
	}

}

