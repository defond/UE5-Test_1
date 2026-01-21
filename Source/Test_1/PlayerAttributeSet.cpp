// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
    /** Настройки - их переопределяем так же в блюпринтах */
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitDamage(10.0f);
    InitLives(3.0f);
    InitMaxLives(3.0f);
    InitSprintMultiplier(1.0f);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Lives, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxLives, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SprintMultiplier, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    /** IsValid */
    if (Attribute == GetHealthAttribute())
    {
        ClampAttribute(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetMaxHealthAttribute())
    {
        ClampAttribute(NewValue, 1.0f, 1000.0f);
    }
    else if (Attribute == GetDamageAttribute())
    {
        ClampAttribute(NewValue, 0.0f, 1000.0f);
    }
    else if (Attribute == GetLivesAttribute())
    {
        ClampAttribute(NewValue, 0.0f, GetMaxLives());
    }
    else if (Attribute == GetMaxLivesAttribute())
    {
        ClampAttribute(NewValue, 1.0f, 10.0f);
    }
    else if (Attribute == GetSprintMultiplierAttribute())
    {
        ClampAttribute(NewValue, 1.0f, 2.0f);
    }
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    /** Постэффект */
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        ClampHealth();
    }
    else if (Data.EvaluatedData.Attribute == GetLivesAttribute())
    {
        ClampLives();
    }
}

void UPlayerAttributeSet::ClampAttribute(float& NewValue, float MinValue, float MaxValue)
{
    NewValue = FMath::Clamp(NewValue, MinValue, MaxValue);
}

void UPlayerAttributeSet::ClampHealth()
{
    SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
}

void UPlayerAttributeSet::ClampLives()
{
    SetLives(FMath::Clamp(GetLives(), 0.0f, GetMaxLives()));
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, OldMaxHealth);
}

void UPlayerAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Damage, OldDamage);
}

void UPlayerAttributeSet::OnRep_Lives(const FGameplayAttributeData& OldLives)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Lives, OldLives);
}

void UPlayerAttributeSet::OnRep_MaxLives(const FGameplayAttributeData& OldMaxLives)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxLives, OldMaxLives);
}

void UPlayerAttributeSet::OnRep_SprintMultiplier(const FGameplayAttributeData& OldSprintMultiplier)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SprintMultiplier, OldSprintMultiplier);
}