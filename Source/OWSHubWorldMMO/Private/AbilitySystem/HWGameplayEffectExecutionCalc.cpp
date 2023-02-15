// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWGameplayEffectExecutionCalc.h"
#include "./AbilitySystem/CombatAttributeSet.h"

struct HWDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritHitDamage);

	HWDamageStatics()
	{
		//Snapshot Damage
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, Damage, Source, true);
		//Snapshot Strength
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, Strength, Source, true);
		//Snapshot Attack
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, Attack, Source, true);
		//Snapshot CritRate
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, CritRate, Source, true);
		//Snapshot CritDamage
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, CritDamage, Source, true);
		//Snapshot CritHitDamage
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCombatAttributeSet, CritHitDamage, Source, true);
	}
};

static const HWDamageStatics& DamageStatics()
{
	static HWDamageStatics DStatics;
	return DStatics;
}

UHWGameplayEffectExecutionCalc::UHWGameplayEffectExecutionCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);	
	RelevantAttributesToCapture.Add(DamageStatics().CritRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritHitDamageDef);
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

	float Damage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	float Strength = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluationParameters, Strength);
	float Attack = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluationParameters, Attack);
	float CritRate = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluationParameters, CritRate);
	float CritDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluationParameters, CritDamage);
	float CritHitDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritHitDamageDef, EvaluationParameters, CritHitDamage);

	//Clamp our values
	Damage = FMath::Clamp(Damage, 0.01f, 100.f); //1% to 10,000%
	Strength = FMath::Clamp(Strength, 5.0f, 15.f); //5 to 15
	Attack = FMath::Clamp(Attack, 1.f, 10000.f); //1 to 10,000
	CritRate = FMath::Clamp(CritRate, 0.f, 1.f); //0 to 100%
	CritDamage = FMath::Clamp(CritDamage, 0.f, 10.f); //0 to 1000%
	CritHitDamage = 0.f;

	//Roll to see if this is a Crit
	bool bWasACrit = false;
	if (FMath::FRand() <= CritRate)
	{
		bWasACrit = true;
	}

	float BaseDamage = Damage * Attack * (1.f + ((Strength - 10.f) / 10.f));
	float AttackerSideCalculatedDamage = BaseDamage;
	float AttackerSideCalculatedCritHitDamage = (bWasACrit ? BaseDamage * CritDamage : 0.f);
	if (AttackerSideCalculatedDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, AttackerSideCalculatedDamage));
	}
	//The critical hit damage is separated from the normal damage and applied on the CritHitDamage modifier.  This will allow our PreGameplayEffectExecute to handle it differently.
	if (AttackerSideCalculatedCritHitDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().CritHitDamageProperty, EGameplayModOp::Override, AttackerSideCalculatedCritHitDamage));
	}
}