// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMARIO_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();
	virtual void OnPossess(APawn* InPawn) override;

	void StopAI();


	// 블랙보드 키 이름이 변하지 않는다는 가정하에 static const 사용

	static const FName TargetActorKey;
	static const FName IsInAttackRangeKey;
	static const FName PatrolPosKey;
	static const FName SpawnedPosKey;

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;


};
