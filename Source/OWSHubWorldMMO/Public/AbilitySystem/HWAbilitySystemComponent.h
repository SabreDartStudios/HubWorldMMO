// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "HWAbilityTagRelationshipMapping.h"
#include "HWAbilitySystemComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

USTRUCT(BlueprintType)
struct FGameplayAbilitySpecHandleWithBool
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilitySpecHandleWithBool()
	{
		GameplayAbilitySpecHandle = FGameplayAbilitySpecHandle();
		bEndAbilityThisFrame = false;
	}

	bool operator==(const FGameplayAbilitySpecHandleWithBool& Other) const
	{
		return GameplayAbilitySpecHandle == Other.GameplayAbilitySpecHandle;
	}

	bool operator!=(const FGameplayAbilitySpecHandleWithBool& Other) const
	{
		return GameplayAbilitySpecHandle != Other.GameplayAbilitySpecHandle;
	}

	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;
	bool bEndAbilityThisFrame;
};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	UHWAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool TryActivateAbilityBatchedAndEndInSameFrame(FGameplayAbilitySpecHandle AbilityHandle);

	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UHWAbilityTagRelationshipMapping* NewMapping);

protected:

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	virtual bool ShouldDoServerAbilityRPCBatch() const override { return true; }

protected:

	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
		UHWAbilityTagRelationshipMapping* TagRelationshipMapping;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	//int32 ActivationGroupCounts[(uint8)ELyraAbilityActivationGroup::MAX];

};
