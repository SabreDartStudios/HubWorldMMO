// Fill out your copyright notice in the Description page of Project Settings.


#include "./Player/HWPlayerController.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Net/UnrealNetwork.h"
#include "./Character/HWGASCharacter.h"
#include "OWSGameInstance.h"
#include "./Interactables/Interactable.h"
#include "./Interactables/SupplyPod.h"
#include "Kismet/GameplayStatics.h"
#include "../OWSHubWorldMMO.h"

typedef TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > FCondensedJsonStringWriterFactory;
typedef TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > FCondensedJsonStringWriter;

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

	//Add initialization parts for tracking when they have all been initialized - Server Side
	InitializationParts.Add(FHWInitializationPart("CUSTOMCHARACTERDATA", 0.5f, true));
	InitializationParts.Add(FHWInitializationPart("PLAYERSTATE", 0.5f, true));
	//Add initialization parts for tracking when they have all been initialized - Client Side
	InitializationParts.Add(FHWInitializationPart("PLAYERSTATE", 0.5f, false));
	InitializationParts.Add(FHWInitializationPart("SERVERSIDEDONE", 0.5f, false));

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

//This is call on the client and the server.  The client fires quite a bit later than the server side.
void AHWPlayerController::BeginPlayingState()
{
	//Player State has been loaded.  Continue with additional Character initialization.
	PartialInitializationComplete("PLAYERSTATE");
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

//This runs on the client and server to track our initialization tasks
void AHWPlayerController::PartialInitializationComplete(FString InitializationPartNameThatIsComplete)
{
	bool bIsServer = HasAuthority();

	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - PartialInitializationComplete Started"));

	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("PartialInitializationComplete - Part: %s"), *InitializationPartNameThatIsComplete);

	//Set initialization part complete
	auto FoundPart = InitializationParts.FindByPredicate([&](FHWInitializationPart& InPart)
	{
		return InPart.InitializationPartName == InitializationPartNameThatIsComplete && InPart.bServerSide == bIsServer;
	});
	if (FoundPart)
	{
		FoundPart->bInitializationComplete = true;
	}

	//Are all the parts complete?
	for (auto& InitializationPart : InitializationParts)
	{
		//If any of the parts are incomplete, then we are not done yet
		if (!InitializationPart.bInitializationComplete && InitializationPart.bServerSide == bIsServer)
		{
			return;
		}
	}

	//All the Initialization parts are complete.
	ReadyToPlay();
}

//Client and Server side Ready to Play
void AHWPlayerController::ReadyToPlay()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - ReadyToPlay Started"));

	if (HasAuthority())
	{
		//Call the BP implementable event
		ReadyToPlayOnServer();

		//RPC to the owning client to let it know the server side initialization tasks are done
		OwningClient_ReadyToPlay();
	}
	else
	{
		//Hide the loading screen
		HideLoadingScreen();

		//Enable player input
		SetInputMode(FInputModeGameOnly());

		//Call the BP implementable event
		ReadyToPlayOnClient();
	}
}

//Owning Client side Ready to Play
void AHWPlayerController::OwningClient_ReadyToPlay_Implementation()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - OwningClient_ReadyToPlay Started"));

	PartialInitializationComplete("SERVERSIDEDONE");
}


void AHWPlayerController::HideLoadingScreen()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("AHWPlayerController - HideLoadingScreen Started"));

}

/***** Supply Pods *****/

FString AHWPlayerController::SerializeSupplyPodsOpened()
{
	FString SerializedSupplyPodsOpened = "";
	if (FJsonObjectConverter::UStructToJsonObjectString(SupplyPodsOpened, SerializedSupplyPodsOpened, 0, EPropertyFlags::CPF_RepSkip))
	{
		return SerializedSupplyPodsOpened;
	}
	return "{}";

	/*
	FString SerializedSupplyPodsOpened = "";
	FJsonObject ArrayJsonObject;
	TArray<TSharedPtr<FJsonValue>> Array;
	for (auto SupplyPodOpened : SupplyPodsOpened.SupplyPods)
	{
		ArrayJsonObject.SetStringField("TempField", SupplyPodOpened.SupplyPodGUID.ToString());
		Array.Add(ArrayJsonObject.GetField<EJson::String>("TempField"));
	}
	TSharedRef<FCondensedJsonStringWriter> Writer = FCondensedJsonStringWriterFactory::Create(&SerializedSupplyPodsOpened);
	FJsonSerializer::Serialize(Array, Writer);

	return SerializedSupplyPodsOpened;
	*/
}

