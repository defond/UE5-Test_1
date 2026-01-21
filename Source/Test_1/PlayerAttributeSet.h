// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS(BlueprintType)
class TEST_1_API UPlayerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UPlayerAttributeSet();

    /** Здоровье */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth)

    /** Урон */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Damage)
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Damage)

    /** Жизни */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Lives)
    FGameplayAttributeData Lives;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Lives)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxLives)
    FGameplayAttributeData MaxLives;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxLives)

    /** Спринт */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SprintMultiplier)
    FGameplayAttributeData SprintMultiplier;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SprintMultiplier)

    /** Репликация */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:

    /** Репликация */
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);

    UFUNCTION()
    virtual void OnRep_Lives(const FGameplayAttributeData& OldLives);

    UFUNCTION()
    virtual void OnRep_MaxLives(const FGameplayAttributeData& OldMaxLives);

    UFUNCTION()
    virtual void OnRep_SprintMultiplier(const FGameplayAttributeData& OldSprintMultiplier);

private:

    void ClampAttribute(float& NewValue, float MinValue, float MaxValue);
    void ClampHealth();
    void ClampLives();
};