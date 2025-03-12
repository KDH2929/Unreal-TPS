// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrainChunk.h"
#include "ProceduralTerrainMap.generated.h"



USTRUCT(BlueprintType)
struct FPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y;

	FPosition()
		: X(0), Y(0)
	{}

	FPosition(int InX, int InY)
		: X(InX), Y(InY)
	{}

	bool operator==(const FPosition& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FPosition& Pos)
	{
		return HashCombine(GetTypeHash(Pos.X), GetTypeHash(Pos.Y));
	}
};



UCLASS()
class PIXELMARIO_API AProceduralTerrainMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainMap();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		// 캐릭터 주위 Chunk를 생성할 칸 수 조정
	float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UVScale;


	// Z값 Clamp용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMax;


	float XOffset;
	float YOffset;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActorSpawnProbability;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> FoilageActor;


private:
	FPosition PlayerPos;
	FPosition ChunkPos;
	TSet<FPosition> GeneratedChunkPos;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
