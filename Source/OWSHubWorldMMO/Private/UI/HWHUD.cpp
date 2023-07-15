// Copyright 2022 Sabre Dart Studios


#include "./UI/HWHUD.h"

AHWHUD::AHWHUD()
{
	
}

void AHWHUD::NormalAbility1CooldownBegin_Implementation()
{
	NormalAbility1CooldownBegin();
}

void AHWHUD::NormalAbility1CooldownEnd_Implementation()
{
	NormalAbility1CooldownEnd();
}

void AHWHUD::AddFloatingDamageItem(FString DamageText, AActor* DamagedActor, FVector2D InitialDisplayLocationOffset, bool IsHealing, bool IsCritical, bool ShowDropShadow, bool ShowOutline)
{
	if (!DamagedActor)
	{
		return;
	}

	FHWFloatingDamage TempFloatingDamageItem;

	TempFloatingDamageItem.DamageText = DamageText;
	TempFloatingDamageItem.IsHealing = IsHealing;
	TempFloatingDamageItem.IsCritical = IsCritical;
	TempFloatingDamageItem.DamageTextLength = 0.f;
	TempFloatingDamageItem.DamagedActor = DamagedActor;
	TempFloatingDamageItem.DisplayLocationOffset = InitialDisplayLocationOffset;
	TempFloatingDamageItem.TextAlpha = 1.f;
	TempFloatingDamageItem.MarkedForDeletion = false;
	TempFloatingDamageItem.ShowDropShadow = ShowDropShadow;
	TempFloatingDamageItem.ShowOutline = ShowOutline;

	//Save initial location of DamagedActor
	TempFloatingDamageItem.InitialDamagedActorLocation = DamagedActor->GetActorLocation();

	if (!IsHealing) //Damage
	{
		TempFloatingDamageItem.OriginalLength = FloatingDamageMinimumDisplayTime;
	}
	else //Healing
	{
		TempFloatingDamageItem.OriginalLength = FloatingHealingMinimumDisplayTime;
	}

	TempFloatingDamageItem.TimeLeft = TempFloatingDamageItem.OriginalLength;
	FloatingDamageItems.Add(TempFloatingDamageItem);
}

void AHWHUD::CleanUpFloatingDamageItems()
{
	FloatingDamageItems.RemoveAll([](const FHWFloatingDamage item) {
		return item.MarkedForDeletion == true;
		});
}

