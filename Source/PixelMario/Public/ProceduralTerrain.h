// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain.generated.h"



class UProceduralMeshComponent;
class UMaterialInterface;


UCLASS()
class PIXELMARIO_API AProceduralTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTerrain();

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
	UMaterialInterface* Material;


private:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;

	TArray<FVector> Vertices;
	TArray<int>Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;
	TArray<struct FProcMeshTangent> Tangents;



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
	void SetMaterial(UMaterialInterface* InMaterial);

private:

	void CreateVertices();
	void CreateTriangles();

};
