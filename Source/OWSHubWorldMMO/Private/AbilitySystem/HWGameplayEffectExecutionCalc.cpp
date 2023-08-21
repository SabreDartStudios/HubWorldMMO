// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWGameplayEffectExecutionCalc.h"
#include "./AbilitySystem/HWCombatAttributeSet.h"

struct HWDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition AttackDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
	FGameplayEffectAttributeCaptureDefinition CritRateDef;
	FGameplayEffectAttributeCaptureDefinition CritDamageDef;

	HWDamageStatics()
	{
		AttackDef = FGameplayEffectAttributeCaptureDefinition(UHWCombatAttributeSet::GetAttackAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UHWCombatAttributeSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(UHWCombatAttributeSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		CritRateDef = FGameplayEffectAttributeCaptureDefinition(UHWCombatAttributeSet::GetCritRateAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		CritDamageDef = FGameplayEffectAttributeCaptureDefinition(UHWCombatAttributeSet::GetCritDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static const HWDamageStatics& DamageStatics()
{
	static HWDamageStatics DStatics;
	return DStatics;
}

UHWGameplayEffectExecutionCalc::UHWGameplayEffectExecutionCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);	
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
}


void UHWGameplayEffectExecutionCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float Attack = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluationParameters, Attack);
	float Defense = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, Defense);
	float MaxHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MaxHealthDef, EvaluationParameters, MaxHealth);
	float CritRate = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluationParameters, CritRate);
	float CritDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluationParameters, CritDamage);

	//Clamp our values
	Attack = FMath::Clamp(Attack, 0.f, 10000.f); //0 to 10,000
	Defense = FMath::Clamp(Defense, 0.f, 10000.f); //0 to 10,000
	MaxHealth = FMath::Clamp(MaxHealth, 0.f, 10000.f); //0 to 10,000
	CritRate = FMath::Clamp(CritRate, 0.f, 1.f); //0 to 100%
	CritDamage = FMath::Clamp(CritDamage, 0.f, 10.f); //0 to 1000%

	//Roll to see if this is a Crit
	bool bWasACrit = false;
	if (FMath::FRand() <= (CritRate / 100.f))
	{
		bWasACrit = true;
	}

	float BaseDamage = Attack + Defense + MaxHealth;
	float AttackerSideCalculatedDamage = BaseDamage;
	float AttackerSideCalculatedCritHitDamage = (bWasACrit ? BaseDamage * (CritDamage / 100.f) : 0.f);

	//Always round down to previous integer.  This is done because our odd/even checks below add 1 if needed
	//This could potentially lead to noticeable problems in games with low damage outputs.  We will be using large damage values 
	//and so this should not be an issue for us.
	int32 AttackerSideTotalDamage = (int32)FMath::Floor(AttackerSideCalculatedDamage + AttackerSideCalculatedCritHitDamage);

	//If the AttackerSideTotalDamage is zero or less, don't do anything
	if (AttackerSideTotalDamage > 0)
	{
		//This was a critical hit
		if (bWasACrit)
		{
			//The AttackerSideTotalDamage is an even number, so we need to add 1 to make it odd
			if (AttackerSideTotalDamage % 2 == 0)
			{
				AttackerSideTotalDamage++; //Add 1 to make it an odd number.  Odd number damage values are critical hits
			}
		}
		else //This is NOT a critical hit
		{
			//The AttackerSideTotalDamage is an odd number, so we need to add 1 to make it even
			if (AttackerSideTotalDamage % 2 == 1)
			{
				AttackerSideTotalDamage++; //Add 1 to make it an even number.  Even number damage values are NOT critical hits
			}
		}

		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHWCombatAttributeSet::GetDamageAttribute(),
			EGameplayModOp::Override, AttackerSideTotalDamage));
	}

	/*
	//The critical hit damage is separated from the normal damage and applied on the CritHitDamage modifier.  This will allow our PreGameplayEffectExecute to handle it differently.
	if (AttackerSideCalculatedCritHitDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHWCombatAttributeSet::GetCritHitDamageAttribute(), EGameplayModOp::Override, AttackerSideCalculatedCritHitDamage));
	}
	*/
}