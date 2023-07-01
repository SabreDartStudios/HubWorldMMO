// Copyright 2023 Sabre Dart Studios


#include "./Portals/HWPortal.h"
#include "../OWSHubWorldMMO.h"
#include "./Character/HWGASPlayerCharacter.h"
#include "./Player/HWPlayerController.h"


AHWPortal::AHWPortal()
{
	
}

void AHWPortal::TravelThroughPortal(AActor* ActorFromOverlapEvent)
{
	//Only process portal travel on the server side
	if (HasAuthority())
	{
		if (ZoneName.IsEmpty())
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWPortal - TravelThroughPortal - ZoneName is Empty!"));
			return;
		}

		AHWGASPlayerCharacter* Character = Cast<AHWGASPlayerCharacter>(ActorFromOverlapEvent);
		if (!Character)
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWPortal - TravelThroughPortal - Unable to cast ActorFromOverlapEvent to AHWPlayerCharacter!"));
			return;
		}

		//Latch - If this Player Character is already transferring between zones, then don't run this code again
		if (Character->GetIsTransferringBetweenZones())
		{
			UE_LOG(OWSHubWorldMMO, Warning, TEXT("AHWPortal - TravelThroughPortal - Player Character is already transferring between zones"));
			return;
		}

		AHWPlayerController* PlayerController = Character->GetController<AHWPlayerController>();
		if (!PlayerController)
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWPortal - TravelThroughPortal - Unable to cast Character->GetController() to AHWPlayerController!"));
			return;
		}

		AOWSPlayerState* OWSPlayerState = PlayerController->GetPlayerState<AOWSPlayerState>();
		if (!OWSPlayerState)
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWPortal - TravelThroughPortal - Unable to cast PlayerController->GetPlayerState to AOWSPlayerState!"));
			return;
		}

		FString CharacterName = OWSPlayerState->GetPlayerName();
		if (CharacterName.IsEmpty())
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("AHWPortal - TravelThroughPortal - CharacterName from AOWSPlayerState is empty!"));
			return;
		}

		//Set IsTransferringBetweenZones to true
		Character->SetIsTransferringBetweenZones(true);
		//Stop movement
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->DisableMovement();

		//Show Loading Screen
		PlayerController->ShowLoadingScreen();

		PlayerController->GetZoneServerToTravelTo(PlayerController, ERPGSchemeToChooseMap::MapWithFewestPlayers, 0, ZoneName,
			StartingLocation, StartingRotation);
	}
}

