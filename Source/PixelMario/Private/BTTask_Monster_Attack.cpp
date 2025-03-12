// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Monster_Attack.h"
#include "MonsterAIController.h"
#include "Monster.h"


UBTTask_Monster_Attack::UBTTask_Monster_Attack()
{
	NodeName = TEXT("Attack");
	bNotifyTick = true;
	AttackEnded = false;
}

EBTNodeResult::Type UBTTask_Monster_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (Monster->bAttackEnable == false)
	{
		return EBTNodeResult::Succeeded;
	}


	Monster->OnAttackEnd.AddLambda([this]() -> void {
		AttackEnded = true;
	});

	Monster->Attack();

	return EBTNodeResult::InProgress;

}

void UBTTask_Monster_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (AttackEnded)
	{
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
