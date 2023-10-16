// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/HWGameplayAbility.h"
#include "HWGameplayAbility_FakeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayAbility_FakeProjectile : public UHWGameplayAbility
{
	GENERATED_BODY()
	
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData);

	/** How long after ability activation to delay the Spawn Projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Projectile")
		float DelaySpawnProjectile = 0.f;

	UFUNCTION()
		void OnDelaySpawnProjectile();
};
