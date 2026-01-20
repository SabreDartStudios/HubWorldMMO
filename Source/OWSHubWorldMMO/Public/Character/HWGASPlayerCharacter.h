// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "HWGASCharacter.h"
#include "./AbilitySystem/HWAbilitySystemComponent.h"
#include "./Input/HWInputConfig.h"
#include "./AbilitySystem/HWGameplayAbilitySet.h"
#include "./Input/HWMappableConfigPair.h"
#include "HWGASPlayerCharacter.generated.h"

USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSettings = true;
};

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

	//Zones
	UFUNCTION(BlueprintCallable, Category = "Travel")
		void SetIsTransferringBetweenZones(bool bSetIsTransferringBetweenZones);
	UFUNCTION(BlueprintCallable, Category = "Travel")
		bool GetIsTransferringBetweenZones();

	//Input Config
	UPROPERTY(EditAnywhere, Category = "HW|Input")
		TArray<FMappableConfigPair> DefaultInputConfigs;

	UPROPERTY(EditAnywhere, Category = "HW|Input")
		TArray<FInputMappingContextAndPriority> DefaultInputMappings;

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

	// Input mapping context used by player controlled pawns to create input mappings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Input")
		UInputMappingContext* InputMapping;

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

private:
	bool bIsTransferringBetweenZones;
};
