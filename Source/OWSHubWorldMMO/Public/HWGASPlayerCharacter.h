// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWGASCharacter.h"
#include "HWAbilitySystemComponent.h"
#include "HWInputConfig.h"
#include "HWGameplayAbilitySet.h"
#include "HWMappableConfigPair.h"
#include "HWGASPlayerCharacter.generated.h"



/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWGASPlayerCharacter : public AHWGASCharacter
{
	GENERATED_BODY()

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	AHWGASPlayerCharacter();

	//Input Config
	UPROPERTY(EditAnywhere, Category = "HW|Input")
		TArray<FMappableConfigPair> DefaultInputConfigs;

	UPROPERTY(EditAnywhere, Category = "HW|Input")
		float LookGamepadStickYawRate = 300.0f;
	UPROPERTY(EditAnywhere, Category = "HW|Input")
		float LookGamepadStickPitchRate = 165.0f;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Abilities")
		TArray<TObjectPtr<UHWGameplayAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Abilities")
		UHWAbilityTagRelationshipMapping* AbilityTagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Input")
		TObjectPtr<UHWInputConfig> InputConfig;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);

	template <class T>
	T* GetPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawn must be derived from APawn");
		return Cast<T>(GetOwner());
	}

	template <class T>
	T* GetPawnChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawnChecked must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

protected:
	UHWAbilitySystemComponent* GetHWAbilitySystemComponent();

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void AbilitySystemInitialized();

	virtual void PossessedBy(AController* NewController) override;
};
