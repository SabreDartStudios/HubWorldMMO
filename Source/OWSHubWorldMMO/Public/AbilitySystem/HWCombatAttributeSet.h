// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsModule.h"
#include "HWAttributeSet.h"
#include "HWCombatAttributeSet.generated.h"



/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWCombatAttributeSet : public UHWAttributeSet
{
    GENERATED_BODY()

private:
    FGameplayTag DeadTag;

    FGameplayTag FireDamageTag;
    FGameplayTag WaterDamageTag;
    FGameplayTag IceDamageTag;
    FGameplayTag LightningDamageTag;

    FGameplayTag WetTag;
    FGameplayTag BurningTag;
    FGameplayTag ColdTag;
    FGameplayTag ElectrifiedTag;

    FGameplayTag ChargedTag;
    FGameplayTag FirestormTag;
    FGameplayTag SuperconductTag;

    FGameplayTag FrozenTag;

    FGameplayTag CriticalHitTag;

    void SetupGameplayTags();
    void HandlePreExecuteEffectDamage(bool IsCritDamage, struct FGameplayEffectModCallbackData& Data);

public:
    UHWCombatAttributeSet();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Internal")
        AHWGASCharacter* WhoAttackedUsLast;

    /* BEGIN AUTO GENERATED ATTRIBUTES */


//Health
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = CombatAttributes, Meta = (HideFromModifiers, AllowPrivateAccess = true))
        FGameplayAttributeData Health;
