// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Monster_CheckRange.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API UBTService_Monster_CheckRange : public UBTService
{
	GENERATED_BODY()

	UBTService_Monster_CheckRange();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
