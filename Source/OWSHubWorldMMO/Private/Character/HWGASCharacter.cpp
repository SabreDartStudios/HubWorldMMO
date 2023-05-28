// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWGASCharacter.h"
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

	//AbilitySystem->RegisterGenericGameplayTagEvent().AddUObject(this, &AOWSCharacterWithAbilities::OnGameplayEffectTagCountChanged);

	//Register Cooldown tag event for the client-side only.  This is for UI updates.
	//AbilitySystem->RegisterGameplayTagEvent(NormalAbility1CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHWGASCharacter::OnNormalAbility1CooldownTagChanged);
	AbilitySystem->RegisterGenericGameplayTagEvent().AddUObject(this, &AHWGASCharacter::OnUIRelatedTagsChanged);
}

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

	if (!CombatAttributes)
	{
		UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWGASCharacter - CalculateCombatAttributes - CombatAttributes is NULL!"));
		return;
	}

	CombatAttributes->HWSetAttack(BaseCharacterStats.Strength * 10.f);
	CombatAttributes->HWSetCritRate(BaseCharacterStats.Agility);
	CombatAttributes->HWSetCritDamage(BaseCharacterStats.Strength + BaseCharacterStats.Agility);
	CombatAttributes->HWSetMaxHealth(BaseCharacterStats.Constitution * 100);
}

/*
void AHWGASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	

}
*/

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