void AHWPlayerController::LoadSupplyPodsOpenedFromJSON(FString SupplyPodsOpenedJSON)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(SupplyPodsOpenedJSON, &SupplyPodsOpened);
	SupplyPodsOpened.MarkArrayDirty();

	/*
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(SupplyPodsOpenedJSON);
	TArray<TSharedPtr<FJsonValue>> JsonSupplyPodsArray;
	FJsonSerializer::Deserialize(JsonReader, JsonSupplyPodsArray);

	for (auto SupplyPodOpened : JsonSupplyPodsArray)
	{
		FHWSupplyPodOpenedItem HWSupplyPodOpenedItem;
		HWSupplyPodOpenedItem.SupplyPodGUID = FGuid(SupplyPodOpened->AsString());
		FHWSupplyPodOpenedItem& ItemAdded = SupplyPodsOpened.SupplyPods.Add_GetRef(HWSupplyPodOpenedItem);
		SupplyPodsOpened.MarkItemDirty(ItemAdded);
	}
	*/
}

void AHWPlayerController::OnRep_SupplyPodsOpened()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - OnRep_SupplyPodsOpened Started"));

	RefreshStateOfRelevantSuppplyPods();
}

bool AHWPlayerController::IsSupplyPodOpened(FGuid SupplyPodGUID)
{
	auto FoundEntry = SupplyPodsOpened.SupplyPods.FindByPredicate([&](FHWSupplyPodOpenedItem& InItem)
		{
			return InItem.SupplyPodGUID == SupplyPodGUID;
		});

	if (FoundEntry)
	{
		return true;
	}

	return false;
}

//This method is only called on the server
void AHWPlayerController::AddSupplyPodToOpenedList(FGuid SupplyPodGUID)
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - AddSupplyPodToOpenedList Started"));

	FHWSupplyPodOpenedItem SupplyPodOpenedItemToAdd;

	SupplyPodOpenedItemToAdd.SupplyPodGUID = SupplyPodGUID;

	FHWSupplyPodOpenedItem& ItemAdded = SupplyPodsOpened.SupplyPods.Add_GetRef(SupplyPodOpenedItemToAdd);
	SupplyPodsOpened.MarkItemDirty(ItemAdded);

	//Serialize the array of Supply Pods Opened into a JSON String
	FString SerializedSupplyPodsOpened = SerializeSupplyPodsOpened();

	//Calls the OWS API to save SupplyPodsOpened to Custom Character Data
	OWSPlayerControllerComponent->AddOrUpdateCustomCharacterData(GetOWSPlayerState()->GetPlayerName(), "SupplyPodsOpened", SerializedSupplyPodsOpened);
}

void AHWPlayerController::Server_OpenSupplyPod_Implementation()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - Server_OpenSupplyPod Started"));

	//Get a list of Interactables within range to interact with (InteractionRadius)
	TArray<TWeakObjectPtr<AActor>>	OverlappedInteractables = GetOverlappedInteractables();

	//If there are no OverlappedInteractables, do nothing
	if (OverlappedInteractables.IsEmpty())
	{
		return;
	}

	//Figure out which one to open - For now just pick the first one in the list
	IInteractable* InteractableSupplyPod = Cast<IInteractable>(OverlappedInteractables[0]);
	FGuid SupplyPodGUID = InteractableSupplyPod->GetInteractableGUID();

	AddSupplyPodToOpenedList(SupplyPodGUID);
	InteractableSupplyPod->Interact();
}

