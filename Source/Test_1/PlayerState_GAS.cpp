// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerState_GAS.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

APlayerState_GAS::APlayerState_GAS()
{

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    AbilitySystemComponent->ReplicationMode = EGameplayEffectReplicationMode::Mixed; // Репликация для всех

    AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));

    NetUpdateFrequency = 100.0f; // Частота репликации
}

void APlayerState_GAS::BeginPlay()
{
    Super::BeginPlay();
    InitializeGAS();
}

UAbilitySystemComponent* APlayerState_GAS::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UPlayerAttributeSet* APlayerState_GAS::GetAttributeSet() const
{
    return AttributeSet;
}

void APlayerState_GAS::InitializeGAS()
{
    if (!AbilitySystemComponent || !AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("Log APlayerState_GAS - error init GAS"));
        return;
    }
    AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
    InitializeAttributes();

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data)
            {
                OnHealthChanged(Data.NewValue, Data.OldValue);
            });

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetLivesAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data)
            {
                OnLivesChangedInternal(Data.NewValue, Data.OldValue);
            });
}

void APlayerState_GAS::InitializeAttributes()
{
    if (!AbilitySystemComponent || !AttributeSet)
        return;

    /** Настройки по дефаулту - можно менять в Блюпринтах */
    AttributeSet->SetMaxHealth(InitialHealth);
    AttributeSet->SetHealth(InitialHealth);
    AttributeSet->SetDamage(BaseDamage);
    AttributeSet->SetMaxLives(static_cast<float>(InitialLives));
    AttributeSet->SetLives(static_cast<float>(InitialLives));
    AttributeSet->SetSprintMultiplier(1.0f); // Меняем через коэфициент, а не через числовое значение. Обычно так лучше и проще
}

void APlayerState_GAS::ApplyDamage(float DamageAmount, AActor* DamageInstigator)
{
    /** IsValid */
    if (!AbilitySystemComponent || !AttributeSet || DamageAmount <= 0 || !IsAlive())
        return;

    /** Эффект для урона. Не уверен, что успею сделать */
    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(DamageInstigator ? DamageInstigator : this);

    if (DamageEffectClass)
    {
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
            DamageEffectClass,
            1.0f,
            EffectContext
        );

        if (SpecHandle.IsValid())
        {
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
                DamageAmount
            );

            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
    else
    {
        /* Без эффекта */
        float NewHealth = AttributeSet->GetHealth() - DamageAmount;
        AttributeSet->SetHealth(FMath::Max(0.0f, NewHealth));
    }

    UE_LOG(LogTemp, Log, TEXT("Damage add: %.1f, Health: %.1f"), DamageAmount, AttributeSet->GetHealth());
}

void APlayerState_GAS::StartSprint()
{
    
    if (!AttributeSet || bIsSprinting || !CanSprint())
        return;

    AttributeSet->SetSprintMultiplier(SprintMultiplierValue);

    //** Спринт через Movement Component - так ээфективнее, чем через число. Не нужно парится, если для разных Character разная скорость по дефаулту */
    ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());
    if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
    {
        if (DefaultWalkSpeed == 0.0f)
        {
            DefaultWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
        }
        float BaseWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;

        PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SprintMultiplierValue;
        bIsSprinting = true;

        UE_LOG(LogTemp, Log, TEXT("Sprint start: %.1fx"), SprintMultiplierValue);
    }
}

void APlayerState_GAS::StopSprint()
{
    if (!bIsSprinting)
        return;

    AttributeSet->SetSprintMultiplier(1.0f);

    ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());
    if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
    {
        PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed; // Восстанавливаем из базового значения. Его можно переназначить в Блюпринтах
        bIsSprinting = false;

        UE_LOG(LogTemp, Log, TEXT("Sprint stopped"));
    }
}

