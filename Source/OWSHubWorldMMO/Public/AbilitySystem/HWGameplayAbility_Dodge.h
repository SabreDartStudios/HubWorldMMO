// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "HWGameplayAbility.h"
#include "HWGameplayAbility_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayAbility_Dodge : public UHWGameplayAbility
{
	GENERATED_BODY()
	
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData);

	/** Direction to doge relative to the direction the player character is facing.  
	0 = Forwards
	1 = Right 
	2 = Backwards
	3 = Left*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		uint8 Direction;  //0 = Forwards, 1 = Right, 2 = Backwards, 3 = Left

	/** Dodge movement strength*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float Strength = 2000.f;

	/** How long to apply the dodge movement strength in seconds.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float Duration = 0.2f;

	/** The Animatiom Montage to play while dodging.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* DodgingAnimation;

	/** The play rate to play the DodgingAnimation.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float DodgingAnimationPlayRate = 1.f;

	/** How long after ability activation to delay the DodgedRecentlyGameplayEffect.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
		float DelayDodgedRecentlyApplication = 0.f;

	/** Use the DodgedRecentlyGameplayEffect to apply a Gameplay Tag for ability combo purposes.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
		TSubclassOf<UGameplayEffect> DodgedRecentlyGameplayEffect;

	/** How long after ability activation to delay the IFrameGameplayEffect.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
		float DelayIFrameApplication = 0.f;

	/** Use the IFrameGameplayEffect to apply a Gameplay Tag for purposes of invulnerability while dodging.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
		TSubclassOf<UGameplayEffect> IFrameGameplayEffect;

	UFUNCTION()
		void OnFinished();

	UFUNCTION()
		void OnDelayDodgedRecently();

	UFUNCTION()
		void OnDelayIFrame();

protected:
	float SavedGroundFriction;

};
