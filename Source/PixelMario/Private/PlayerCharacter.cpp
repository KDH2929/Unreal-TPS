

#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerCharacterAnim.h"
#include "Interaction.h"
#include "GunInterface.h"
#include "SpectatorCamera.h"
#include "UI_HUD.h"
#include "UI_DynamicCrosshair.h"
#include "Item.h"
#include "WeaponItem.h"
#include "Gun.h"


APlayerCharacter::APlayerCharacter()
{
    AnimState = EAnimState::Default;
    CameraState = ECameraState::ThirdPerson;


    static ConstructorHelpers::FObjectFinder<UInputAction> IA_ChangeCam(TEXT("InputAction'/Game/ThirdPerson/Input/Actions/IA_FreeCam.IA_FreeCam'"));
    if (IA_ChangeCam.Succeeded())
    {
        ChangeFreeCam_Action = IA_ChangeCam.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> IA_MouseScroll(TEXT("InputAction'/Game/ThirdPerson/Input/Actions/IA_MouseScroll.IA_MouseScroll'"));
    if (IA_MouseScroll.Succeeded())
    {
        MouseScroll_Action = IA_MouseScroll.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> IA_Consume(TEXT("InputAction'/Game/ThirdPerson/Input/Actions/IA_Consume.IA_Consume'"));
    if (IA_Consume.Succeeded())
    {
        ItemConsume_Action = IA_Consume.Object;
    }


    // ThirdPersonCamera
    ThirdPersonCameraBoomWithGun = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraBoomWithGun"));
    ThirdPersonCameraBoomWithGun->SetupAttachment(RootComponent);
    ThirdPersonCameraBoomWithGun->TargetArmLength = 100.0f;
    ThirdPersonCameraBoomWithGun->bUsePawnControlRotation = true;

    ThirdPersonCameraWithGun = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCameraWithGun"));
    ThirdPersonCameraWithGun->SetupAttachment(ThirdPersonCameraBoomWithGun);
    ThirdPersonCameraWithGun->SetRelativeLocation(FVector(0.f, 40.f, 30.f));
    ThirdPersonCameraWithGun->bUsePawnControlRotation = false;



    // FirstPersonCamera
    FirstPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonCameraBoom"));
    FirstPersonCameraBoom->SetupAttachment(RootComponent);
    FirstPersonCameraBoom->TargetArmLength = 0.0f;
    FirstPersonCameraBoom->bUsePawnControlRotation = true;
    

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(FirstPersonCameraBoom);
    FirstPersonCamera->SetRelativeLocation(FVector(23.f, 0.f, 50.f));
    FirstPersonCamera->bUsePawnControlRotation = false;
    

    // Camera Active
    FollowCamera->SetActive(true);
    ThirdPersonCameraWithGun->SetActive(false);
    FirstPersonCamera->SetActive(false);

    CurrentCamera = FollowCamera;


    MaxHP = 100;
    CurrentHP = MaxHP;

}

void APlayerCharacter::EquipWeapon(TSubclassOf<AWeapon> WeaponClass)
{

    if (WeaponClass)
    {
        DropWeapon();

        AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorLocation(), GetActorRotation());

        if (SpawnedWeapon)
        {
            CurrentWeapon = SpawnedWeapon;
            WeaponSocketName = CurrentWeapon->GetWeaponSocketName();
            EWeaponType WeaponType = CurrentWeapon->GetWeaponType();

            if (GetMesh()->DoesSocketExist(WeaponSocketName))
            {
                CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
                SetAnimStateByWeaponType(WeaponType);
                SetCameraStateByWeaponType(WeaponType);
                ChangeCamera();
                SetCrosshairVisibility(true);

                if (CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))
                {
                    AGun* Gun = Cast<AGun>(CurrentWeapon);

                    if (GetMesh()->GetAnimInstance())
                    {
                        UPlayerCharacterAnim* AnimInstance = Cast<UPlayerCharacterAnim>(GetMesh()->GetAnimInstance());
                        if (AnimInstance)
                        {
                            AnimInstance->BindToGun(Gun);               // 델리게이트 바인딩
                        }
                    }

                    Gun->BindToPlayerMouseMovement(this);
                }
            }

            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Weapon Socket Name is invalid");
            }

        }
    }
}

