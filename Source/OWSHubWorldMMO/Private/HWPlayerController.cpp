// Fill out your copyright notice in the Description page of Project Settings.


#include "HWPlayerController.h"
#include "HWGASCharacter.h"
#include "OWSGameInstance.h"
#include "../OWSHubWorldMMO.h"

AHWPlayerController::AHWPlayerController()
{
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("OWSAPICustomerKey"),
		OWSAPICustomerKey,
		GGameIni
	);

	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("OWS2APIPath"),
		OWS2APIPath,
		GGameIni
	);

	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("OWSEncryptionKey"),
		OWSEncryptionKey,
		GGameIni
	);

	//Add initialization parts for tracking when they have all been initialized
	InitiailzationParts.Add(FHWInitializationPart("CUSTOMCHARACTERDATA", 0.5f));
	InitiailzationParts.Add(FHWInitializationPart("INVENTORY", 0.5f));

	//Create UOWSPlayerControllerComponent and bind delegates
	OWSPlayerControllerComponent = CreateDefaultSubobject<UOWSPlayerControllerComponent>(TEXT("OWS Player Controller Component"));

	OWSPlayerControllerComponent->OnNotifyGetCustomCharacterDataDelegate.BindUObject(this, &AHWPlayerController::NotifyGetCustomCharacterData);
	OWSPlayerControllerComponent->OnErrorGetCustomCharacterDataDelegate.BindUObject(this, &AHWPlayerController::ErrorCustomCharacterData);
	/*
	OWSPlayerControllerComponent->OnNotifyPlayerLogoutDelegate.BindUObject(this, &AHWPlayerController::NotifyPlayerLogout);
	OWSPlayerControllerComponent->OnErrorPlayerLogoutDelegate.BindUObject(this, &AHWPlayerController::ErrorPlayerLogout);
	OWSPlayerControllerComponent->OnNotifyCreateCharacterDelegate.BindUObject(this, &AHWPlayerController::NotifyCreateCharacter);
	OWSPlayerControllerComponent->OnErrorCreateCharacterDelegate.BindUObject(this, &AHWPlayerController::ErrorCreateCharacter);
	*/
}

void AHWPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AHWPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HasAuthority())
	{
		InitializeCharacterOnServerSide();
	}
}

//Event called on the Server side when the player leaves
void AHWPlayerController::PawnLeavingGame()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - PawnLeavingGame Started"));

	Super::PawnLeavingGame();
}

//Get a reference to the HWCharacter this Player Controller controls
AHWCharacter* AHWPlayerController::GetHWCharacter() const
{
	return Cast<AHWCharacter>(GetPawn());
}

//Get a reference to the OWSPlayerState for this Player Controller
AOWSPlayerState* AHWPlayerController::GetOWSPlayerState() const
{
	return GetPlayerState<AOWSPlayerState>();
}

UHWAbilitySystemComponent* AHWPlayerController::GetHWAbilitySystemComponent() const
{
	const AHWGASCharacter* GASCharacter = Cast<AHWGASCharacter>(GetHWCharacter());
	return (GASCharacter ? GASCharacter->GetHWAbilitySystemComponent() : nullptr);
}

void AHWPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UHWAbilitySystemComponent* HWASC = GetHWAbilitySystemComponent())
	{
		HWASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

//We might not need this
void AHWPlayerController::TravelToMap(const FString& ServerAndPort, const float X, const float Y, const float Z, const float RX, const float RY,
	const float RZ, const FString& PlayerName, const bool SeamlessTravel)
{
	OWSPlayerControllerComponent->TravelToMap2(ServerAndPort, X, Y, Z, RX, RY, RZ, PlayerName, SeamlessTravel);
}

//Set the Selected Character on the UserSession and then Travel to the Zone that the Character was last on.  This ZoneName is stored in the Characters table.
void AHWPlayerController::SetSelectedCharacterAndConnectToLastZone(FString UserSessionGUID, FString SelectedCharacterName)
{
	//Set character name and get user session
	OWSPlayerControllerComponent->SetSelectedCharacterAndConnectToLastZone(UserSessionGUID, SelectedCharacterName);
}

//Travel to the Zone that the Character was last on.  This ZoneName is stored in the Characters table.
void AHWPlayerController::TravelToLastZoneServer(FString CharacterName)
{
	OWSPlayerControllerComponent->TravelToLastZoneServer(CharacterName);
}

//Call logout on the API
void AHWPlayerController::PlayerLogout()
{
	FString CharacterName = PlayerState->GetPlayerName();
	OWSPlayerControllerComponent->PlayerLogout(CharacterName);
}

void AHWPlayerController::InitializeCharacterOnServerSide()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - InitializeCharacterOnServerSide Started"));

	//Make an async call to get the custom character data for this character.  NotifyGetCustomCharacterData will handle processing the data returned.
	OWSPlayerControllerComponent->GetCustomCharacterData(GetOWSPlayerState()->GetPlayerName());


}