void AHWPlayerController::RefreshStateOfRelevantSuppplyPods()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - RefreshStateOfRelevantSuppplyPods Started"));

	TArray<AActor*> NetworkRelevantSupplyPodsActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASupplyPod::StaticClass(), NetworkRelevantSupplyPodsActors);

	for (AActor* NetworkRelevantSupplyPodActor : NetworkRelevantSupplyPodsActors)
	{
		ASupplyPod* NetworkRelevantSupplyPod = Cast<ASupplyPod>(NetworkRelevantSupplyPodActor);
		if (NetworkRelevantSupplyPod)
		{
			NetworkRelevantSupplyPod->RefreshSupplyPodState();
			UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - Refreshed State of Supply Pod: %s"), *NetworkRelevantSupplyPod->GetInteractableGUID().ToString());
		}
	}
}

//Called via local input to Interact with an Interactable
void AHWPlayerController::Interact()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWPlayerController - Interact Started"));

	//Get a list of Interactables within range to interact with (InteractionRadius)
	TArray<TWeakObjectPtr<AActor>>	OverlappedInteractables = GetOverlappedInteractables();

	//If there are no OverlappedInteractables, do nothing
	if (OverlappedInteractables.IsEmpty())
	{
		return;
	}

	//Figure out which one to open - For now just pick the first one in the list
	IInteractable* InteractableSupplyPod = Cast<IInteractable>(OverlappedInteractables[0]);

	//If the Supply Pod is already Open, then do nothing
	if (IsSupplyPodOpened(InteractableSupplyPod->GetInteractableGUID()))
	{
		return;
	}

	InteractableSupplyPod->Interact();

	//Run at Server RPC to open a Supply Pod
	Server_OpenSupplyPod();
}

TArray<TWeakObjectPtr<AActor>> AHWPlayerController::GetOverlappedInteractables()
{
	//Get character transform
	FTransform CharacterTransform = GetHWCharacter()->GetActorTransform();

	//Sphere trace for Interactables by first scanning in a sphere around the character for WorldDynamic actors within InteractionRadius range
	TArray<FOverlapResult> Overlaps;
	bool bTraceComplex = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(RadiusTargetingOverlap), bTraceComplex);
	Params.bReturnPhysicalMaterial = false;
	GetWorld()->OverlapMultiByObjectType(Overlaps, CharacterTransform.GetTranslation(), FQuat::Identity, FCollisionObjectQueryParams(ECC_WorldDynamic),
		FCollisionShape::MakeSphere(InteractionRadius), Params);

	//Loop through the Overlaps and only add those implementing UInteractable to the OverlappedInteractables array
	TArray<TWeakObjectPtr<AActor>>	OverlappedInteractables;
	for (int32 i = 0; i < Overlaps.Num(); ++i)
	{
		AActor* OverlappedActor = Cast<AActor>(Overlaps[i].GetActor());
		//Make sure the reference is valid and we aren't interacting with ourselves
		if (OverlappedActor && OverlappedActor != GetHWCharacter())
		{
			//Make sure this overlapped actor is Interactable (implementing UInteractable)
			if (OverlappedActor->Implements<UInteractable>())
			{
				OverlappedInteractables.Add(OverlappedActor);
			}
		}
	}

	return OverlappedInteractables;
}

void AHWPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Fast Arrays don't use push model, but there's no harm in marking them with it.
	// The flag will just be ignored.

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(AHWPlayerController, SupplyPodsOpened, Params);
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
			else if (CustomFieldName == "SupplyPodsOpened")
			{
				LoadSupplyPodsOpenedFromJSON(CustomFieldValue);
			}
			else if (CustomFieldName.Contains("Inventory"))
			{
				TInlineComponentArray<UHWInventoryComponent*> AttachedInventoryComponents;
				GetComponents<UHWInventoryComponent>(AttachedInventoryComponents);
				for (int32 curIndex = 0; curIndex < AttachedInventoryComponents.Num(); curIndex++)
				{
					if (AttachedInventoryComponents[curIndex]->InventoryName == CustomFieldName)
					{
						AttachedInventoryComponents[curIndex]->LoadInventoryFromJSON(CustomFieldValue);
					}
				}
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