void APlayerCharacter::EquipWeapon(AWeaponItem* WeaponItem)
{
    if (WeaponItem)
    {
        TSubclassOf<AWeapon> WeaponClass = WeaponItem->GetWeaponClass();

        EquipWeapon(WeaponClass);
    }

    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Weapon Equip is failed");
    }
}


void APlayerCharacter::DropWeapon()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;

        ZoomOut();
        SetCrosshairVisibility(false);
    }
}


EAnimState APlayerCharacter::GetAnimState()
{
    return AnimState;
}

FTransform APlayerCharacter::GetLeftHandIKData()
{
    if (CurrentWeapon)
    {
        AGun* CurrentGun = Cast<AGun>(CurrentWeapon);
        if (CurrentGun->GetClass()->ImplementsInterface(UGunInterface::StaticClass()))
        {
            IGunInterface* GunInterface = Cast<IGunInterface>(CurrentGun);

            USkinnedMeshComponent* CharacterMeshComponent = GetMesh();
            FTransform LHIKDataWS = GunInterface->GetLeftHandIKData();
            FVector InPosition = LHIKDataWS.GetLocation();
            FRotator InRotation = LHIKDataWS.GetRotation().Rotator();

            FVector OutPosition = FVector::ZeroVector;
            FRotator OutRotation = FRotator::ZeroRotator; 

            if (GetMesh()->GetBoneIndex(RightHandBoneName) != INDEX_NONE)
            {
                if (CharacterMeshComponent)
                {
                    CharacterMeshComponent->TransformToBoneSpace(RightHandBoneName, InPosition, InRotation, OutPosition, OutRotation);
                }

                FTransform LHIKDataBS;

                LHIKDataBS.SetLocation(OutPosition);
                LHIKDataBS.SetRotation(OutRotation.Quaternion());
                LHIKDataBS.SetScale3D(FVector(1, 1, 1));

                return LHIKDataBS;
            }

            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "RightHandBoneName is invalid");
                FString DebugMessage = FString::Printf(TEXT("Invalid RightHandBoneName is %s"), *RightHandBoneName.ToString());
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMessage);
            }
        }
    }

    return FTransform::Identity;
}

AWeapon* APlayerCharacter::GetCurrentWeapon()
{
    return CurrentWeapon;
}

void APlayerCharacter::SetCrosshairVisibility(bool bActive)
{
    if (UI_Crosshair)
    {
        if (bActive == true)
        {
            if (CurrentWeapon && CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))
            {
                UI_Crosshair->SetVisibility(ESlateVisibility::Visible);
            }

            else
            {
                UI_Crosshair->SetVisibility(ESlateVisibility::Hidden);
            }
        }

        else
        {
            UI_Crosshair->SetVisibility(ESlateVisibility::Hidden);
        }

    }
}

