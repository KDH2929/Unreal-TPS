// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Monster_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API UBTTask_Monster_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Monster_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
