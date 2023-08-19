// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h" 
#include "./AbilitySystem/HWCombatAttributeSet.h" 
#include "./Character/HWCharacter.h"
//#include "./Player/HWPlayerController.h"
#include "./UI/HWHUD.h"
#include "./AbilitySystem/HWAbilitySystemComponent.h"
#include "HWGASCharacter.generated.h"

//Row definition for the Combat State Icons
USTRUCT()
struct FCombatStateIconsDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FCombatStateIconsDataTableRow()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat State")
		FGameplayTag CombatStateTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat State")
		TSoftObjectPtr<UTexture2D> Icon;
};

//Row definition for the combined runtime and setup data for displaying the combat state icons in the UI
USTRUCT(BlueprintType, Blueprintable)
struct FHWCombatStateIconDisplayItem
{
	GENERATED_USTRUCT_BODY()

		FHWCombatStateIconDisplayItem()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat State")
		FGameplayTag CombatStateTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat State")
		TSoftObjectPtr<UTexture2D> ItemIcon;

};

//UObject to contain the combat state icon row struct (this is required by the UI system)
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OWSHUBWORLDMMO_API UHWCombatStateDisplayItemObject : public UObject
{
	GENERATED_BODY()

public:
	FHWCombatStateIconDisplayItem Data;

};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWGASCharacter : public AHWCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AHWGASCharacter();

	static FName AbilitySystemComponentName;

	//Calculate Combat Attributes
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void CalculateCombatAttributes();

	//Reaction Gameplay Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Effects")
		TSubclassOf<class UGameplayEffect> BurningEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Effects")
		TSubclassOf<class UGameplayEffect> WetEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Effects")
		TSubclassOf<class UGameplayEffect> ColdEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Effects")
		TSubclassOf<class UGameplayEffect> ChargedEffect;

	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return (UAbilitySystemComponent*)AbilitySystem;
	};

	FORCEINLINE UHWAbilitySystemComponent* GetHWAbilitySystemComponent() const
	{
		return AbilitySystem;
	};

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GrantAbility(TSubclassOf<class UGameplayAbility> NewAbility, int AbilityLevel);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GrantAbilityKeyBind(TSubclassOf<class UGameplayAbility> NewAbility, int AbilityLevel, int InputID);

	//Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void OnDeath(AHWGASCharacter* WhoKilledMe);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void OnTakeDamage(AHWGASCharacter* WhoAttackedMe, float DamageAmount, bool IsCritical);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
		void OnInflictDamage(AHWGASCharacter* WhoWasDamaged, float DamageAmount, bool IsCritical);
	
	void OnUIRelatedTagsChanged(const FGameplayTag Tag, int32 NewCount);

	//UI Combat State
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat State")
		UDataTable* CombatStateIcons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat State")
		TArray<UHWCombatStateDisplayItemObject*> CombatStateDisplayItems;

	UFUNCTION(BlueprintCallable, Category = "Combat State")
		void AddCombatStateDisplayItem(UHWCombatStateDisplayItemObject* ItemToRemove);

	UFUNCTION(BlueprintCallable, Category = "Combat State")
		void RemoveCombatStateDisplayItem(UHWCombatStateDisplayItemObject* ItemToAdd);

	UFUNCTION(BlueprintCallable, Category = "Combat State")
		void ReloadCombatStateDisplayItems();

	FCombatStateIconsDataTableRow* FindCombatStateIconsDataTableRowFromGameplayTag(FGameplayTag GameplayTagToSearchFor);

	//Gameplay Tags
	FGameplayTag NormalAbility1CooldownTag;
	FGameplayTag BurningStateTag;

	FNormalAbility1CooldownBeginDelegate NormalAbility1CooldownBeginDelegate;
	FNormalAbility1CooldownEndDelegate NormalAbility1CooldownEndDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ReloadUIForCombatStateDisplayItems();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_Controller() override;

	virtual void WetTagChanged(const FGameplayTag Tag, int32 NewCount);
	virtual void ColdTagChanged(const FGameplayTag Tag, int32 NewCount);
	virtual void BurningTagChanged(const FGameplayTag Tag, int32 NewCount);
	virtual void CombatStateTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:

	FSimpleMulticastDelegate OnAbilitySystemInitialized;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UHWAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		UHWCombatAttributeSet* CombatAttributes;
};