protected:
    UFUNCTION()
        void OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Health, OldHealth); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Health)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetHealth() const
    {
        return GetHealth();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetHealth(float NewVal)
    {
        SetHealth(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitHealth(float NewVal)
    {
        InitHealth(NewVal);
    }

    //MaxHealth
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData MaxHealth;
protected:
    UFUNCTION()
        void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxHealth, OldMaxHealth); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxHealth)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetMaxHealth() const
    {
        return GetMaxHealth();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetMaxHealth(float NewVal)
    {
        SetMaxHealth(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitMaxHealth(float NewVal)
    {
        InitMaxHealth(NewVal);
    }

    //HealthRegenRate
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegenRate, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData HealthRegenRate;
protected:
    UFUNCTION()
        void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, HealthRegenRate, OldHealthRegenRate); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, HealthRegenRate)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetHealthRegenRate() const
    {
        return GetHealthRegenRate();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetHealthRegenRate(float NewVal)
    {
        SetHealthRegenRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitHealthRegenRate(float NewVal)
    {
        InitHealthRegenRate(NewVal);
    }
    
    //Energy
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Energy, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Energy;
protected:
    UFUNCTION()
        void OnRep_Energy(const FGameplayAttributeData& OldEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Energy, OldEnergy); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Energy)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetEnergy() const
    {
        return GetEnergy();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetEnergy(float NewVal)
    {
        SetEnergy(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitEnergy(float NewVal)
    {
        InitEnergy(NewVal);
    }

    //MaxEnergy
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergy, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData MaxEnergy;
protected:
    UFUNCTION()
        void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxEnergy, OldMaxEnergy); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxEnergy)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetMaxEnergy() const
    {
        return GetMaxEnergy();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetMaxEnergy(float NewVal)
    {
        SetMaxEnergy(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitMaxEnergy(float NewVal)
    {
        InitMaxEnergy(NewVal);
    }

    //EnergyRegenRate
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyRegenRate, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData EnergyRegenRate;
protected:
    UFUNCTION()
        void OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, EnergyRegenRate, OldEnergyRegenRate); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, EnergyRegenRate)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetEnergyRegenRate() const
    {
        return GetEnergyRegenRate();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetEnergyRegenRate(float NewVal)
    {
        SetEnergyRegenRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitEnergyRegenRate(float NewVal)
    {
        InitEnergyRegenRate(NewVal);
    }

    //Stamina
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Stamina;
protected:
    UFUNCTION()
        void OnRep_Stamina(const FGameplayAttributeData& OldStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Stamina, OldStamina); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Stamina)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetStamina() const
    {
        return GetStamina();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetStamina(float NewVal)
    {
        SetStamina(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitStamina(float NewVal)
    {
        InitStamina(NewVal);
    }

    //MaxStamina
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData MaxStamina;
protected:
    UFUNCTION()
        void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxStamina, OldMaxStamina); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxStamina)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetMaxStamina() const
    {
        return GetMaxStamina();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetMaxStamina(float NewVal)
    {
        SetMaxStamina(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitMaxStamina(float NewVal)
    {
        InitMaxStamina(NewVal);
    }

    //Strength
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Strength;
protected:
    UFUNCTION()
        void OnRep_Strength(const FGameplayAttributeData& OldStrength) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Strength, OldStrength); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Strength)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetStrength() const
    {
        return GetStrength();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetStrength(float NewVal)
    {
        SetStrength(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitStrength(float NewVal)
    {
        InitStrength(NewVal);
    }

    //Agility
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Agility, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Agility;
protected:
    UFUNCTION()
        void OnRep_Agility(const FGameplayAttributeData& OldAgility) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Agility, OldAgility); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Agility)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetAgility() const
    {
        return GetAgility();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetAgility(float NewVal)
    {
        SetAgility(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitAgility(float NewVal)
    {
        InitAgility(NewVal);
    }

    //Constitution
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Constitution, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Constitution;
protected:
    UFUNCTION()
        void OnRep_Constitution(const FGameplayAttributeData& OldConstitution) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Constitution, OldConstitution); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Constitution)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetConstitution() const
    {
        return GetConstitution();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetConstitution(float NewVal)
    {
        SetConstitution(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitConstitution(float NewVal)
    {
        InitConstitution(NewVal);
    }

    //CritRate
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritRate, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData CritRate;
protected:
    UFUNCTION()
        void OnRep_CritRate(const FGameplayAttributeData& OldCritRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, CritRate, OldCritRate); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, CritRate)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetCritRate() const
    {
        return GetCritRate();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetCritRate(float NewVal)
    {
        SetCritRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitCritRate(float NewVal)
    {
        InitCritRate(NewVal);
    }

    //CritDamage
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData CritDamage;
protected:
    UFUNCTION()
        void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, CritDamage, OldCritDamage); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, CritDamage)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetCritDamage() const
    {
        return GetCritDamage();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetCritDamage(float NewVal)
    {
        SetCritDamage(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitCritDamage(float NewVal)
    {
        InitCritDamage(NewVal);
    }

    //Attack
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Attack;
protected:
    UFUNCTION()
        void OnRep_Attack(const FGameplayAttributeData& OldAttack) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Attack, OldAttack); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Attack)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetAttack() const
    {
        return GetAttack();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetAttack(float NewVal)
    {
        SetAttack(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitAttack(float NewVal)
    {
        InitAttack(NewVal);
    }

    //Defense
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = CombatAttributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData Defense;
protected:
    UFUNCTION()
        void OnRep_Defense(const FGameplayAttributeData& OldDefense) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Defense, OldDefense); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Defense)
        UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        float HWGetDefense() const
    {
        return GetDefense();
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWSetDefense(float NewVal)
    {
        SetDefense(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = CombatAttributes)
        void HWInitDefense(float NewVal)
    {
        InitDefense(NewVal);
    }


    /* END AUTO GENERATED ATTRIBUTES */

private:
    /** This Damage is just used for applying negative health mods. Its not a 'persistent' attribute. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatAttributes, meta = (HideFromLevelInfos, AllowPrivateAccess = true))		// You can't make a GameplayEffect 'powered' by Damage (Its transient)
        FGameplayAttributeData	Damage;
    /** This Healing is just used for applying positive health mods. Its not a 'persistent' attribute. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatAttributes, meta = (HideFromLevelInfos, AllowPrivateAccess = true))		// You can't make a GameplayEffect 'powered' by Healing (Its transient)
        FGameplayAttributeData	Healing;

public:

    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Damage)
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Healing)

protected:

    virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

};
