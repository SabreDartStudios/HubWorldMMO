// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HWHUD.generated.h"

DECLARE_DELEGATE(FNormalAbility1CooldownBeginDelegate);
DECLARE_DELEGATE(FNormalAbility1CooldownEndDelegate);

USTRUCT(BlueprintType)
struct FHWFloatingDamage
{
	GENERATED_BODY()

	FHWFloatingDamage() {
		DamageText = "";
		DamageTextLength = 0.f;
		OriginalLength = 0.f;
		TimeLeft = 0.f;
		TextAlpha = 0.f;
		Scale = 1.f;
		InitialDamagedActorLocation = FVector(0);
		InitialDamagedActorProjection = FVector(9999); //Start at some impossible value as an initial state so we can test for initial state
		DisplayLocation = FVector2D(0);
		DisplayLocationOffset = FVector2D(0);
		DamagedActor = nullptr;
		IsHealing = true;
		IsCritical = true;
		MarkedForDeletion = true;
		ShowDropShadow = true;
		ShowOutline = true;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		FString DamageText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		float DamageTextLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		float OriginalLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		float TimeLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		float TextAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		FVector InitialDamagedActorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		FVector InitialDamagedActorProjection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		FVector2D DisplayLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		FVector2D DisplayLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		AActor* DamagedActor;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		bool IsHealing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		bool IsCritical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		bool MarkedForDeletion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		bool ShowDropShadow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
		bool ShowOutline;
};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AHWHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		UCurveFloat* FloatingDamageScaleCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		UCurveFloat* FloatingDamageSpeedCurve;
	/** This setting is ignored if FloatingDamageSpeedCurve is set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		FVector2D FloatingDamageSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		UCurveFloat* FloatingDamageTextAlphaCurve;
	/** This setting is ignored if FloatingDamageTextAlphaCurve is set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		float FloatingDamageFadeOutSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		UFont* FloatingDamageFont;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		FLinearColor FloatingDamageFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		FLinearColor FloatingDamageDropShadowColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		float FloatingDamageDropShadowOffsetX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		float FloatingDamageDropShadowOffsetY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		FLinearColor FloatingDamageOutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		UFont* FloatingDamageOutlineFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Damage")
		float FloatingDamageMinimumDisplayTime;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		UCurveFloat* FloatingCriticalDamageScaleCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		UCurveFloat* FloatingCriticalDamageSpeedCurve;
	/** This setting is ignored if FloatingCriticalDamageSpeedCurve is set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		FVector2D FloatingCriticalDamageSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		UCurveFloat* FloatingCriticalDamageTextAlphaCurve;
	/** This setting is ignored if FloatingCriticalDamageTextAlphaCurve is set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		float FloatingCriticalDamageFadeOutSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		UFont* FloatingCriticalDamageFont;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		FLinearColor FloatingCriticalDamageFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		FLinearColor FloatingCriticalDamageDropShadowColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		float FloatingCriticalDamageDropShadowOffsetX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		float FloatingCriticalDamageDropShadowOffsetY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		FLinearColor FloatingCriticalDamageOutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		UFont* FloatingCriticalDamageOutlineFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Damage")
		float FloatingCriticalDamageMinimumDisplayTime;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		UFont* FloatingHealingFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		FVector2D FloatingHealingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		FLinearColor FloatingHealingFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		FLinearColor FloatingHealingDropShadowColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		float FloatingHealingDropShadowOffsetX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		float FloatingHealingDropShadowOffsetY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		FLinearColor FloatingHealingOutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		UFont* FloatingHealingOutlineFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		float FloatingHealingMinimumDisplayTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Healing")
		float FloatingHealingFadeOutSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		UFont* FloatingCriticalHealingFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		FVector2D FloatingCriticalHealingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		FLinearColor FloatingCriticalHealingFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		FLinearColor FloatingCriticalHealingDropShadowColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		float FloatingCriticalHealingDropShadowOffsetX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		float FloatingCriticalHealingDropShadowOffsetY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		FLinearColor FloatingCriticalHealingOutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		UFont* FloatingCriticalHealingOutlineFont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		float FloatingCriticalHealingMinimumDisplayTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Critical Healing")
		float FloatingCriticalHealingFadeOutSpeed;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void AddFloatingDamageItem(FString DamageText, AActor* DamagedActor, FVector2D InitialDisplayLocationOffset, bool IsHealing = false, bool IsCritical = false, bool ShowDropShadow = false, bool ShowOutline = false);

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void CleanUpFloatingDamageItems();

	UFUNCTION(BlueprintNativeEvent, Category = "Cooldowns")
		void NormalAbility1CooldownBegin();
	UFUNCTION(BlueprintNativeEvent, Category = "Cooldowns")
		void NormalAbility1CooldownEnd();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		TArray<FHWFloatingDamage> FloatingDamageItems;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void RenderFloatingDamage(float DeltaTime);

};
