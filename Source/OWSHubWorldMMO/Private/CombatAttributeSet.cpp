// Copyright 2022 Sabre Dart Studios


#include "CombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayTagsModule.h"
#include "GameplayEffectExtension.h"
#include "HWGASCharacter.h"

UCombatAttributeSet::UCombatAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxHealth = 100.f;
	MaxEnergy = 100.f;
	MaxMana = 100.f;
	Health = 100.f;
	Energy = 100.f;
	Mana = 100.f;

	EnergyRegenRate = 1.f;
	ManaRegenRate = 0.5f;

	Strength = 10;
	Agility = 10;
	Constitution = 10;

	Attack = 0;

	CritRate = 1.0f;
	CritDamage = 0.5f;

	SetupGameplayTags();
}

void UCombatAttributeSet::SetupGameplayTags()
{
	DeadTag = FGameplayTag::RequestGameplayTag("Combat.State.Dead");

	FireDamageTag = FGameplayTag::RequestGameplayTag("Combat.DamageType.Fire");
	WaterDamageTag = FGameplayTag::RequestGameplayTag("Combat.DamageType.Water");
	IceDamageTag = FGameplayTag::RequestGameplayTag("Combat.DamageType.Ice");
	LightningDamageTag = FGameplayTag::RequestGameplayTag("Combat.DamageType.Lightning");

	WetTag = FGameplayTag::RequestGameplayTag("Combat.State.Wet");
	BurningTag = FGameplayTag::RequestGameplayTag("Combat.State.Burning");
	ColdTag = FGameplayTag::RequestGameplayTag("Combat.State.Cold");
	ChargedTag = FGameplayTag::RequestGameplayTag("Combat.State.Charged");
	CriticalHitTag = FGameplayTag::RequestGameplayTag("Combat.Flags.CriticalHit");
}

//This function is only called on the server
bool UCombatAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	static FProperty* HealthProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Health));
	static FProperty* DamageProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Damage));
	static FProperty* CritHitDamageProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, CritHitDamage));
	static FProperty* HealingProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Healing));

	FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	float CritHitDamageMagnitude = 0.f;
	if (CritHitDamageProperty == ModifiedProperty)
	{
		HandlePreExecuteEffectDamage(true, Data);
	}

	// Is Damage about to be applied?
	if (DamageProperty == ModifiedProperty)
	{
		HandlePreExecuteEffectDamage(false, Data);
	}

	return true;
}