void APlayerState_GAS::RespawnPlayer()
{
    /** Проверяем, можно ли респавнить */
    if (!AttributeSet || !CanRespawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot respawn!"));
        return;
    }

    AttributeSet->SetLives(AttributeSet->GetLives() - 1);

    if (!CanRespawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("GAME OVER!"));
        HandleDeath();
        return;
    }

    AttributeSet->SetHealth(AttributeSet->GetMaxHealth());

    DeactivateRagdoll();

    OnPlayerRespawned.Broadcast();

    UE_LOG(LogTemp, Log, TEXT("Respawn. Lives: %.0f"), AttributeSet->GetLives());
}

bool APlayerState_GAS::CanRespawn() const
{
    return AttributeSet && AttributeSet->GetLives() > 0;
}

int32 APlayerState_GAS::GetCurrentLives() const
{
    return AttributeSet ? static_cast<int32>(AttributeSet->GetLives()) : 0;
}

bool APlayerState_GAS::IsAlive() const
{
    return AttributeSet && AttributeSet->GetHealth() > 0; // Мертвые не спринтуют ))
}

bool APlayerState_GAS::CanSprint() const
{
    return IsAlive() && AttributeSet;
}

void APlayerState_GAS::OnHealthChanged(float NewValue, float OldValue)
{
    if (NewValue <= 0 && OldValue > 0)
    {
        HandleDeath();
    }

    UE_LOG(LogTemp, Log, TEXT("Damage. Health: %.1f -> %.1f"), OldValue, NewValue);
}

void APlayerState_GAS::OnLivesChangedInternal(float NewValue, float OldValue)
{
    OnLivesChanged.Broadcast(static_cast<int32>(NewValue));

    if (NewValue <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Over! ))"));
        // Тут надо вызывать виджет конца игры 
    }

    UE_LOG(LogTemp, Log, TEXT("Lives changed: %.0f -> %.0f"), OldValue, NewValue);
}

void APlayerState_GAS::HandleDeath()
{
    if (!AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("HandleDeath: AttributeSet is null!"));
        return;
    }

    OnPlayerDied.Broadcast();
    UE_LOG(LogTemp, Warning, TEXT("Player died event broadcasted"));

    ActivateRagdoll();

    if (CanRespawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("Player has lives. Respawning in %.1f seconds"), RespawnTime);
        
        GetWorld()->GetTimerManager().SetTimer(
            RespawnTimerHandle,
            this,
            &APlayerState_GAS::RestoreAfterDeath,
            RespawnTime,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GAME OVER!"));

        OnGameOver.Broadcast();

        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
    }
}

void APlayerState_GAS::RestoreAfterDeath()
{
    if (CanRespawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("RestoreAfterDeath: Attempting respawn..."));
        RespawnPlayer();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RestoreAfterDeath: Cannot respawn - no lives left!"));

        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

        OnGameOver.Broadcast();
    }
}



void APlayerState_GAS::ActivateRagdoll()
{
    ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());
    if (PlayerCharacter && PlayerCharacter->GetMesh())
    {
        PlayerCharacter->GetMesh()->SetSimulatePhysics(true);

        if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
        {
            PlayerCharacter->DisableInput(PC);
        }

        /** Отключаем коллизии */
        PlayerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

        UE_LOG(LogTemp, Log, TEXT("Ragdoll"));
    }
}

void APlayerState_GAS::DeactivateRagdoll()
{
    ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());
    if (PlayerCharacter && PlayerCharacter->GetMesh())
    {
        PlayerCharacter->GetMesh()->SetSimulatePhysics(false);

        if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
        {
            PlayerCharacter->EnableInput(PC);
        }
        
        PlayerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        /** Для обычного СкелеталМеша. Для нестандартного - надо ввести в виде переменных */
        PlayerCharacter->GetMesh()->SetRelativeLocationAndRotation(
            FVector(0, 0, -90), 
            FRotator(0, -90, 0)
        );

        UE_LOG(LogTemp, Log, TEXT("Ragdoll off"));
    }
}