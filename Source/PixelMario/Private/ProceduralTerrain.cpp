// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrain.h"
#include "KismetProceduralMeshLibrary.h"


// Sets default values
AProceduralTerrain::AProceduralTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetupAttachment(GetRootComponent());

	Material = NULL;

	XSize = 5;
	YSize = 5;
	ZMultiplier = 200;
	VertexDistance = 100;
	UVScale = 1;
	NoiseScale = 1;

}

// Called when the game starts or when spawned
void AProceduralTerrain::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProceduralTerrain::OnConstruction(const FTransform& Transform)
{
	CreateMesh();
}

// Called every frame
void AProceduralTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralTerrain::CreateMesh()
{
	Vertices.Reset();
	Triangles.Reset();
	UV0.Reset();

	CreateVertices();
	CreateTriangles();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);
	ProceduralMesh->SetMaterial(0, Material);

	/*
	FVector BottomLeft = GetActorLocation();
	FVector BottomRight = BottomLeft + FVector(0, VertexDistance * XSize, 0);
	FVector TopLeft = BottomLeft + FVector(VertexDistance * XSize, 0, 0);
	FVector TopRight = BottomLeft + FVector(VertexDistance * XSize, VertexDistance * XSize, 0);

	DrawDebugSphere(GetWorld(), BottomLeft, 5.0f, 16, FColor::Red, true, 5.f, 0U, 0.0f);
	DrawDebugSphere(GetWorld(), BottomRight, 5.0f, 16, FColor::Red, true, 5.f, 0U, 0.0f);
	DrawDebugSphere(GetWorld(), TopLeft, 5.0f, 16, FColor::Red, true, 5.f, 0U, 0.0f);
	DrawDebugSphere(GetWorld(), TopRight, 5.0f, 16, FColor::Red, true, 5.f, 0U, 0.0f);
	*/
}

void AProceduralTerrain::SetProperties(float InXSize, float InYSize, float InZMultiplier, float InVertexDistance, float InUVScale, float InNoiseScale)
{
	XSize = InXSize;
	YSize = InYSize;
	ZMultiplier = InZMultiplier;
	VertexDistance = InVertexDistance;
	UVScale = InUVScale;
	NoiseScale = InNoiseScale;
}


void AProceduralTerrain::SetMaterial(UMaterialInterface* InMaterial)
{
	Material = InMaterial;
}

void AProceduralTerrain::CreateVertices()
{
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			// Pivot을 중앙으로 하기 위해서 Size / (-2)

			float VertexX = (XSize / (-2) + X) * VertexDistance;
			float VertexY = (YSize / (-2) + Y) * VertexDistance;

			float Z = FMath::PerlinNoise2D(FVector2D((X * NoiseScale + 0.1), (Y * NoiseScale + 0.1))) * ZMultiplier;


			float UVX = (XSize / (-2) + X) * UVScale;
			float UVY = (YSize / (-2) + Y) * UVScale;


			Vertices.Add(FVector(VertexX, VertexY, Z));
			UV0.Add(FVector2D(UVX, UVY));


			/*

			PerlinNoise에 0.1 을 더해주는 이유는

			X와 Y값이 정수(1, 2, 3, 4 ...) 로 들어가게 된다면 Perlin noise 값이 계속 0으로 나오게 된다.

			*/

		}
	}
}

void AProceduralTerrain::CreateTriangles()
{
	int Vertex = 0;


	for (int X = 0; X < XSize; ++X) {

		for (int Y = 0; Y < YSize; ++Y)
		{
			Triangles.Add(Vertex);								// 좌하단 
			Triangles.Add(Vertex + 1);						// 우하단
			Triangles.Add(Vertex + YSize + 1);			// 좌상단

			Triangles.Add(Vertex + 1);						// 우하단
			Triangles.Add(Vertex + YSize + 2);			// 우상단
			Triangles.Add(Vertex + YSize + 1);			// 좌상단

			++Vertex;
		}

		++Vertex;
	}

}
