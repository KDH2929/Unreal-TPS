// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrainMap.h"
#include "ProceduralTerrainChunk.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProceduralTerrainMap::AProceduralTerrainMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Distance = 3;
	Size = 4;
	ZMultiplier = 300;
	Scale = 1000;
	UVScale = 1;
	NoiseScale = 0.1;
	XOffset = 0;
	YOffset = 0;
	ActorSpawnProbability = 0.01;

}

// Called when the game starts or when spawned
void AProceduralTerrainMap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralTerrainMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            FVector PawnLocation = PlayerPawn->GetActorLocation();

			PlayerPos.X = PawnLocation.X / Scale;
			PlayerPos.Y = PawnLocation.Y / Scale;

			for (int X = -Distance; X <= Distance; X++)
			{
				for (int Y = -Distance; Y <= Distance; Y++)
				{
					ChunkPos.X = X + PlayerPos.X;
					ChunkPos.Y = Y + PlayerPos.Y;

					if (!GeneratedChunkPos.Contains(ChunkPos))
					{						
						UClass* Chunk = AProceduralTerrainChunk::StaticClass();

						FVector SpawnLocation = FVector(ChunkPos.X * Scale, ChunkPos.Y * Scale, GetActorLocation().Z);
						AActor* SpawnedActor = GetWorld()->SpawnActor<AProceduralTerrainChunk>(Chunk, SpawnLocation, FRotator::ZeroRotator);
					
						if (SpawnedActor)
						{
							AProceduralTerrainChunk* SpawnedChunk = Cast<AProceduralTerrainChunk>(SpawnedActor);

							if (SpawnedChunk)
							{
								float VertexDist = Scale / Size;
								XOffset = Size * ChunkPos.X;
								YOffset = Size * ChunkPos.Y;

								SpawnedChunk->SetProperties(Size, Size, ZMultiplier, VertexDist, UVScale, NoiseScale);
								SpawnedChunk->SetOffset(XOffset, YOffset);
								SpawnedChunk->SetMaterial(Material);
								SpawnedChunk->SetSpawnProbability(ActorSpawnProbability);
								SpawnedChunk->SetActorToSpawn(ActorToSpawn);
								SpawnedChunk->SetFoilageActor(FoilageActor);

								SpawnedChunk->CreateMesh();
								GeneratedChunkPos.Add(ChunkPos);
							}

							else
							{
								GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Yellow, FString("Chunk Casting is failed"));
							}
						}

						else
						{
							GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Yellow, FString("Chunk Spawn is failed"));
						}
					}
				}
			}
        }
    }
}