void AHWHUD::RenderFloatingDamage(float DeltaTime)
{
	for (FHWFloatingDamage& FloatingDamageItem : FloatingDamageItems)
	{
		//Calculate text width the first time
		if (FloatingDamageItem.DamageTextLength <= 0.f)
		{
			float TextWidth = 0.f, TextHeight = 0.f;
			GetTextSize(FloatingDamageItem.DamageText, TextWidth, TextHeight, FloatingDamageFont, 1.f);
			FloatingDamageItem.DamageTextLength = TextWidth;
		}

		//Calculate the initial projection only once
		if (FloatingDamageItem.InitialDamagedActorProjection.X == 9999) //Test for initial state
		{
			FloatingDamageItem.InitialDamagedActorProjection = Project(FloatingDamageItem.InitialDamagedActorLocation);
			FloatingDamageItem.InitialDamagedActorProjection.X += FloatingDamageItem.DisplayLocationOffset.X;
			FloatingDamageItem.DisplayLocationOffset.X = 0.f;
			FloatingDamageItem.InitialDamagedActorProjection.Y += FloatingDamageItem.DisplayLocationOffset.Y;
			FloatingDamageItem.DisplayLocationOffset.Y = 0.f;
		}

		//Calculate the Alpha
		float Alpha = (FloatingDamageItem.OriginalLength - FloatingDamageItem.TimeLeft) / FloatingDamageItem.OriginalLength;

		//Move text up
		if (FloatingDamageItem.IsCritical)
		{
			if (FloatingCriticalDamageSpeedCurve)
			{
				FloatingDamageItem.DisplayLocationOffset.Y = FloatingCriticalDamageSpeedCurve->GetFloatValue(Alpha);
			}
			else
			{
				FloatingDamageItem.DisplayLocationOffset.Y -= FloatingCriticalDamageSpeed.Y * DeltaTime;
			}
		}
		else
		{
			if (FloatingDamageSpeedCurve)
			{
				FloatingDamageItem.DisplayLocationOffset.Y = FloatingDamageSpeedCurve->GetFloatValue(Alpha);
			}
			else
			{
				FloatingDamageItem.DisplayLocationOffset.Y -= FloatingDamageSpeed.Y * DeltaTime;
			}
		}

		//Calculate final location on screen		
		FloatingDamageItem.DisplayLocation.X = FloatingDamageItem.InitialDamagedActorProjection.X - (FloatingDamageItem.DamageTextLength / 2.f) + FloatingDamageItem.DisplayLocationOffset.X;
		FloatingDamageItem.DisplayLocation.Y = FloatingDamageItem.InitialDamagedActorProjection.Y + FloatingDamageItem.DisplayLocationOffset.Y;
			
		//Remove DeltaTime from TimeLeft
		FloatingDamageItem.TimeLeft -= DeltaTime;

		//Has time expired?
		if (FloatingDamageItem.TimeLeft < 0.1f)
		{
			//Mark for deletion that will happen in CleanUpFloatingDamageItems()
			FloatingDamageItem.MarkedForDeletion = true;
		}
		else
		{
			UFont* FloatingTextFont;
			UFont* FloatingTextOutlineFont;
			FLinearColor TextColorToRender;
			FLinearColor DropShadowColorToRender;
			FLinearColor OutlineColorToRender;
			float DropShadowOffsetX;
			float DropShadowOffsetY;

			//Is this damage or healing?
			if (!FloatingDamageItem.IsHealing) //Damage
			{
				if (!FloatingDamageItem.IsCritical)
				{
					FloatingTextFont = FloatingDamageFont;
					FloatingTextOutlineFont = FloatingDamageOutlineFont;

					TextColorToRender = FloatingDamageFontColor;
					DropShadowColorToRender = FloatingDamageDropShadowColor;
					OutlineColorToRender = FloatingDamageOutlineColor;

					DropShadowOffsetX = FloatingDamageDropShadowOffsetX;
					DropShadowOffsetY = FloatingDamageDropShadowOffsetY;

					if (FloatingDamageTextAlphaCurve)
					{
						FloatingDamageItem.TextAlpha = FloatingDamageTextAlphaCurve->GetFloatValue(Alpha);
					}
					//Fade out if FloatingDamageFadeOutSpeed > 0.f
					else if (FloatingDamageFadeOutSpeed > 0.f)
					{
						FloatingDamageItem.TextAlpha = FMath::Clamp(FloatingDamageItem.TextAlpha - ((FloatingDamageFadeOutSpeed / FloatingDamageItem.OriginalLength) * DeltaTime), 0.f, 1.f);
					}
				}
				else
				{
					FloatingTextFont = FloatingCriticalDamageFont;
					FloatingTextOutlineFont = FloatingCriticalDamageOutlineFont;

					TextColorToRender = FloatingCriticalDamageFontColor;
					DropShadowColorToRender = FloatingCriticalDamageDropShadowColor;
					OutlineColorToRender = FloatingCriticalDamageOutlineColor;

					DropShadowOffsetX = FloatingCriticalDamageDropShadowOffsetX;
					DropShadowOffsetY = FloatingCriticalDamageDropShadowOffsetY;

					if (FloatingCriticalDamageTextAlphaCurve)
					{
						FloatingDamageItem.TextAlpha = FloatingCriticalDamageTextAlphaCurve->GetFloatValue(Alpha);
					}
					//Fade out if FloatingCriticalDamageFadeOutSpeed > 0.f
					else if (FloatingCriticalDamageFadeOutSpeed > 0.f)
					{
						FloatingDamageItem.TextAlpha = FMath::Clamp(FloatingDamageItem.TextAlpha - ((FloatingCriticalDamageFadeOutSpeed / FloatingDamageItem.OriginalLength) * DeltaTime), 0.f, 1.f);
					}
				}
			}
			else //Healing
			{
				if (!FloatingDamageItem.IsCritical)
				{
					FloatingTextFont = FloatingHealingFont;
					FloatingTextOutlineFont = FloatingHealingOutlineFont;

					TextColorToRender = FloatingHealingFontColor;
					DropShadowColorToRender = FloatingHealingDropShadowColor;
					OutlineColorToRender = FloatingHealingOutlineColor;

					DropShadowOffsetX = FloatingHealingDropShadowOffsetX;
					DropShadowOffsetY = FloatingHealingDropShadowOffsetY;

					//Fade out if FloatingHealingFadeOutSpeed > 0.f
					if (FloatingHealingFadeOutSpeed > 0.f)
					{
						FloatingDamageItem.TextAlpha = FMath::Clamp(FloatingDamageItem.TextAlpha - ((FloatingHealingFadeOutSpeed / FloatingDamageItem.OriginalLength) * DeltaTime), 0.f, 1.f);
					}
				}
				else
				{
					FloatingTextFont = FloatingCriticalHealingFont;
					FloatingTextOutlineFont = FloatingCriticalHealingOutlineFont;

					TextColorToRender = FloatingCriticalHealingFontColor;
					DropShadowColorToRender = FloatingCriticalHealingDropShadowColor;
					OutlineColorToRender = FloatingCriticalHealingOutlineColor;

					DropShadowOffsetX = FloatingCriticalHealingDropShadowOffsetX;
					DropShadowOffsetY = FloatingCriticalHealingDropShadowOffsetY;

					//Fade out if FloatingCriticalHealingFadeOutSpeed > 0.f
					if (FloatingCriticalHealingFadeOutSpeed > 0.f)
					{
						FloatingDamageItem.TextAlpha = FMath::Clamp(FloatingDamageItem.TextAlpha - ((FloatingCriticalHealingFadeOutSpeed / FloatingDamageItem.OriginalLength) * DeltaTime), 0.f, 1.f);
					}
				}
			}

			//Get scale from curve if it exists
			float Scale = 1.f;
			if (FloatingDamageItem.IsCritical && FloatingDamageScaleCurve)
			{
				Scale = FloatingDamageScaleCurve->GetFloatValue(Alpha);
			}
			if (FloatingDamageItem.IsCritical && FloatingCriticalDamageScaleCurve)
			{
				Scale = FloatingCriticalDamageScaleCurve->GetFloatValue(Alpha);
			}

			TextColorToRender.A = FloatingDamageItem.TextAlpha;
			DropShadowColorToRender.A = FloatingDamageItem.TextAlpha;
			OutlineColorToRender.A = FloatingDamageItem.TextAlpha;

			if (FloatingDamageItem.ShowDropShadow)
			{
				DrawText(FloatingDamageItem.DamageText, DropShadowColorToRender, FloatingDamageItem.DisplayLocation.X + DropShadowOffsetX,
					FloatingDamageItem.DisplayLocation.Y + DropShadowOffsetY, FloatingTextFont, Scale, false);
			}
			if (FloatingDamageItem.ShowOutline)
			{
				DrawText(FloatingDamageItem.DamageText, OutlineColorToRender, FloatingDamageItem.DisplayLocation.X, FloatingDamageItem.DisplayLocation.Y, 
					FloatingTextOutlineFont, Scale, false);
			}
			DrawText(FloatingDamageItem.DamageText, TextColorToRender, FloatingDamageItem.DisplayLocation.X, FloatingDamageItem.DisplayLocation.Y, FloatingTextFont, 
				Scale, false);
		}
	}
}