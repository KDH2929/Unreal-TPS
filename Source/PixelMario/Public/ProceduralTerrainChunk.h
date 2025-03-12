// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrainChunk.generated.h"


class UProceduralMeshComponent;
class UMaterialInterface;


UCLASS()
class PIXELMARIO_API AProceduralTerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrainChunk();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VertexDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UVScale;

	// Z°ª Clamp¿ë
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YOffset;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorToSpawn;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> FoilageActor;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActorSpawnProbability;

private:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;

	TArray<FVector> Vertices;
	TArray<int>Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;
	TArray<struct FProcMeshTangent> Tangents;

	TArray<FTransform> ActorSpawnTransforms;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	UFUNCTION(BlueprintCallable)
	void CreateMesh();


	UFUNCTION(BlueprintCallable)
	void SetProperties(float InXSize, float InYSize, float InZMultiplier, float InVertexDistance, float InUVScale, float InNoiseScale);

	UFUNCTION(BlueprintCallable)
	void SetOffset(float InXOffset, float InYOffset);

	UFUNCTION(BlueprintCallable)
	void SetMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable)
	void SetSpawnProbability(float InProbability);


	UFUNCTION(BlueprintCallable)
	void SetActorToSpawn(TSubclassOf<AActor> InActor);


	UFUNCTION(BlueprintCallable)
	void SetFoilageActor(TSubclassOf<AActor> InActor);



private:

	void CreateVertices();
	void CreateTriangles();
	void CreateProceduralActorTransform(FVector InLocation);
	void SpawnActors();
	void SpawnFoilages();

};