void APlayerCharacter::SetHUDVisibility(bool bActive)
{
    if (UI_HUD)
    {
        UI_HUD->SetVisibility(bActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    Inventory = GetWorld()->SpawnActor<AInventory>(AInventory::StaticClass(), FTransform());

    if (Inventory)
    {
        Inventory->Clear();
        Inventory->SetMaxInventorySize(6);
        Inventory->SetCurrentInventoryIndex(0);
        Inventory->OnWeaponRemoved.AddUObject(this, &APlayerCharacter::OnInventoryWeaponRemoved);
    }

    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Inventory spawn is failed");
    }


    if (UI_HUDClass)
    {
        UI_HUD = CreateWidget<UUI_HUD>(GetWorld(), UI_HUDClass);
        if (UI_HUD)
        {
            UI_HUD->AddToViewport();
            UI_HUD->BindToInventory(Inventory);
            UI_HUD->BindToCharacter(this);
        }
    }


    if (UI_CrosshairClass)
    {
        UI_Crosshair = CreateWidget<UUI_DynamicCrosshair>(GetWorld(), UI_CrosshairClass);

        if (UI_Crosshair)
        {
            UI_Crosshair->AddToViewport();
            SetCrosshairVisibility(false);
        }
    }

    if (UI_SniperZoomClass)
    {
        UI_SniperZoom = CreateWidget<UUserWidget>(GetWorld(), UI_SniperZoomClass);
        if (UI_SniperZoom)
        {
            UI_SniperZoom->AddToViewport();
            UI_SniperZoom->SetVisibility(ESlateVisibility::Hidden);
        }
    }

}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (UI_Crosshair)
    {
        if (CurrentWeapon && CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))
        {
            float Speed = GetVelocity().Size();
            float ClampedSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 600.f), FVector2D(5.f, 100.f), Speed);

            AGun* CurrentGun = Cast<AGun>(CurrentWeapon);

            CurrentGun->AddAimSpread(ClampedSpeed);
            

            float CrosshairSpread = CurrentGun->GetCrosshairSpread();

            UI_Crosshair->SetCrosshairSpread(CrosshairSpread);

        }
    }
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
       
        //Interact
        EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

        //ItemThrow
        EnhancedInputComponent->BindAction(ItemThrowAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ItemThrow);

        //LeftMouseAction
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Canceled, this, &APlayerCharacter::SetAutoFireDisabled);
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetAutoFireDisabled);


        //RightMouseAction
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ZoomIn);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Canceled, this, &APlayerCharacter::ZoomOut);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Completed, this, &APlayerCharacter::ZoomOut);

        // ChangeCam
        EnhancedInputComponent->BindAction(ChangeFreeCam_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeFreeCam);

        // MouseScroll
        EnhancedInputComponent->BindAction(MouseScroll_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::MouseScrollZoom);

        // ItemConsume
        EnhancedInputComponent->BindAction(ItemConsume_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::ItemConsume);

        //KeyNum1
        EnhancedInputComponent->BindAction(KeyNum1_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot1Change);

        //KeyNum2
        EnhancedInputComponent->BindAction(KeyNum2_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot2Change);

        //KeyNum3
        EnhancedInputComponent->BindAction(KeyNum3_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot3Change);

        //KeyNum4
        EnhancedInputComponent->BindAction(KeyNum4_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot4Change);

        //KeyNum5
        EnhancedInputComponent->BindAction(KeyNum5_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot5Change);

        //KeyNum6
        EnhancedInputComponent->BindAction(KeyNum6_Action, ETriggerEvent::Triggered, this, &APlayerCharacter::InventorySlot6Change);
    }
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
    if (CameraState == ECameraState::ThirdPerson)
    {
        FVector2D MovementVector = Value.Get<FVector2D>();

        if (Controller != nullptr)
        {
            // find out which way is forward
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

            AddMovementInput(ForwardDirection, MovementVector.Y);
            AddMovementInput(RightDirection, MovementVector.X);
        }
    }

    else if (CameraState == ECameraState::ThirdPersonWithGun || CameraState == ECameraState::FirstPerson)
    {

        FVector2D MovementVector = Value.Get<FVector2D>();

        if (Controller != nullptr)
        {
            AddMovementInput(GetActorForwardVector(), MovementVector.Y);
            AddMovementInput(GetActorRightVector(), MovementVector.X);
        }
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);

        if (OnMouseMoved.IsBound())
        {
            OnMouseMoved.Broadcast();
        }
    }
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

    float ReceivedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHP -= ReceivedDamage;

    if (CurrentHP <= 0 && !bIsDead)
    {
        CurrentHP = 0;
        Die();

        bIsDead = true;
    }

    float HPPercentage = CurrentHP / MaxHP;
    OnHPChanged.Broadcast(HPPercentage);

    return ReceivedDamage;
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
    FVector CameraLocation;
    FRotator CameraRotation;
    GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);


    FVector StartLocation = GetActorLocation();
    FVector EndLocation = GetActorLocation();

    if (CameraState == ECameraState::ThirdPerson)
    {
        StartLocation = GetActorLocation();
        EndLocation = StartLocation + CurrentCamera->GetForwardVector() * 500;
    }

    else if (CameraState == ECameraState::ThirdPersonWithGun)
    {
        FVector CurrentCameraLocation = CurrentCamera->GetComponentLocation();
        StartLocation = CurrentCameraLocation + (CurrentCamera->GetForwardVector() * ThirdPersonCameraBoomWithGun->TargetArmLength);
        EndLocation = StartLocation + (CurrentCamera->GetForwardVector() * 500);
    }


    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);


    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        CollisionParams
    );


    if (bHit)
    {
        if (HitResult.GetComponent())
        {
            UPrimitiveComponent* HitComponent = HitResult.GetComponent();
            AActor* HitActor = HitComponent->GetOwner();

            if (HitActor &&  HitActor->GetClass()->ImplementsInterface(UInteraction::StaticClass())) {

                IInteraction* Interaction = Cast<IInteraction>(HitResult.GetActor());

                if (Interaction)
                {
                    AItem* HitItem = Cast<AItem>(HitActor);

                    if (HitItem)
                    {
                        int AddedItemIndex = Inventory->AddItem(HitItem);

                        if (AddedItemIndex != INDEX_NONE)                                // 아이템을 인벤토리에 추가
                        {
                            Interaction->InteractWithItem(HitResult.GetActor());        // 아이템 처리

                            // 현재 인벤토리에 들어간 아이템의 인덱스와 현재 인벤토리의 인덱스가 같을 시 처리
                            
                            if (HitItem->GetItemType() == EItemType::Weapon && AddedItemIndex == Inventory->GetCurrentInventoryIndex())
                            {
                                ChangeWeaponByIndex(AddedItemIndex);
                            }
                        }
                    }

                    
                }
            }


        }
    }
}

