// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWGASCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsModule.h"
#include "./AbilitySystem/HWGameplayAbility.h"
#include "../OWSHubWorldMMO.h"


FName AHWGASCharacter::AbilitySystemComponentName(TEXT("HWGAS"));

AHWGASCharacter::AHWGASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UHWAbilitySystemComponent>(AHWGASCharacter::AbilitySystemComponentName);
	AbilitySystem->SetIsReplicated(true);

	CombatAttributes = CreateDefaultSubobject<UHWCombatAttributeSet>(TEXT("CombatAttributeSet"));
}

// Called when the game starts or when spawned
void AHWGASCharacter::BeginPlay()
{
	Super::BeginPlay();

	NormalAbility1CooldownTag = FGameplayTag::RequestGameplayTag("Combat.Cooldown.NormalAbility1");
	BurningStateTag = FGameplayTag::RequestGameplayTag("Combat.State.Burning");

	//Code for displaying the actor name and local role
	//UE_LOG(OWSHubWorldMMO, Warning, TEXT("AHWGASCharacter - %s - %s"), *UKismetSystemLibrary::GetDisplayName(this), 
	//	*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()));

	//Only run on proxy clients
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		//Register combat state tag events for the mobs showing on the player or other players showing on the player
		AbilitySystem->RegisterGenericGameplayTagEvent().AddUObject(this, &AHWGASCharacter::CombatStateTagChanged);
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		//Register Cooldown tag event for the player client-side only.  This is for UI updates.
		AbilitySystem->RegisterGenericGameplayTagEvent().AddUObject(this, &AHWGASCharacter::OnUIRelatedTagsChanged);
	}
}

// Called every frame
void AHWGASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHWGASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASCharacter - SetupPlayerInputComponent Started"));
}


//Only called on the server side
void AHWGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASCharacter - PossessedBy Started"));

	//Initialize our abilities
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);

		OnAbilitySystemInitialized.Broadcast();
	}
}

//This won't be called on mobs, because they don't replicate their controller
void AHWGASCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASCharacter - OnRep_Controller Started"));

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystem)
	{
		AbilitySystem->RefreshAbilityActorInfo();
	}

	if (GetController())
	{
		if (Cast<APlayerController>(GetController())->GetHUD())
		{
			AHWHUD* PtrToHWHUD = Cast<AHWHUD>(Cast<APlayerController>(GetController())->GetHUD());
			NormalAbility1CooldownBeginDelegate.BindUObject(PtrToHWHUD, &AHWHUD::NormalAbility1CooldownBegin);
			NormalAbility1CooldownEndDelegate.BindUObject(PtrToHWHUD, &AHWHUD::NormalAbility1CooldownEnd);
		}
	}
}

void AHWGASCharacter::GrantAbility(TSubclassOf<class UGameplayAbility> NewAbility, int AbilityLevel)
{
	if (AbilitySystem)
	{
		if (HasAuthority())
		{
			if (NewAbility)
			{
				//UHWGameplayAbility* AbilityCDO = NewAbility->GetDefaultObject<UHWGameplayAbility>();
				FGameplayAbilitySpec AbilitySpec(NewAbility.GetDefaultObject(), AbilityLevel);
				//AbilitySpec.DynamicAbilityTags.AddTag(AbilityCDO->AbilityTags.GetByIndex(0));

				AbilitySystem->GiveAbility(AbilitySpec);
			}
		}
	}
}

void AHWGASCharacter::GrantAbilityKeyBind(TSubclassOf<class UGameplayAbility> NewAbility, int AbilityLevel, int InputID)
{
	if (AbilitySystem)
	{
		if (HasAuthority())
		{
			if (NewAbility)
			{
				AbilitySystem->GiveAbility(FGameplayAbilitySpec(NewAbility.GetDefaultObject(), AbilityLevel, InputID));
			}
		}
	}
}

void AHWGASCharacter::CalculateCombatAttributes()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASCharacter - CalculateCombatAttributes Started"));
	
	UHWCombatAttributeSet* HWCombatAttributeSet = (UHWCombatAttributeSet*)AbilitySystem->GetAttributeSet(UHWCombatAttributeSet::StaticClass());

	if (!HWCombatAttributeSet)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWGASCharacter - CalculateCombatAttributes - CombatAttributes is NULL!"));
		return;
	}

	HWCombatAttributeSet->HWSetAttack(BaseCharacterStats.Strength * 10.f);
	HWCombatAttributeSet->HWSetCritRate(BaseCharacterStats.Agility);
	HWCombatAttributeSet->HWSetCritDamage(BaseCharacterStats.Strength + BaseCharacterStats.Agility);
	HWCombatAttributeSet->HWSetMaxHealth(BaseCharacterStats.Constitution * 100);
}

/*
void AHWGASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	

}
*/

//UI Combat State
void AHWGASCharacter::AddCombatStateDisplayItem(UHWCombatStateDisplayItemObject* ItemToAdd)
{
	CombatStateDisplayItems.Add(ItemToAdd);
	ReloadCombatStateDisplayItems();
}

