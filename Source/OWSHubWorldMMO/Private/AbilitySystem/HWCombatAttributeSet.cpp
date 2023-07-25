// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayTagsModule.h"
#include "GameplayEffectExtension.h"
#include "./Character/HWGASCharacter.h"

UHWCombatAttributeSet::UHWCombatAttributeSet()
{
	InitMaxHealth(1000.f); //Overriden by calculation of Constitution * 100
	InitMaxEnergy(100.f);
	InitMaxStamina(100.f);
	InitHealth(100.f);
	InitEnergy(100.f);
	InitStamina(100.f);

	InitHealthRegenRate(100.0f); //This value is regenerated every 5 seconds
	InitEnergyRegenRate(0.f);	

	InitStrength(10.f);
	InitAgility(10.f);
	InitConstitution(10.f);

	InitAttack(0.f);

	InitCritRate(1.0f);
	InitCritDamage(0.5f);

	SetupGameplayTags();
}

void UHWCombatAttributeSet::SetupGameplayTags()
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
bool UHWCombatAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	float CritHitDamageMagnitude = 0.f;
	if (Data.EvaluatedData.Attribute == GetCritHitDamageAttribute())
	{
		HandlePreExecuteEffectDamage(true, Data);
	}

	// Is Damage about to be applied?
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		HandlePreExecuteEffectDamage(false, Data);
	}

	return true;
}

//This function is only called on the server
void UHWCombatAttributeSet::HandlePreExecuteEffectDamage(bool IsCritDamage, struct FGameplayEffectModCallbackData& Data)
{
	static FProperty* DamageProperty = FindFieldChecked<FProperty>(UHWCombatAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UHWCombatAttributeSet, Damage));

	if (Data.EvaluatedData.Magnitude > 0.f)
	{
		FGameplayTagContainer EffectTags;
		Data.EffectSpec.GetAllAssetTags(EffectTags);

		//Get source attributes
		UAbilitySystemComponent* SourceAbilitySystem = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
		const UHWCombatAttributeSet* SourceCombatAttributes = SourceAbilitySystem->GetSet<UHWCombatAttributeSet>();

		//Check who damaged us
		WhoAttackedUsLast = Cast<AHWGASCharacter>(SourceAbilitySystem->GetOwner());

		//Get target attributes
		const AHWGASCharacter* MyCharacter = Cast<AHWGASCharacter>(GetOwningActor());
		UAbilitySystemComponent* TargetAbilitySystem = MyCharacter->GetAbilitySystemComponent();
		const UHWCombatAttributeSet* TargetCombatAttributes = TargetAbilitySystem->GetSet<UHWCombatAttributeSet>();

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


void UHWCombatAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	// What property was modified?
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Treat damage as minus health
		SetHealth(FMath::Clamp(GetHealth() - Damage.GetCurrentValue(), 0.f, GetMaxHealth()));
		Damage = 0.f;
	}

	if (Data.EvaluatedData.Attribute ==GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + Healing.GetCurrentValue(), 0.f, GetMaxHealth()));
		Healing = 0.f;
	}

	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0.f, GetMaxEnergy()));
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
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

void UHWCombatAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, HealthRegenRate, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Energy, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, MaxEnergy, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, EnergyRegenRate, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Stamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, MaxStamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Strength, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Agility, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Constitution, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, CritRate, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, CritDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, CritHitDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Attack, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Defense, COND_OwnerOnly, REPNOTIFY_Always);

}