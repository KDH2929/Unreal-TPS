#include "Item.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/BodyInstance.h"

// Sets default values
AItem::AItem()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(CollisionComponent);

    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("ItemCollider"));

    StaticMesh->SetLinearDamping(1.0f);
    StaticMesh->SetAngularDamping(5.0f);


    ItemName = "DefaultItem";
    ItemType = EItemType::General;
}

FString AItem::GetItemName()
{
    return *ItemName;
}

EItemType AItem::GetItemType()
{
    return ItemType;
}

void AItem::SetItemName(FString InItemName)
{
    ItemName = InItemName;
}

UTexture2D* AItem::GetItemTexture()
{
    return ItemTexture;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();

    
    if (StaticMesh)
    {
        StaticMesh->SetSimulatePhysics(true);

        float Mass = 100.0f;
        StaticMesh->SetMassOverrideInKg(NAME_None, Mass);
    }
    
}


void AItem::InteractWithItem(AActor* Interactor)
{

    if (Interactor)
    {
        Interactor->Destroy();
    }
    
}


// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
