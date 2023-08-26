// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayTagsModule.h"
#include "GameplayEffectExtension.h"
#include "Kismet/GameplayStatics.h"
#include "./Character/HWGASCharacter.h"
#include "./Game/HWGameMode.h"

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

	SetupGameplayEffects();
	SetupGameplayTags();
}

void UHWCombatAttributeSet::SetupGameplayEffects()
{
	UWorld* const World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		AHWGameMode* HWGameMode = Cast<AHWGameMode>(World->GetAuthGameMode());

		if (HWGameMode)
		{
			ApplyColdGameplayEffect = HWGameMode->ApplyColdGameplayEffect;
			ApplyBurningGameplayEffect = HWGameMode->ApplyBurningGameplayEffect;
			ApplyWetGameplayEffect = HWGameMode->ApplyWetGameplayEffect;
			ApplyElectrifiedGameplayEffect = HWGameMode->ApplyElectrifiedGameplayEffect;

			ApplyFrozenGameplayEffect = HWGameMode->ApplyFrozenGameplayEffect;
			ApplyChargedGameplayEffect = HWGameMode->ApplyChargedGameplayEffect;
		}
	}
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
	ElectrifiedTag = FGameplayTag::RequestGameplayTag("Combat.State.Electrified");

	ChargedTag = FGameplayTag::RequestGameplayTag("Combat.State.Charged");
	FrozenTag = FGameplayTag::RequestGameplayTag("Combat.State.Frozen");

	CriticalHitTag = FGameplayTag::RequestGameplayTag("Combat.Flags.CriticalHit");
}

