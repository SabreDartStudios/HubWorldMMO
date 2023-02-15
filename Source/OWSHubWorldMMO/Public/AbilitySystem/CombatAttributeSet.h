// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsModule.h"
#include "CombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//Forward declare to stop circular reference
class AHWGASCharacter;

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UCombatAttributeSet : public UAttributeSet
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Health, Category = Attributes, meta = (HideFromModifiers))
        FGameplayAttributeData Health;
    UFUNCTION()
        void OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Health, OldHealth); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetHealth() const
    {
        return Health.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetHealth(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetHealthAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitHealth(float NewVal)
    {
        Health.SetBaseValue(NewVal);
        Health.SetCurrentValue(NewVal);
    }

    //MaxHealth
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth, Category = Attributes)
        FGameplayAttributeData MaxHealth;
    UFUNCTION()
        void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxHealth, OldMaxHealth); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxHealth() const
    {
        return MaxHealth.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxHealth(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetMaxHealthAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxHealth(float NewVal)
    {
        MaxHealth.SetBaseValue(NewVal);
        MaxHealth.SetCurrentValue(NewVal);
    }

    //HealthRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_HealthRegenRate, Category = Attributes)
        FGameplayAttributeData HealthRegenRate;
    UFUNCTION()
        void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HealthRegenRate, OldHealthRegenRate); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HealthRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetHealthRegenRate() const
    {
        return HealthRegenRate.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetHealthRegenRate(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetHealthRegenRateAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitHealthRegenRate(float NewVal)
    {
        HealthRegenRate.SetBaseValue(NewVal);
        HealthRegenRate.SetCurrentValue(NewVal);
    }

    //Mana
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Mana, Category = Attributes)
        FGameplayAttributeData Mana;
    UFUNCTION()
        void OnRep_Mana(const FGameplayAttributeData& OldMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Mana, OldMana); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Mana)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMana() const
    {
        return Mana.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMana(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetManaAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMana(float NewVal)
    {
        Mana.SetBaseValue(NewVal);
        Mana.SetCurrentValue(NewVal);
    }

    //MaxMana
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxMana, Category = Attributes)
        FGameplayAttributeData MaxMana;
    UFUNCTION()
        void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxMana, OldMaxMana); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxMana)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxMana() const
    {
        return MaxMana.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxMana(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetMaxManaAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxMana(float NewVal)
    {
        MaxMana.SetBaseValue(NewVal);
        MaxMana.SetCurrentValue(NewVal);
    }

    //ManaRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ManaRegenRate, Category = Attributes)
        FGameplayAttributeData ManaRegenRate;
    UFUNCTION()
        void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ManaRegenRate, OldManaRegenRate); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ManaRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetManaRegenRate() const
    {
        return ManaRegenRate.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetManaRegenRate(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetManaRegenRateAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitManaRegenRate(float NewVal)
    {
        ManaRegenRate.SetBaseValue(NewVal);
        ManaRegenRate.SetCurrentValue(NewVal);
    }

    //Energy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Energy, Category = Attributes)
        FGameplayAttributeData Energy;
    UFUNCTION()
        void OnRep_Energy(const FGameplayAttributeData& OldEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Energy, OldEnergy); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Energy)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetEnergy() const
    {
        return Energy.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetEnergy(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetEnergyAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitEnergy(float NewVal)
    {
        Energy.SetBaseValue(NewVal);
        Energy.SetCurrentValue(NewVal);
    }

    //MaxEnergy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxEnergy, Category = Attributes)
        FGameplayAttributeData MaxEnergy;
    UFUNCTION()
        void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxEnergy, OldMaxEnergy); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxEnergy)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetMaxEnergy() const
    {
        return MaxEnergy.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetMaxEnergy(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetMaxEnergyAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitMaxEnergy(float NewVal)
    {
        MaxEnergy.SetBaseValue(NewVal);
        MaxEnergy.SetCurrentValue(NewVal);
    }

    //EnergyRegenRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_EnergyRegenRate, Category = Attributes)
        FGameplayAttributeData EnergyRegenRate;
    UFUNCTION()
        void OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, EnergyRegenRate, OldEnergyRegenRate); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, EnergyRegenRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetEnergyRegenRate() const
    {
        return EnergyRegenRate.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetEnergyRegenRate(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetEnergyRegenRateAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitEnergyRegenRate(float NewVal)
    {
        EnergyRegenRate.SetBaseValue(NewVal);
        EnergyRegenRate.SetCurrentValue(NewVal);
    }

    //Strength
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Strength, Category = Attributes)
        FGameplayAttributeData Strength;
    UFUNCTION()
        void OnRep_Strength(const FGameplayAttributeData& OldStrength) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Strength, OldStrength); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Strength)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetStrength() const
    {
        return Strength.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetStrength(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetStrengthAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitStrength(float NewVal)
    {
        Strength.SetBaseValue(NewVal);
        Strength.SetCurrentValue(NewVal);
    }

    //Agility
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Agility, Category = Attributes)
        FGameplayAttributeData Agility;
    UFUNCTION()
        void OnRep_Agility(const FGameplayAttributeData& OldAgility) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Agility, OldAgility); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Agility)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetAgility() const
    {
        return Agility.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetAgility(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetAgilityAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitAgility(float NewVal)
    {
        Agility.SetBaseValue(NewVal);
        Agility.SetCurrentValue(NewVal);
    }

    //Constitution
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Constitution, Category = Attributes)
        FGameplayAttributeData Constitution;
    UFUNCTION()
        void OnRep_Constitution(const FGameplayAttributeData& OldConstitution) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Constitution, OldConstitution); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Constitution)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetConstitution() const
    {
        return Constitution.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetConstitution(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetConstitutionAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitConstitution(float NewVal)
    {
        Constitution.SetBaseValue(NewVal);
        Constitution.SetCurrentValue(NewVal);
    }

    //CritRate
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritRate, Category = Attributes)
        FGameplayAttributeData CritRate;
    UFUNCTION()
        void OnRep_CritRate(const FGameplayAttributeData& OldCritRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CritRate, OldCritRate); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CritRate)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritRate() const
    {
        return CritRate.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritRate(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetCritRateAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritRate(float NewVal)
    {
        CritRate.SetBaseValue(NewVal);
        CritRate.SetCurrentValue(NewVal);
    }

    //CritDamage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritDamage, Category = Attributes)
        FGameplayAttributeData CritDamage;
    UFUNCTION()
        void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CritDamage, OldCritDamage); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CritDamage)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritDamage() const
    {
        return CritDamage.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritDamage(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetCritDamageAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritDamage(float NewVal)
    {
        CritDamage.SetBaseValue(NewVal);
        CritDamage.SetCurrentValue(NewVal);
    }

    //CritHitDamage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CritHitDamage, Category = Attributes)
        FGameplayAttributeData CritHitDamage;
    UFUNCTION()
        void OnRep_CritHitDamage(const FGameplayAttributeData& OldCritHitDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CritHitDamage, OldCritHitDamage); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CritHitDamage)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetCritHitDamage() const
    {
        return CritHitDamage.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetCritHitDamage(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetCritHitDamageAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitCritHitDamage(float NewVal)
    {
        CritHitDamage.SetBaseValue(NewVal);
        CritHitDamage.SetCurrentValue(NewVal);
    }

    //Attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Attack, Category = Attributes)
        FGameplayAttributeData Attack;
    UFUNCTION()
        void OnRep_Attack(const FGameplayAttributeData& OldAttack) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Attack, OldAttack); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Attack)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetAttack() const
    {
        return Attack.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetAttack(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetAttackAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitAttack(float NewVal)
    {
        Attack.SetBaseValue(NewVal);
        Attack.SetCurrentValue(NewVal);
    }

    //Defense
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Defense, Category = Attributes)
        FGameplayAttributeData Defense;
    UFUNCTION()
        void OnRep_Defense(const FGameplayAttributeData& OldDefense) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Defense, OldDefense); }
    ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Defense)
        UFUNCTION(BlueprintCallable, Category = Attributes)
        float HWGetDefense() const
    {
        return Defense.GetCurrentValue();
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWSetDefense(float NewVal)
    {
        UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
        if (ensure(AbilityComp))
        {
            AbilityComp->SetNumericAttributeBase(GetDefenseAttribute(), NewVal);
        };
    }
    UFUNCTION(BlueprintCallable, Category = Attributes)
        void HWInitDefense(float NewVal)
    {
        Defense.SetBaseValue(NewVal);
        Defense.SetCurrentValue(NewVal);
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
