// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWGASMobCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsModule.h"
#include "../OWSHubWorldMMO.h"


AHWGASMobCharacter::AHWGASMobCharacter()
{
	//Since this is for AI we will override this to Minimal mode
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}