void APlayerCharacter::ItemThrow(const FInputActionValue& Value)
{
    int CurrentInventoryIndex = Inventory->GetCurrentInventoryIndex();

    if (Inventory && Inventory->GetCurrentInventorySize() > 0)
    {

        TSubclassOf<AItem> ItemToThrow = Inventory->Items[CurrentInventoryIndex];
        FVector ThrowLocation = GetActorLocation() + GetActorForwardVector() * 100;

        if (ItemToThrow)
        {
            FActorSpawnParameters SpawnParams;
            if (GetWorld()->SpawnActor<AItem>(ItemToThrow, ThrowLocation, FRotator::ZeroRotator, SpawnParams)) 
            {
                if (Inventory->RemoveItemByIndex(CurrentInventoryIndex))
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Item is removed successfully");
                }

                else
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Removing Item is failed");
                }
            }

            else 
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Item spawn is failed");
            }

        }

        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Item is not valid");
        }

    }

    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "No item to throw");
    }

     
}

void APlayerCharacter::InventorySlot1Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot1");
    Inventory->SetCurrentInventoryIndex(0);
    
    EItemType ItemType = Inventory->GetItemTypeByIndex(0);
    
    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(0);
    }

}

void APlayerCharacter::InventorySlot2Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot2");
    Inventory->SetCurrentInventoryIndex(1);
    
    EItemType ItemType = Inventory->GetItemTypeByIndex(1);

    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(1);
    }
}

void APlayerCharacter::InventorySlot3Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot3");
    Inventory->SetCurrentInventoryIndex(2);
    
    EItemType ItemType = Inventory->GetItemTypeByIndex(2);

    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(2);
    }
}

void APlayerCharacter::InventorySlot4Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot4");
    Inventory->SetCurrentInventoryIndex(3);
    
    EItemType ItemType = Inventory->GetItemTypeByIndex(3);

    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(3);
    }
}

void APlayerCharacter::InventorySlot5Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot5");
    Inventory->SetCurrentInventoryIndex(4);
    

    EItemType ItemType = Inventory->GetItemTypeByIndex(4);

    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(4);
    }
}

void APlayerCharacter::InventorySlot6Change(const FInputActionValue& Value)
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Slot6");
    Inventory->SetCurrentInventoryIndex(5);
    
    EItemType ItemType = Inventory->GetItemTypeByIndex(5);

    if (ItemType == EItemType::Weapon)
    {
        ChangeWeaponByIndex(5);
    }
}

void APlayerCharacter::OnInventoryWeaponRemoved(TSubclassOf<AWeapon> RemovedWeaponClass)
{
    if ((CurrentWeapon != nullptr) && (RemovedWeaponClass == CurrentWeapon->GetClass()))        // GetClass() 유효성검사 필요
    {
        AnimState = EAnimState::Default;
        CameraState = ECameraState::ThirdPerson;
        DropWeapon();
        ChangeCamera();
    }
}


