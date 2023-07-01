// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/NetSerialization.h"
#include "OWSPlayerControllerComponent.h"
#include "./Character/HWGASPlayerCharacter.h"
#include "./Inventory/HWInventoryComponent.h"
#include "./AbilitySystem/HWAbilitySystemComponent.h"
#include "HWPlayerController.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FHWInitializationPart
{
	GENERATED_USTRUCT_BODY()

	FHWInitializationPart()
	{
		InitializationPartName = "";
		InitializationPartPercentage = 1.f;
		bServerSide = true;
		bInitializationComplete = false;
	}

	FHWInitializationPart(FString inInitializationPartName, float inInitializationPartPercentage, bool inServerSide)
	{
		InitializationPartName = inInitializationPartName;
		InitializationPartPercentage = inInitializationPartPercentage;
		bServerSide = inServerSide;
		bInitializationComplete = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		FString InitializationPartName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		float InitializationPartPercentage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		bool bServerSide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		bool bInitializationComplete;

};

USTRUCT(BlueprintType, Blueprintable)
struct FHWSupplyPodOpenedItem : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

		friend struct FHWSupplyPodMaster;

	FHWSupplyPodOpenedItem()
	{
		SupplyPodGUID = FGuid();
	}

	/*
	void PreReplicatedRemove(const struct FHWSupplyPodMaster& InArray);
	void PostReplicatedAdd(const struct FHWSupplyPodMaster& InArray);
	void PostReplicatedChange(const struct FHWSupplyPodMaster& InArray);
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
		FGuid SupplyPodGUID;
};

USTRUCT(BlueprintType, Blueprintable)
struct FHWSupplyPodMaster : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

		friend struct FHWSupplyPodOpenedItem;

	FHWSupplyPodMaster()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
		TArray<FHWSupplyPodOpenedItem> SupplyPods;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FHWSupplyPodOpenedItem>(SupplyPods, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FHWSupplyPodMaster> : public TStructOpsTypeTraitsBase2<FHWSupplyPodMaster> //TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHWPlayerController();

	UPROPERTY(BlueprintReadOnly)
		UOWSPlayerControllerComponent* OWSPlayerControllerComponent;

	UFUNCTION(BlueprintCallable, Category = "HW|PlayerController")
		UHWAbilitySystemComponent* GetHWAbilitySystemComponent() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HW|Inventory")
		UHWInventoryComponent* InventoryComponentForInventoryUI;

	//Interaction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HW|Interaction")
		float InteractionRadius = 200.f;

	UFUNCTION(BlueprintCallable, Category = "HW|Interaction")
		void Interact();

		TArray<TWeakObjectPtr<AActor>> GetOverlappedInteractables();

	// Supply Pods Opened (Replicated)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SupplyPodsOpened, Category = "HW|Persistence")
		FHWSupplyPodMaster SupplyPodsOpened;

	UFUNCTION(BlueprintCallable, Category = "HW|Persistence")
		bool IsSupplyPodOpened(FGuid SupplyPodGUID);

	UFUNCTION()
		virtual void OnRep_SupplyPodsOpened();

	void RefreshStateOfRelevantSuppplyPods();

	// Serialize Base Character Stats to an FString
	UFUNCTION(BlueprintCallable, Category = "HW|Persistence")
		FString SerializeSupplyPodsOpened();

	// Deserialize an FString to Base Character Stats
	UFUNCTION(BlueprintCallable, Category = "HW|Persistence")
		void LoadSupplyPodsOpenedFromJSON(FString SupplyPodsOpenedJSON);

	UFUNCTION(BlueprintCallable, Category = "HW|Persistence")
		void AddSupplyPodToOpenedList(FGuid SupplyPodGUID);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
		void Server_OpenSupplyPod();

	//OWS
	UPROPERTY(BlueprintReadWrite)
		FString OWSAPICustomerKey;

	UPROPERTY(BlueprintReadWrite, Category = "Config")
		FString OWS2APIPath = "";

	UPROPERTY(BlueprintReadWrite, Category = "Config")
		FString OWSEncryptionKey = "";

	UFUNCTION(BlueprintCallable, Category = "Player Character")
		AHWGASPlayerCharacter* GetHWGASPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "Player State")
		AOWSPlayerState* GetOWSPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Travel")
		void TravelToMap(const FString& ServerAndPort, const float X, const float Y, const float Z, const float RX, const float RY,
			const float RZ, const FString& PlayerName, const bool SeamlessTravel);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	void PawnLeavingGame();


	//Initiailization
	UPROPERTY(EditAnywhere, Category = "HW|Initialization")
		TArray<FHWInitializationPart> InitializationParts;

	//This is where Character Initialization code starts
	void InitializeCharacterOnServerSide();

	//Called when Custom Character Data Initialization is complete
	void CustomCharacterDataInitializationComplete();

	//Caled each time part of the initialization comlpeltes.  When all the parts are done it calls ReadyToPlay.
	void PartialInitializationComplete(FString initializationPartName);

	//Called when everything has been initialized and we are ready to turn the game over to the player to play.
	void ReadyToPlay();

	UFUNCTION(BlueprintImplementableEvent, Category = "Initialization")
		void ReadyToPlayOnServer();

	UFUNCTION(Client, Reliable, Category = "Initialization")
		void OwningClient_ReadyToPlay();

	UFUNCTION(BlueprintImplementableEvent, Category = "Initialization")
		void ReadyToPlayOnClient();

	//Show the loading screen
	void ShowLoadingScreen();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading Screen")
		void BP_ShowLoadingScreen();

	//Hide the loading screen
	void HideLoadingScreen();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading Screen")
		void BP_HideLoadingScreen();

	//SetSelectedCharacterAndConnectToLastZone
	UFUNCTION(BlueprintCallable, Category = "Travel")
		void SetSelectedCharacterAndConnectToLastZone(FString UserSessionGUID, FString SelectedCharacterName);

	void TravelToLastZoneServer(FString CharacterName);

	/*
	FString ServerTravelUserSessionGUID;
	FString ServerTravelCharacterName;
	float ServerTravelX;
	float ServerTravelY;
	float ServerTravelZ;
	float ServerTravelRX;
	float ServerTravelRY;
	float ServerTravelRZ;
	*/

	FVector LastCharacterLocation;
	FRotator LastCharacterRotation;

	/*
	//Create Character
	UFUNCTION(BlueprintCallable, Category = "Login")
		void CreateCharacter(FString UserSessionGUID, FString CharacterName, FString ClassName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void NotifyCreateCharacter(const FCreateCharacter& CreateCharacterData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void ErrorCreateCharacter(const FString& ErrorMsg);
	*/

	//Player Logout
	UFUNCTION(BlueprintCallable, Category = "Player")
		void PlayerLogout();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
		void NotifyPlayerLogout();
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
		void ErrorPlayerLogout(const FString& ErrorMsg);

	//GetZoneServerToTravelTo
	UFUNCTION(BlueprintCallable, Category = "Travel")
		void GetZoneServerToTravelTo(APlayerController* PlayerController, TEnumAsByte<ERPGSchemeToChooseMap::SchemeToChooseMap> SelectedSchemeToChooseMap, 
			int32 WorldServerID, FString ZoneName, FVector LocationOnMap, FRotator StartingRotation);

	//Player Controller Component Delegate Bindings
	void NotifyGetCustomCharacterData(TSharedPtr<FJsonObject> JsonObject);
	void ErrorCustomCharacterData(const FString& ErrorMsg);

	void NotifyZoneServerToTravelTo(const FString& ServerAndPort);
	void ErrorZoneServerToTravelTo(const FString& ErrorMsg);

	

protected:

	virtual void BeginPlayingState() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	FVector ZoneToTravelToLocation;
	FRotator ZoneToTravelToStartingRotation;
};
