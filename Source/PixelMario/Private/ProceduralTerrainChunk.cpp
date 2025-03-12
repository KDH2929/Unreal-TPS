// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainChunk.h"
#include "KismetProceduralMeshLibrary.h"


// Sets default values
AProceduralTerrainChunk::AProceduralTerrainChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetupAttachment(GetRootComponent());

	ActorSpawnProbability = 0.01;
	Material = NULL;
	ActorToSpawn = NULL;

	XSize = 5;
	YSize = 5;
	ZMultiplier = 200;
	VertexDistance = 100;
	UVScale = 1;
	NoiseScale = 1;
	XOffset = 0;
	YOffset = 0;

}

// Called when the game starts or when spawned
void AProceduralTerrainChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProceduralTerrainChunk::OnConstruction(const FTransform& Transform)
{
	// CreateMesh();
}


// Called every frame
void AProceduralTerrainChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralTerrainChunk::CreateMesh()
{
	Vertices.Reset();
	Triangles.Reset();
	UV0.Reset();
	ActorSpawnTransforms.Reset();

	CreateVertices();
	CreateTriangles();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);
	ProceduralMesh->SetMaterial(0, Material);

	SpawnActors();
	SpawnFoilages();
}

void AProceduralTerrainChunk::SetProperties(float InXSize, float InYSize, float InZMultiplier, float InVertexDistance, float InUVScale, float InNoiseScale)
{
	XSize = InXSize;
	YSize = InYSize;
	ZMultiplier = InZMultiplier;
	VertexDistance = InVertexDistance;
	UVScale = InUVScale;
	NoiseScale = InNoiseScale;
}

void AProceduralTerrainChunk::SetOffset(float InXOffset, float InYOffset)
{
	XOffset = InXOffset;
	YOffset = InYOffset;
}

void AProceduralTerrainChunk::SetMaterial(UMaterialInterface* InMaterial)
{
	Material = InMaterial;
}

void AProceduralTerrainChunk::SetSpawnProbability(float InProbability)
{
	ActorSpawnProbability = InProbability;
}

void AProceduralTerrainChunk::SetActorToSpawn(TSubclassOf<AActor> InActor)
{
	ActorToSpawn = InActor;
}

void AProceduralTerrainChunk::SetFoilageActor(TSubclassOf<AActor> InActor)
{
	FoilageActor = InActor;
}


void AProceduralTerrainChunk::CreateVertices()
{
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			// Pivot을 중앙으로 하기 위해서 Size / (-2)

			float VertexX = (XSize / (-2) + X) * VertexDistance;
			float VertexY = (YSize / (-2) + Y) * VertexDistance;

			float Z = FMath::PerlinNoise2D(FVector2D((X + 0.1 + XOffset) * NoiseScale, (Y + 0.1 + YOffset) * NoiseScale)) * ZMultiplier;


			float UVX = (XSize / (-2) + X) * UVScale;
			float UVY = (YSize / (-2) + Y) * UVScale;


			Vertices.Add(FVector(VertexX, VertexY, Z));
			UV0.Add(FVector2D(UVX, UVY));

			CreateProceduralActorTransform(FVector(VertexX, VertexY, Z));


			/*

			PerlinNoise에 0.1 을 더해주는 이유는

			X와 Y값이 정수(1, 2, 3, 4 ...) 로 들어가게 된다면 Perlin noise 값이 계속 0으로 나오게 된다.

			*/

		}
	}
}

void AProceduralTerrainChunk::CreateTriangles()
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

void AProceduralTerrainChunk::CreateProceduralActorTransform(FVector InLocation)
{
	if (ActorToSpawn)
	{
		float Probability = FMath::RandRange(0.0f, 1.0f);

		if (Probability <= ActorSpawnProbability)
		{
			FVector ActorLocation = GetActorLocation() + InLocation;
			FRotator ActorRotation = FRotator::ZeroRotator;
			FVector ActorScale = FVector(1, 1, 1);
			FTransform ActorTransform = FTransform(ActorRotation, ActorLocation, ActorScale);

			ActorSpawnTransforms.Add(ActorTransform);
		}
	}

}

void AProceduralTerrainChunk::SpawnActors()
{
	if (ActorToSpawn)
	{
		for (const FTransform& SpawnTransform : ActorSpawnTransforms)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams);

		}
	}
}

void AProceduralTerrainChunk::SpawnFoilages()
{
	if (FoilageActor)
	{
		for (const FVector& Vertex : Vertices)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FVector ActorLocation = GetActorLocation() + Vertex;

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(FoilageActor, ActorLocation, FRotator::ZeroRotator, SpawnParams);

		}
	}
}


/*   Runtime foilage 생성   (컴퓨터 사양 이슈로 포기)

if (SpawnedActor)
			{
				AProceduralPoilage* Foilage = Cast<AProceduralFoilage>(SpawnedActor);

				if (Foilage)
				{
					if (Foilage->GetStaticMesh())
					{
						FVector StaticMeshBoxExtent = Foilage->GetStaticMesh()->GetBoundingBox().GetExtent();

						float SpaceBetweenRow = StaticMeshBoxExtent.X;
						float SpaceBetweenColumn = StaticMeshBoxExtent.Y;

						int NumRow = FMath::FloorToInt(VertexDistance / SpaceBetweenRow);
						int NumColumn = FMath::FloorToInt(VertexDistance / SpaceBetweenColumn);

						FString Message = FString::Printf(TEXT("NumRow: %d, NumColumn: %d"), NumRow, NumColumn);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);

						// Foilage->SetRowColumn(NumRow, NumColumn);
						Foilage->SpawnFoilage();

					}
				}

*/