void APlayerCharacter::ChangeWeaponByIndex(int Index)
{
    TSubclassOf<AItem> ItemClass = Inventory->GetItemByIndex(Index);

    if (ItemClass != nullptr)
    {
        AItem* Item = ItemClass->GetDefaultObject<AItem>();

        EItemType ItemType = Item->GetItemType();

        ZoomOut();
        SetCrosshairVisibility(false);

        if (ItemType == EItemType::Weapon)
        {
            AWeaponItem* WeaponItem = ItemClass->GetDefaultObject<AWeaponItem>();
            EquipWeapon(WeaponItem);
        }

        else
        {
            AnimState = EAnimState::Default;
            CameraState = ECameraState::ThirdPerson;
            ChangeCamera();
        }
    }
}

void APlayerCharacter::SetAnimStateByWeaponType(EWeaponType InWeaponType)
{

    if (InWeaponType == EWeaponType::Pistol)
    {
        AnimState = EAnimState::Pistol;
    }

    else if (InWeaponType == EWeaponType::Rifle)
    {
        AnimState = EAnimState::Rifle;
    }

    else if (InWeaponType == EWeaponType::SniperRifle)
    {
        AnimState = EAnimState::Rifle;
    }


    else if (InWeaponType == EWeaponType::Shotgun)
    {
        AnimState = EAnimState::Rifle;
    }

    else
    {
        AnimState = EAnimState::Default;
    }
}

void APlayerCharacter::SetCameraStateByWeaponType(EWeaponType InWeaponType)
{
    if (InWeaponType == EWeaponType::Pistol)
    {
        CameraState = ECameraState::ThirdPersonWithGun;
    }

    else if (InWeaponType == EWeaponType::Rifle)
    {
        CameraState = ECameraState::ThirdPersonWithGun;
    }

    else if (InWeaponType == EWeaponType::SniperRifle)
    {
        CameraState = ECameraState::ThirdPersonWithGun;
    }


    else if (InWeaponType == EWeaponType::Shotgun)
    {
        CameraState = ECameraState::ThirdPersonWithGun;
    }

    else
    {
        CameraState = ECameraState::ThirdPerson;
    }
}

ECameraState APlayerCharacter::GetCameraState()
{
    return CameraState;
}

void APlayerCharacter::SetCameraState(ECameraState InCameraState)
{
    CameraState = InCameraState;
}

void APlayerCharacter::SetCurrentCamera(UCameraComponent* InCameraComponent)
{
    CurrentCamera = InCameraComponent;
}

void APlayerCharacter::ChangeCamera()
{
    if (CameraState == ECameraState::ThirdPerson)
    {
        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        GetCharacterMovement()->bOrientRotationToMovement = true;

        FollowCamera->SetActive(true);
        ThirdPersonCameraWithGun->SetActive(false);
        FirstPersonCamera->SetActive(false);

        CurrentCamera = FollowCamera;
    }

    else if (CameraState == ECameraState::ThirdPersonWithGun)
    {
        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;

        GetCharacterMovement()->bOrientRotationToMovement = false;

        FollowCamera->SetActive(false);
        ThirdPersonCameraWithGun->SetActive(true);
        FirstPersonCamera->SetActive(false);

        CurrentCamera = ThirdPersonCameraWithGun;
    }

    else if (CameraState == ECameraState::FirstPerson)
    {
        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;

        GetCharacterMovement()->bOrientRotationToMovement = false;

        FollowCamera->SetActive(false);
        ThirdPersonCameraWithGun->SetActive(false);
        FirstPersonCamera->SetActive(true);

        CurrentCamera = FirstPersonCamera;
    }
}

void APlayerCharacter::Attack()
{
    bAutoFireEnabled = true;
    Fire();
}


void APlayerCharacter::Fire()
{
    if (CurrentWeapon && CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))
    {
        AGun* CurrentGun = Cast<AGun>(CurrentWeapon);

        EWeaponType CurrentWeaponType = CurrentGun->GetWeaponType();
        
        float ShootDelay = CurrentGun->GetShootDelay();

        if (bFireEnabled)
        {
            if (CurrentWeaponType == EWeaponType::Pistol || CurrentWeaponType == EWeaponType::SniperRifle)
            {
                HitscanFire();
                bFireEnabled = false;
                GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::SetFireEnabled, ShootDelay, false);
            }

            else if (CurrentWeaponType == EWeaponType::Rifle)
            {
                HitscanFire();

                if (bAutoFireEnabled)
                {
                    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::Fire, ShootDelay, false);
                }
            }

            else if (CurrentWeaponType == EWeaponType::Shotgun)
            {
                for (int Firenum = 0; Firenum < 10; Firenum++)
                {
                    HitscanFire();
                }

                bFireEnabled = false;
                GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::SetFireEnabled, ShootDelay, false);
            }
        }
    }
}

