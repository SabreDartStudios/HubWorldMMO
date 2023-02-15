// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OWSPlayerControllerComponent.h"
#include "./Character/HWCharacter.h"
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
		bInitializationComplete = false;
	}

	FHWInitializationPart(FString inInitializationPartName, float inInitializationPartPercentage)
	{
		InitializationPartName = inInitializationPartName;
		InitializationPartPercentage = inInitializationPartPercentage;
		bInitializationComplete = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		FString InitializationPartName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		float InitializationPartPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initiailzation")
		bool bInitializationComplete;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		UHWInventoryComponent* InventoryComponentForInventoryUI;

	UPROPERTY(BlueprintReadWrite)
		FString OWSAPICustomerKey;

	UPROPERTY(BlueprintReadWrite, Category = "Config")
		FString OWS2APIPath = "";

	UPROPERTY(BlueprintReadWrite, Category = "Config")
		FString OWSEncryptionKey = "";

	UFUNCTION(BlueprintCallable, Category = "Player Character")
		AHWCharacter* GetHWCharacter() const;

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
		TArray<FHWInitializationPart> InitiailzationParts;

	//This is where Character Initialization code starts
	void InitializeCharacterOnServerSide();

	//Called when Custom Character Data Initialization is complete
	void CustomCharacterDataInitializationComplete();

	//Caled each time part of the initialization comlpeltes.  When all the parts are done it calls ReadyToPlay.
	void PartialInitializationComplete(FString initializationPartName);

	//Called when everything has been initialized and we are ready to turn the game over to the player to play.
	void ReadyToPlay();

	UFUNCTION(Client, Reliable, Category = "Initialization")
		void OwningClient_ReadyToPlay();

	//Hide the loading screen
	void HideLoadingScreen();

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

	/*
	//Get Map Server to Travel To
	UFUNCTION(BlueprintCallable, Category = "Travel")
		void GetMapServerToTravelTo(FString ZoneName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Travel")
		void NotifyMapServerToTravelTo(const FString& ServerAndPort);
	UFUNCTION(BlueprintImplementableEvent, Category = "Travel")
		void ErrorMapServerToTravelTo(const FString& ErrorMsg);
	*/
	
	//Player Controller Component Delegate Bindings
	void NotifyGetCustomCharacterData(TSharedPtr<FJsonObject> JsonObject);
	void ErrorCustomCharacterData(const FString& ErrorMsg);

protected:

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