//This function is only called on the server
void UCombatAttributeSet::HandlePreExecuteEffectDamage(bool IsCritDamage, struct FGameplayEffectModCallbackData& Data)
{
	static FProperty* DamageProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Damage));

	if (Data.EvaluatedData.Magnitude > 0.f)
	{
		FGameplayTagContainer EffectTags;
		Data.EffectSpec.GetAllAssetTags(EffectTags);

		//Get source attributes
		UAbilitySystemComponent* SourceAbilitySystem = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
		const UCombatAttributeSet* SourceCombatAttributes = SourceAbilitySystem->GetSet<UCombatAttributeSet>();

		//Check who damaged us
		WhoAttackedUsLast = Cast<AHWGASCharacter>(SourceAbilitySystem->GetOwner());

		//Get target attributes
		const AHWGASCharacter* MyCharacter = Cast<AHWGASCharacter>(GetOwningActor());
		UAbilitySystemComponent* TargetAbilitySystem = MyCharacter->GetAbilitySystemComponent();
		const UCombatAttributeSet* TargetCombatAttributes = TargetAbilitySystem->GetSet<UCombatAttributeSet>();

		//Check for reactions

		//Reactions where the target is Burning
		if (TargetAbilitySystem->HasMatchingGameplayTag(BurningTag))
		{
			//Vaporize
			if (EffectTags.HasTag(WaterDamageTag))
			{

			}
			//Melt
			if (EffectTags.HasTag(IceDamageTag))
			{

			}
			//Firestorm
			if (EffectTags.HasTag(LightningDamageTag))
			{

			}
		}

		//Reactions where the target is Wet
		else if (TargetAbilitySystem->HasMatchingGameplayTag(WetTag))
		{
			//Vaporize
			if (EffectTags.HasTag(FireDamageTag))
			{

			}
			//Frozen
			if (EffectTags.HasTag(IceDamageTag))
			{

			}
			//Electrocute
			if (EffectTags.HasTag(LightningDamageTag))
			{

			}
		}

		//Reactions where the target is Cold
		else if (TargetAbilitySystem->HasMatchingGameplayTag(ColdTag))
		{
			//Melt
			if (EffectTags.HasTag(FireDamageTag))
			{

			}
			//Frozen
			if (EffectTags.HasTag(WaterDamageTag))
			{

			}
			//Superconduct
			if (EffectTags.HasTag(LightningDamageTag))
			{

			}
		}

		//Reactions where the target is Charged
		else if (TargetAbilitySystem->HasMatchingGameplayTag(ChargedTag))
		{
			//Firestorm
			if (EffectTags.HasTag(FireDamageTag))
			{

			}
			//Electrocute
			if (EffectTags.HasTag(WaterDamageTag))
			{

			}
			//Superconduct
			if (EffectTags.HasTag(IceDamageTag))
			{

			}
		}

		else //No reaction
		{
			if (EffectTags.HasTag(FireDamageTag) && MyCharacter->BurningEffect)
			{
				FGameplayEffectSpecHandle BurningEffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(MyCharacter->BurningEffect, 1, TargetAbilitySystem->MakeEffectContext());

				TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*BurningEffectSpecHandle.Data.Get());
			}
		}

		float NewDamageMagnitude = Data.EvaluatedData.Magnitude;

		//If Defense is greater than zero, then reduce the damage by Defense as a percent (i.e. 0.05 Defense is a 5% reduction in incoming damage)
		if (HWGetDefense() > 0.f)
		{
			NewDamageMagnitude -= NewDamageMagnitude * HWGetDefense();
		}

		if (NewDamageMagnitude > 0.f)
		{
			AHWGASCharacter* DamagedCharacter = Cast<AHWGASCharacter>(GetOwningActor());
			if (DamagedCharacter)
			{
				DamagedCharacter->OnTakeDamage(WhoAttackedUsLast, NewDamageMagnitude, IsCritDamage);

				if (WhoAttackedUsLast)
				{
					AHWGASCharacter* AttackingCharacter = Cast<AHWGASCharacter>(WhoAttackedUsLast);

					if (AttackingCharacter)
					{
						AttackingCharacter->OnInflictDamage(DamagedCharacter, NewDamageMagnitude, IsCritDamage);
					}
				}
			}
		}

		//Set the new damage magnitude
		Data.EvaluatedData.Magnitude = NewDamageMagnitude;

		//This is CritHitDamage, so we need to convert it to Damage for the PostGameplayEffectExecute to apply
		if (IsCritDamage)
		{
			Data.EvaluatedData.Attribute = DamageProperty;
		}
	}
}


void UCombatAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	static FProperty* DamageProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Damage));
	static FProperty* HealingProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Healing));
	static FProperty* EnergyProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Energy));
	static FProperty* ManaProperty = FindFieldChecked<FProperty>(UCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Mana));
	FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	// What property was modified?
	if (DamageProperty == ModifiedProperty)
	{
		// Treat damage as minus health
		SetHealth(FMath::Clamp(GetHealth() - Damage.GetCurrentValue(), 0.f, GetMaxHealth()));
		Damage = 0.f;
	}

	if (HealingProperty == ModifiedProperty)
	{
		SetHealth(FMath::Clamp(GetHealth() + Healing.GetCurrentValue(), 0.f, GetMaxHealth()));
		Healing = 0.f;
	}

	if (EnergyProperty == ModifiedProperty)
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0.f, GetMaxEnergy()));
	}

	if (ManaProperty == ModifiedProperty)
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	//Dead
	if (GetHealth() <= 0.f)
	{
		AHWGASCharacter* MyCharacter = Cast<AHWGASCharacter>(GetOwningActor());
		if (MyCharacter && !MyCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(FGameplayTagContainer(DeadTag)))
		{
			MyCharacter->OnDeath(WhoAttackedUsLast);
		}
	}
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, EnergyRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Constitution, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CritRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CritHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Defense, COND_None, REPNOTIFY_Always);

}