void APlayerCharacter::HitscanFire()
{
    AGun* CurrentGun = Cast<AGun>(CurrentWeapon);

    if (CurrentGun->GetClass()->ImplementsInterface(UGunInterface::StaticClass()))
    {
        IGunInterface* GunInterface = Cast<IGunInterface>(CurrentGun);

        GunInterface->GunFire(CurrentGun);

        float WeaponRange = CurrentGun->GetWeaponRange();
        float WeaponDamage = CurrentGun->GetWeaponDamage();
        
        FVector CameraLocation;
        FRotator CameraRotation;
        GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
        

        FVector MuzzleFlashLocation = GunInterface->GetMuzzleFlashLocation();
        FVector CurrentCameraLocation = CurrentCamera->GetComponentLocation();
        FVector ShootOffset = CurrentGun->GetLinetraceShootOffsetVector(CurrentCamera->GetRightVector(), CurrentCamera->GetUpVector());

        if (CurrentGun->GetWeaponType() == EWeaponType::SniperRifle && bZoomIn)
        {
            ShootOffset = FVector::ZeroVector;
        }

        FVector StartLocation = CurrentCameraLocation + (CurrentCamera->GetForwardVector() * ThirdPersonCameraBoomWithGun->TargetArmLength);
        FVector EndLocation = StartLocation + (CurrentCamera->GetForwardVector() * WeaponRange) + ShootOffset;


        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;

        CollisionParams.AddIgnoredActor(this);
        CollisionParams.AddIgnoredActor(CurrentGun);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            EndLocation,
            ECC_GameTraceChannel2,
            CollisionParams
        );

        DrawDebugLine(
            GetWorld(),
            StartLocation,
            EndLocation,
            FColor::Red,
            false,
            2.0f,
            0,
            1.0f
        );

        if (bHit)
        {
            if (HitResult.GetActor())
            {
                FString ActorName = HitResult.GetActor()->GetName();
                AActor* HitActor = HitResult.GetActor();

                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Hit Actor: %s"), *ActorName));
                float AppliedDamage = UGameplayStatics::ApplyDamage(HitActor, WeaponDamage, GetInstigatorController(), this, UDamageType::StaticClass());


                if (BulletDecal)
                {
                    FVector DecalLocation = HitResult.Location;
                    FRotator DecalRotation = HitResult.ImpactNormal.Rotation();
                    FVector DecalSize = FVector(8.0f, 8.0f, 8.0f);
                    float DecalDuration = 10.0f;


                    UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
                        GetWorld(),
                        BulletDecal,
                        DecalSize,
                        DecalLocation,
                        DecalRotation,
                        DecalDuration
                    );

                    
                    if (Decal)
                    {
                        Decal->SetFadeScreenSize(0);
                    }
                }
            }
        }


    }
}

void APlayerCharacter::SetFireEnabled()
{
    bFireEnabled = true;
}

void APlayerCharacter::SetAutoFireDisabled()
{
    bAutoFireEnabled = false;
}

void APlayerCharacter::ZoomIn()
{
    if (CurrentWeapon && CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))         // 총을 소지한 상태에서만 Zoom 가능
    {
        bZoomIn = true;


        // Bone을 숨길시 그림자도 해당 부분이 없어짐
        // SkeletalMeshComponent->HideBone(SkeletalMeshComponent->GetBoneIndex(BoneNameToHide), PBO_None);

        CameraState = ECameraState::FirstPerson;
        ChangeCamera();

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        UCameraComponent* WeaponCamera = CurrentWeapon->GetWeaponCamera();
        CurrentCamera = WeaponCamera;

        if (PlayerController)
        {
            PlayerController->SetViewTarget(CurrentWeapon);
        }

        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "PlayerController is invalid");
        }


        if(CurrentWeapon->GetWeaponType() == EWeaponType::SniperRifle)
        {
            if (UI_SniperZoom)
            {
                UI_SniperZoom->SetVisibility(ESlateVisibility::Visible);
                SetHUDVisibility(false);
                SetCrosshairVisibility(false);
            }
        }

    }
}

