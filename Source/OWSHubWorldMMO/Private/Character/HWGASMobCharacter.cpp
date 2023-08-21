// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWGASMobCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsModule.h"
#include "./UI/HWMobNameplateWidget.h"
#include "../OWSHubWorldMMO.h"


AHWGASMobCharacter::AHWGASMobCharacter()
{
	MobNameplateWidgetComponent = CreateDefaultSubobject<UHWMobNameplateWidgetComponent>(TEXT("MobNameplate"));
	MobNameplateWidgetComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Since this is for AI we will override this to Minimal mode
	GetAbilitySystemComponent()->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AHWGASMobCharacter::ReloadUIForCombatStateDisplayItems()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASMobCharacter - ReloadUIForCombatStateDisplayItems Started"));

	UHWMobNameplateWidget* MobNameplateWidget = Cast<UHWMobNameplateWidget>(MobNameplateWidgetComponent->GetWidget());

	MobNameplateWidget->RefreshSource();
}