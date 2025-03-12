// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Monster_TurnToTarget.h"
#include "Monster.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"


UBTTask_Monster_TurnToTarget::UBTTask_Monster_TurnToTarget()
{
	NodeName = TEXT("Turn");
}


EBTNodeResult::Type UBTTask_Monster_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}


	auto Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMonsterAIController::TargetActorKey));

	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}


	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Monster->GetActorLocation(), Target->GetActorLocation());

	Monster->SetActorRotation(FMath::RInterpTo(Monster->GetActorRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 2.0f));


	return EBTNodeResult::Succeeded;

}