void APlayerCharacter::ZoomOut()
{
    bZoomIn = false;

    if (UI_SniperZoom)
    {
        UI_SniperZoom->SetVisibility(ESlateVisibility::Hidden);
        SetHUDVisibility(true);
    }


    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetViewTarget(PlayerController->GetPawn());
    }

    if (CurrentWeapon && CurrentWeapon->GetClass()->IsChildOf(AGun::StaticClass()))
    {
        CameraState = ECameraState::ThirdPersonWithGun;
        ChangeCamera();
        SetCrosshairVisibility(true);
    }

    else
    {
        CameraState = ECameraState::ThirdPerson;
        ChangeCamera();
        SetCrosshairVisibility(false);
    }

}

void APlayerCharacter::MouseScrollZoom(const FInputActionValue& Value)
{
    float ZoomValue = Value.Get<float>();

    // 3인칭 시점
    if (GetFollowCamera()->IsActive())
    {
        float TargetArmLength = GetCameraBoom()->TargetArmLength;

        GetCameraBoom()->TargetArmLength = FMath::Clamp(TargetArmLength + ZoomValue * (-20), 200.0f, 400.0f);
    }

    // 무기 든 3인칭 시점

    if (ThirdPersonCameraWithGun->IsActive())
    {
        float TargetArmLength = ThirdPersonCameraBoomWithGun->TargetArmLength;

        ThirdPersonCameraBoomWithGun->TargetArmLength = FMath::Clamp(TargetArmLength + ZoomValue * (-10), 30.0f, 100.0f);
    }
}

void APlayerCharacter::ChangeFreeCam()
{
    FTransform SpawnTransform = GetFollowCamera()->GetComponentTransform();

    AActor* SpawnedCamera = GetWorld()->SpawnActor<ASpectatorCamera>(ASpectatorCamera::StaticClass(), SpawnTransform);

    AController* PlayerController = GetController();

    if (SpawnedCamera)
    {
        SetHUDVisibility(false);
        SetCrosshairVisibility(false);
        PlayerController->Possess(Cast<ASpectatorCamera>(SpawnedCamera));
    }

}

void APlayerCharacter::ItemConsume()
{
    int CurrentInventoryIndex = Inventory->GetCurrentInventoryIndex();
    EItemType ItemType = Inventory->GetItemTypeByIndex(CurrentInventoryIndex);

    if (ItemType == EItemType::Healing)
    {
        if (CurrentHP < MaxHP)
        {
            CurrentHP = FMath::Clamp(CurrentHP + 20, 0.0f, MaxHP);
            Inventory->RemoveItemByIndex(CurrentInventoryIndex);

            float HPPercentage = CurrentHP / MaxHP;
            OnHPChanged.Broadcast(HPPercentage);
        }
    }
}

void APlayerCharacter::Die()
{
    SetCrosshairVisibility(false);
    UI_SniperZoom->SetVisibility(ESlateVisibility::Hidden);

    SetCameraState(ECameraState::ThirdPerson);
    ChangeCamera();

    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = false;


    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            InputSubsystem->RemoveMappingContext(DefaultMappingContext);

            // 만약 다른 키는 동작하게 하고 싶다면 다른 Mapping Context를 추가하는 식으로 처리하면 될 듯
            // InputSubsystem->AddMappingContext( ... )   
        }
    }

    bFireEnabled = false;
    bAutoFireEnabled = false;
       
    AnimState = EAnimState::Dead;

    GetWorldTimerManager().SetTimer(DeadTimerHandle, this, &APlayerCharacter::CreateDeadUI, 2.5f, false);

}


void APlayerCharacter::CreateDeadUI()
{
    if (UI_DeadClass)
    {
        UI_Dead = CreateWidget<UUserWidget>(GetWorld(), UI_DeadClass);
        if (UI_Dead)
        {
            UI_Dead->AddToViewport();
        }

        bool bIsPaused = UGameplayStatics::IsGamePaused(GetWorld());
        UGameplayStatics::SetGamePaused(GetWorld(), !bIsPaused);
      
    }
}
