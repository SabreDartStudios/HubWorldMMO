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
    GENERATED_UCLASS_BODY()

private:
    FGameplayTag DeadTag;

    FGameplayTag FireDamageTag;
    FGameplayTag WaterDamageTag;
    FGameplayTag IceDamageTag;
    FGameplayTag LightningDamageTag;

    FGameplayTag WetTag;
    FGameplayTag BurningTag;
    FGameplayTag ColdTag;
    FGameplayTag ChargedTag;
    FGameplayTag FirestormTag;
    FGameplayTag SuperconductTag;

    FGameplayTag FrozenTag;

    FGameplayTag CriticalHitTag;

    void SetupGameplayTags();
    void HandlePreExecuteEffectDamage(bool IsCritDamage, struct FGameplayEffectModCallbackData& Data);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Internal")
        AHWGASCharacter* WhoAttackedUsLast;

	/* BEGIN AUTO GENERATED ATTRIBUTES */


    //Health
private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = Attributes, meta = (HideFromModifiers, AllowPrivateAccess = true))
        FGameplayAttributeData Health;
protected:
    UFUNCTION()
        void OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Health, OldHealth); }
public:
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Health)
    UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetHealth() const
    {
        return GetHealth();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetHealth(float NewVal)
    {
        SetHealth(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitHealth(float NewVal)
    {
        InitHealth(NewVal);
    }

    //MaxHealth
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth, Category = Attributes, Meta = (AllowPrivateAccess = true))
        FGameplayAttributeData MaxHealth;
    UFUNCTION()
        void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxHealth, OldMaxHealth); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxHealth)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxHealth() const
    {
        return GetMaxHealth();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxHealth(float NewVal)
    {
        SetMaxHealth(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxHealth(float NewVal)
    {
        InitMaxHealth(NewVal);
    }

    //HealthRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_HealthRegenRate, Category = Attributes)
        FGameplayAttributeData HealthRegenRate;
    UFUNCTION()
        void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, HealthRegenRate, OldHealthRegenRate); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, HealthRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetHealthRegenRate() const
    {
        return GetHealthRegenRate();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetHealthRegenRate(float NewVal)
    {
        SetHealthRegenRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitHealthRegenRate(float NewVal)
    {
        InitHealthRegenRate(NewVal);
    }

    //Mana
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Mana, Category = Attributes)
        FGameplayAttributeData Mana;
    UFUNCTION()
        void OnRep_Mana(const FGameplayAttributeData& OldMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Mana, OldMana); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Mana)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMana() const
    {
        return GetMana();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMana(float NewVal)
    {
        SetMana(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMana(float NewVal)
    {
        InitMana(NewVal);
    }

    //MaxMana
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxMana, Category = Attributes)
        FGameplayAttributeData MaxMana;
    UFUNCTION()
        void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxMana, OldMaxMana); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxMana)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxMana() const
    {
        return GetMaxMana();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxMana(float NewVal)
    {
        SetMaxMana(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxMana(float NewVal)
    {
        InitMaxMana(NewVal);
    }

    //ManaRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ManaRegenRate, Category = Attributes)
        FGameplayAttributeData ManaRegenRate;
    UFUNCTION()
        void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, ManaRegenRate, OldManaRegenRate); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, ManaRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetManaRegenRate() const
    {
        return GetManaRegenRate();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetManaRegenRate(float NewVal)
    {
        SetManaRegenRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitManaRegenRate(float NewVal)
    {
        InitManaRegenRate(NewVal);
    }

    //Energy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Energy, Category = Attributes)
        FGameplayAttributeData Energy;
    UFUNCTION()
        void OnRep_Energy(const FGameplayAttributeData& OldEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Energy, OldEnergy); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Energy)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetEnergy() const
    {
        return GetEnergy();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetEnergy(float NewVal)
    {
        SetEnergy(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitEnergy(float NewVal)
    {
        InitEnergy(NewVal);
    }

    //MaxEnergy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxEnergy, Category = Attributes)
        FGameplayAttributeData MaxEnergy;
    UFUNCTION()
        void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, MaxEnergy, OldMaxEnergy); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, MaxEnergy)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxEnergy() const
    {
        return GetMaxEnergy();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxEnergy(float NewVal)
    {
        SetMaxEnergy(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxEnergy(float NewVal)
    {
        InitMaxEnergy(NewVal);
    }

    //EnergyRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_EnergyRegenRate, Category = Attributes)
        FGameplayAttributeData EnergyRegenRate;
    UFUNCTION()
        void OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, EnergyRegenRate, OldEnergyRegenRate); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, EnergyRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetEnergyRegenRate() const
    {
        return GetEnergyRegenRate();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetEnergyRegenRate(float NewVal)
    {
        SetEnergyRegenRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitEnergyRegenRate(float NewVal)
    {
        InitEnergyRegenRate(NewVal);
    }

    //Strength
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Strength, Category = Attributes)
        FGameplayAttributeData Strength;
    UFUNCTION()
        void OnRep_Strength(const FGameplayAttributeData& OldStrength) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Strength, OldStrength); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Strength)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetStrength() const
    {
        return GetStrength();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetStrength(float NewVal)
    {
        SetStrength(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitStrength(float NewVal)
    {
        InitStrength(NewVal);
    }

    //Agility
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Agility, Category = Attributes)
        FGameplayAttributeData Agility;
    UFUNCTION()
        void OnRep_Agility(const FGameplayAttributeData& OldAgility) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Agility, OldAgility); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Agility)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetAgility() const
    {
        return GetAgility();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetAgility(float NewVal)
    {
        SetAgility(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitAgility(float NewVal)
    {
        InitAgility(NewVal);
    }

    //Constitution
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Constitution, Category = Attributes)
        FGameplayAttributeData Constitution;
    UFUNCTION()
        void OnRep_Constitution(const FGameplayAttributeData& OldConstitution) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Constitution, OldConstitution); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Constitution)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetConstitution() const
    {
        return GetConstitution();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetConstitution(float NewVal)
    {
        SetConstitution(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitConstitution(float NewVal)
    {
        InitConstitution(NewVal);
    }

    //CritRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritRate, Category = Attributes)
        FGameplayAttributeData CritRate;
    UFUNCTION()
        void OnRep_CritRate(const FGameplayAttributeData& OldCritRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, CritRate, OldCritRate); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, CritRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritRate() const
    {
        return GetCritRate();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritRate(float NewVal)
    {
        SetCritRate(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritRate(float NewVal)
    {
        InitCritRate(NewVal);
    }

    //CritDamage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritDamage, Category = Attributes)
        FGameplayAttributeData CritDamage;
    UFUNCTION()
        void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, CritDamage, OldCritDamage); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, CritDamage)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritDamage() const
    {
        return GetCritDamage();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritDamage(float NewVal)
    {
        SetCritDamage(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritDamage(float NewVal)
    {
        InitCritDamage(NewVal);
    }

    //CritHitDamage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritHitDamage, Category = Attributes)
        FGameplayAttributeData CritHitDamage;
    UFUNCTION()
        void OnRep_CritHitDamage(const FGameplayAttributeData& OldCritHitDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, CritHitDamage, OldCritHitDamage); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, CritHitDamage)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritHitDamage() const
    {
        return GetCritHitDamage();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritHitDamage(float NewVal)
    {
        SetCritHitDamage(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritHitDamage(float NewVal)
    {
        InitCritHitDamage(NewVal);
    }

    //Attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Attack, Category = Attributes)
        FGameplayAttributeData Attack;
    UFUNCTION()
        void OnRep_Attack(const FGameplayAttributeData& OldAttack) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Attack, OldAttack); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Attack)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetAttack() const
    {
        return GetAttack();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetAttack(float NewVal)
    {
        SetAttack(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitAttack(float NewVal)
    {
        InitAttack(NewVal);
    }

    //Defense
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Defense, Category = Attributes)
        FGameplayAttributeData Defense;
    UFUNCTION()
        void OnRep_Defense(const FGameplayAttributeData& OldDefense) { GAMEPLAYATTRIBUTE_REPNOTIFY(UHWCombatAttributeSet, Defense, OldDefense); }
    ATTRIBUTE_ACCESSORS(UHWCombatAttributeSet, Defense)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetDefense() const
    {
        return GetDefense();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetDefense(float NewVal)
    {
        SetDefense(NewVal);
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitDefense(float NewVal)
    {
        InitDefense(NewVal);
    }

	
	/* END AUTO GENERATED ATTRIBUTES */

    /** This Damage is just used for applying negative health mods. Its not a 'persistent' attribute. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeTest", meta = (HideFromLevelInfos))		// You can't make a GameplayEffect 'powered' by Damage (Its transient)
        FGameplayAttributeData	Damage;

    /** This Healing is just used for applying positive health mods. Its not a 'persistent' attribute. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeTest", meta = (HideFromLevelInfos))		// You can't make a GameplayEffect 'powered' by Healing (Its transient)
        FGameplayAttributeData	Healing;

    virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

};
