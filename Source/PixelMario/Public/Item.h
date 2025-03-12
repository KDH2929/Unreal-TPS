// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction.h"
#include "Item.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	General UMETA(DisplayName = "General"),
	Weapon UMETA(DisplayName = "Weapon"),
	Healing UMETA(DisplayName = "Healing"),
	Invalid UMETA(DisplayName = "Invalid")
};


UCLASS()
class PIXELMARIO_API AItem : public AActor, public IInteraction
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	FString ItemName;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemTexture;


public:	
	// Sets default values for this actor's properties
	AItem();

	UFUNCTION(BlueprintCallable)
	FString GetItemName();


	UFUNCTION(BlueprintCallable)
	EItemType GetItemType();

	UFUNCTION(BlueprintCallable)
	void SetItemName(FString InItemName);


	UFUNCTION(BlueprintCallable)
	UTexture2D* GetItemTexture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InteractWithItem(AActor* Interactor) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;




};