void AHWGASCharacter::RemoveCombatStateDisplayItem(UHWCombatStateDisplayItemObject* ItemToRemove)
{
	//Look up full ItemToRemove based on just the Gameplay Tag
	FGameplayTag GameplayTagToRemove = ItemToRemove->Data.CombatStateTag;
	
	//Loop through CombatStateDisplayItems and find the one to remove based on the GameplayTag
	int32 indexToRemove = -1;
	for (int indexToCheck=0; indexToCheck < CombatStateDisplayItems.Num(); indexToCheck++)
	{
		if (CombatStateDisplayItems[indexToCheck]->Data.CombatStateTag == GameplayTagToRemove)
		{
			indexToRemove = indexToCheck;
		}
	}

	//If we found a match entry, remove it
	if (indexToRemove > -1)
	{
		CombatStateDisplayItems.RemoveAt(indexToRemove);
		ReloadCombatStateDisplayItems();
	}
}

void AHWGASCharacter::ReloadCombatStateDisplayItems()
{
	//Only allow this to run on the client side
	if (HasAuthority())
	{
		return;
	}

	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWGASCharacter - ReloadCombatStateDisplayItems Started on the Client Side"));
	
	for (auto& CombatStateDisplayItem : CombatStateDisplayItems)
	{
		FCombatStateIconsDataTableRow* FoundDataTableEntry = FindCombatStateIconsDataTableRowFromGameplayTag(
			CombatStateDisplayItem->Data.CombatStateTag);

		if (FoundDataTableEntry)
		{		
			CombatStateDisplayItem->Data.ItemIcon = FoundDataTableEntry->Icon;
		}
	}

	ReloadUIForCombatStateDisplayItems();
}

FCombatStateIconsDataTableRow* AHWGASCharacter::FindCombatStateIconsDataTableRowFromGameplayTag(FGameplayTag GameplayTagToSearchFor)
{
	FCombatStateIconsDataTableRow* FoundItemType =
		CombatStateIcons->FindRow<FCombatStateIconsDataTableRow>(FName(GameplayTagToSearchFor.ToString()), "");
	if (FoundItemType)
	{
		return FoundItemType;
	}

	return nullptr;
}

void AHWGASCharacter::ReloadUIForCombatStateDisplayItems()
{

}

//Events

//Respond to UI related tag changes
void AHWGASCharacter::OnUIRelatedTagsChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag == NormalAbility1CooldownTag)
	{
		//It was removed
		if (NewCount == 0)
		{
			NormalAbility1CooldownEndDelegate.ExecuteIfBound();
		}
		else //It was added
		{
			NormalAbility1CooldownBeginDelegate.ExecuteIfBound();
		}
	}
	else if (Tag == BurningStateTag)
	{
		//It was removed
		if (NewCount == 0)
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("BurningStateTag - Removed"));
		}
		else //It was added
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("BurningStateTag - Added"));
		}
	}
}

//Fires on proxy clients to handle combat.state tags when they are added or removed
void AHWGASCharacter::CombatStateTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	//We only want to process Combat.State tags
	if (!Tag.ToString().Contains("Combat.State."))
		return;

	//Lookup the tag in our Combat State Icons Data Table
	FCombatStateIconsDataTableRow* FoundDataTableEntry = FindCombatStateIconsDataTableRowFromGameplayTag(
		Tag);

	//If the tag isn't in the table don't continue
	if (!FoundDataTableEntry)
		return;

	if (NewCount == 0)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("Removed"));

		UHWCombatStateDisplayItemObject* CombatStateDisplayItemObject = NewObject<UHWCombatStateDisplayItemObject>();
		CombatStateDisplayItemObject->Data.CombatStateTag = Tag;
		RemoveCombatStateDisplayItem(CombatStateDisplayItemObject);
	}
	else //It was added
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("Added"));

		UHWCombatStateDisplayItemObject* CombatStateDisplayItemObject = NewObject<UHWCombatStateDisplayItemObject>();
		CombatStateDisplayItemObject->Data.CombatStateTag = Tag;
		AddCombatStateDisplayItem(CombatStateDisplayItemObject);
	}
}

//Fires on proxy clients when the Wet tag is added or removed
void AHWGASCharacter::WetTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	//It was removed
	if (NewCount == 0)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("WetStateTag - Removed"));
	}
	else //It was added
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("WetStateTag - Added"));
	}
}

//Fires on proxy clients when the Cold tag is added or removed
void AHWGASCharacter::ColdTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	//It was removed
	if (NewCount == 0)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("ColdStateTag - Removed"));
	}
	else //It was added
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("ColdStateTag - Added"));
	}
}

//Fires on proxy clients when the Burning tag is added or removed
void AHWGASCharacter::BurningTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	//It was removed
	if (NewCount == 0)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("BurningStateTag - Removed"));
	}
	else //It was added
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("BurningStateTag - Added"));
	}
}