void AHWPlayerController::CustomCharacterDataInitializationComplete()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - CustomCharacterDataInitializationComplete Started"));

	
}

//This runs on the server side to track our initialization tasks
void AHWPlayerController::PartialInitializationComplete(FString InitializationPartNameThatIsComplete)
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - PartialInitializationComplete Started"));

	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("PartialInitializationComplete - Part: %s"), *InitializationPartNameThatIsComplete);

	//Set initialization part complete
	auto FoundPart = InitiailzationParts.FindByPredicate([&](FHWInitializationPart& InPart)
	{
		return InPart.InitializationPartName == InitializationPartNameThatIsComplete;
	});
	if (FoundPart)
	{
		FoundPart->bInitializationComplete = true;
	}

	//Are all the parts complete?
	for (auto& InitializationPart : InitiailzationParts)
	{
		//If any of the parts are incomplete, then we are not done yet
		if (!InitializationPart.bInitializationComplete)
		{
			return;
		}
	}

	//All the Initialization parts are complete.
	ReadyToPlay();
}

//Server side Ready to Play
void AHWPlayerController::ReadyToPlay()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - ReadyToPlay Started"));

	//RPC to the owning client to remove the loading screen and enable player input
	OwningClient_ReadyToPlay();
}

//Owning Client side Ready to Play
void AHWPlayerController::OwningClient_ReadyToPlay_Implementation()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - OwningClient_ReadyToPlay Started"));

	//Hide the loading screen
	HideLoadingScreen();

	//Enable player input
	SetInputMode(FInputModeGameOnly());
}


void AHWPlayerController::HideLoadingScreen()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - HideLoadingScreen Started"));

}

//***** Delegate Bindings *****

//Callback for Get Custom Character Data.  Deserializes the BaseCharacterStats and BaseCharacterSkills and stores the values in the STRUCTs on the Character.
void AHWPlayerController::NotifyGetCustomCharacterData(TSharedPtr<FJsonObject> JsonObject)
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - NotifyGetCustomCharacterData Started"));
	
	if (JsonObject->HasField("rows"))
	{
		TArray<TSharedPtr<FJsonValue>> Rows = JsonObject->GetArrayField("rows");

		//Loop through the Custom Field rows
		for (int RowNum = 0; RowNum != Rows.Num(); RowNum++) {
			TSharedPtr<FJsonObject> tempRow = Rows[RowNum]->AsObject();
			FString CustomFieldName = tempRow->GetStringField("CustomFieldName");
			FString CustomFieldValue = tempRow->GetStringField("FieldValue");

			if (CustomFieldName == "BaseCharacterStats")
			{
				GetHWCharacter()->LoadBaseCharacterStatsFromJSON(CustomFieldValue);
			}
			else if (CustomFieldName == "BaseCharacterSkills")
			{
				GetHWCharacter()->LoadBaseCharacterSkillsFromJSON(CustomFieldValue);
			}
		}

		//Custom Character Data has been loaded.  Continue with additional Character initialization.
		PartialInitializationComplete("CUSTOMCHARACTERDATA");
	}
	else
	{
		UE_LOG(OWSHubWorldMMO, Warning, TEXT("NotifyGetCustomCharacterData Server returned no data!  This can happen when the Character has no Custom Data and might not be an error."));
	}
}

//Callback for Get Custom Character Data.  Called when there is an error
void AHWPlayerController::ErrorCustomCharacterData(const FString& ErrorMsg)
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - ErrorCustomCharacterData: %s"), *ErrorMsg);
	
}
