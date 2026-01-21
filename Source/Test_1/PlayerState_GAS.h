// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PlayerState_GAS.generated.h"

class UAbilitySystemComponent;
class UPlayerAttributeSet;

// Делегаты
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLivesChangedSignature, int32, NewLives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawnedSignature);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class TEST_1_API APlayerState_GAS : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    APlayerState_GAS();


    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    
    UFUNCTION(BlueprintCallable, Category = "GAS")
    UPlayerAttributeSet* GetAttributeSet() const;

    /** Урон */
    UFUNCTION(BlueprintCallable, Category = "Player|Damage")
    void ApplyDamage(float DamageAmount, AActor* DamageInstigator = nullptr);

    /** Спринт */
    UFUNCTION(BlueprintCallable, Category = "Player|Movement")
    void StartSprint();

    UFUNCTION(BlueprintCallable, Category = "Player|Movement")
    void StopSprint();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player|Movement")
    bool CanSprint() const;

    /** Смерть */
    UFUNCTION(BlueprintCallable, Category = "Player|Lifecycle")
    void RespawnPlayer();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player|Lifecycle")
    bool CanRespawn() const;

    /** Получить текущее количество жизней (целое число) */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player|State")
    int32 GetCurrentLives() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player|State")
    bool IsAlive() const;

    /** Делегаты */
    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnPlayerDiedSignature OnPlayerDied;

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnPlayerRespawnedSignature OnPlayerRespawned;

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnLivesChangedSignature OnLivesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnPlayerDiedSignature OnGameOver;

protected:
    virtual void BeginPlay() override;
    //
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UPlayerAttributeSet> AttributeSet;

    /** Настройки */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Settings", meta = (ClampMin = "1.0"))
    float InitialHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Settings", meta = (ClampMin = "1"))
    int32 InitialLives = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Settings", meta = (ClampMin = "0.0"))
    float BaseDamage = 10.0f; // Я тут не уверен в правильности такого решения - сделал сугубо для простоты. Числовой дамаг бывает крайне сложен в настройке

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Movement", meta = (ClampMin = "1.0", ClampMax = "2.0"))
    float SprintMultiplierValue = 1.5f;

    /** Делей респавна */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Settings", meta = (ClampMin = "0.5"))
    float RespawnTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS|Effects")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    /********* */

    /** Респавн */
    FTimerHandle RespawnTimerHandle;

    /** Спринт */
    bool bIsSprinting = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Settings")
    float DefaultWalkSpeed = 0.0f; // Переназначим её в блюпринтах и в начале спринта

private:
    void InitializeGAS();
    void InitializeAttributes();

    /** Жизнь */
    UFUNCTION()
    void OnHealthChanged(float NewValue, float OldValue);

    UFUNCTION()
    void OnLivesChangedInternal(float NewValue, float OldValue);

    /** Смерть */
    void HandleDeath();

    void RestoreAfterDeath();

    void ActivateRagdoll();

    void DeactivateRagdoll();
};