//This function is only called on the server
bool UHWCombatAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	// Is Damage about to be applied?
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		//If the damage is an Odd number, then this is a crit, otherwise it isn't
		if ((int32)FMath::Floor(Data.EvaluatedData.Magnitude) % 2 == 1)
		{
			HandlePreExecuteEffectDamage(true, Data);
		}
		else
		{
			HandlePreExecuteEffectDamage(false, Data);
		}
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
		FGameplayTagContainer GrantedTags;
		Data.EffectSpec.GetAllGrantedTags(GrantedTags);

		//Get source attributes
		UAbilitySystemComponent* SourceAbilitySystem = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
		const UGameplayAbility* AbilityUsed = Data.EffectSpec.GetContext().GetAbility();
		const UHWCombatAttributeSet* SourceCombatAttributes = SourceAbilitySystem->GetSet<UHWCombatAttributeSet>();

		//Check who damaged us
		WhoAttackedUsLast = Cast<AHWGASCharacter>(SourceAbilitySystem->GetOwner());

		//Get target attributes
		const AHWGASCharacter* MyCharacter = Cast<AHWGASCharacter>(GetOwningActor());
		UAbilitySystemComponent* TargetAbilitySystem = MyCharacter->GetAbilitySystemComponent();
		const UHWCombatAttributeSet* TargetCombatAttributes = TargetAbilitySystem->GetSet<UHWCombatAttributeSet>();

		//Check for reactions
		float VaporizeDamageMultiplier = 1.f;
		float ElectrocuteDamageMultiplier = 1.f;
		float MeltDamageMultiplier = 1.f;

		bool bShouldApplyWetEffect = false;
		bool bShouldApplyColdEffect = false;
		bool bShouldApplyBurningEffect = false;
		bool bShouldApplyElectrifiedEffect = false;
		
		if (EffectTags.HasTag(WaterDamageTag))
		{
			bShouldApplyWetEffect = true;
		}
		else if (EffectTags.HasTag(IceDamageTag))
		{
			bShouldApplyColdEffect = true;
		}
		else if (EffectTags.HasTag(FireDamageTag))
		{
			bShouldApplyBurningEffect = true;
		}
		else if (EffectTags.HasTag(LightningDamageTag))
		{
			bShouldApplyElectrifiedEffect = true;
		}

		//Reactions where the target is Burning
		if (TargetAbilitySystem->HasMatchingGameplayTag(BurningTag))
		{
			//Vaporize
			if (EffectTags.HasTag(WaterDamageTag))
			{
				VaporizeDamageMultiplier = 1.5f;
				//Remove Wet Tag from what is going to be applied
				bShouldApplyWetEffect = false;
				//Remove any gameplay effects that have the Burning Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(BurningTag));
			}
			//Melt - 1.3 times damage. 
			if (EffectTags.HasTag(IceDamageTag))
			{
				MeltDamageMultiplier = 1.3f;
				//Remove Cold Tag from what is going to be applied
				bShouldApplyColdEffect = false;
				//Remove any gameplay effects that have the Burning Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(BurningTag));

				//TargetAbilitySystem->AddLooseGameplayTag(WetTag);
				//TargetAbilitySystem->AddMinimalReplicationGameplayTag(WetTag);
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
				VaporizeDamageMultiplier = 1.5f;
				//Remove Burning Tag from what is going to be applied
				bShouldApplyBurningEffect = false;
				//Remove any gameplay effects that have the Wet Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(WetTag));
			}
			//Frozen - Freeze in place
			if (EffectTags.HasTag(IceDamageTag))
			{
				//Remove Cold Tag from what is going to be applied
				bShouldApplyColdEffect = false;
				//Remove any gameplay effects that have the Wet Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(WetTag));
				//Apply Frozen
				TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyFrozenGameplayEffect->GetDefaultObject<UGameplayEffect>(),
					1.0f, TargetAbilitySystem->MakeEffectContext());
			}
			//Electrocute - 1.2 times damage and random chance to apply Charged status.
			if (EffectTags.HasTag(LightningDamageTag))
			{
				ElectrocuteDamageMultiplier = 1.2f;
				//Remove Electrified Tag from what is going to be applied
				bShouldApplyElectrifiedEffect = false;
				//Remove any gameplay effects that have the Wet Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(WetTag));
				//Chance to apply Charged
				TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyChargedGameplayEffect->GetDefaultObject<UGameplayEffect>(),
					1.0f, TargetAbilitySystem->MakeEffectContext());
			}
		}

		//Reactions where the target is Cold
		else if (TargetAbilitySystem->HasMatchingGameplayTag(ColdTag))
		{
			//Melt - 1.3 times damage. 
			if (EffectTags.HasTag(FireDamageTag))
			{
				MeltDamageMultiplier = 1.3f;
				//Remove Burning Tag from what is going to be applied
				bShouldApplyBurningEffect = false;
				//Remove any gameplay effects that have the Cold Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ColdTag));
			}
			//Frozen - Freeze in place
			if (EffectTags.HasTag(WaterDamageTag))
			{
				//Remove Wet Tag from what is going to be applied
				bShouldApplyWetEffect = false;
				//Remove any gameplay effects that have the Cold Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ColdTag));
				//Apply Frozen
				TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyFrozenGameplayEffect->GetDefaultObject<UGameplayEffect>(),
					1.0f, TargetAbilitySystem->MakeEffectContext());
			}
			//Superconduct
			if (EffectTags.HasTag(LightningDamageTag))
			{

			}
		}

		//Reactions where the target is Frozen
		else if (TargetAbilitySystem->HasMatchingGameplayTag(FrozenTag))
		{
			//Melt - 1.3 times damage.  Applies Wet status.
			if (EffectTags.HasTag(FireDamageTag))
			{
				MeltDamageMultiplier = 1.3f;
				//Remove Burning Tag from what is going to be applied
				bShouldApplyBurningEffect = false;
				//Remove any gameplay effects that have the Frozen Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FrozenTag));
				//Apply Wet Tag
				bShouldApplyWetEffect = true;
			}
		}

		//Reactions where the target is ElectrifiedTag
		else if (TargetAbilitySystem->HasMatchingGameplayTag(ElectrifiedTag))
		{
			//Firestorm
			if (EffectTags.HasTag(FireDamageTag))
			{

			}
			//Electrocute
			if (EffectTags.HasTag(WaterDamageTag))
			{
				ElectrocuteDamageMultiplier = 1.2f;
				//Remove Wet Tag from what is going to be applied
				bShouldApplyWetEffect = false;
				//Remove any gameplay effects that have the Wet Tag already granted
				TargetAbilitySystem->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(WetTag));
				//Chance to apply Charged
				TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyChargedGameplayEffect->GetDefaultObject<UGameplayEffect>(),
					1.0f, TargetAbilitySystem->MakeEffectContext());
			}
			//Superconduct
			if (EffectTags.HasTag(IceDamageTag))
			{

			}
		}
		else //No reaction
		{

		}

		if (bShouldApplyWetEffect)
		{
			TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyWetGameplayEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f, TargetAbilitySystem->MakeEffectContext());
		}
		else if (bShouldApplyBurningEffect)
		{
			TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyBurningGameplayEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f, TargetAbilitySystem->MakeEffectContext());
		}
		else if (bShouldApplyColdEffect)
		{
			TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyColdGameplayEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f, TargetAbilitySystem->MakeEffectContext());
		}
		else if (bShouldApplyElectrifiedEffect)
		{
			TargetAbilitySystem->ApplyGameplayEffectToSelf(ApplyElectrifiedGameplayEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f, TargetAbilitySystem->MakeEffectContext());
		}

		//Apply damage multipliers
		float NewDamageMagnitude = Data.EvaluatedData.Magnitude * VaporizeDamageMultiplier * ElectrocuteDamageMultiplier * MeltDamageMultiplier;

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
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Attack, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWCombatAttributeSet, Defense, COND_OwnerOnly, REPNOTIFY_Always);

}