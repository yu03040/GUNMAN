// Fill out your copyright notice in the Description page of Project Settings.

#include "UICharacter.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"

bool UUICharacter::Initialize()
{
	bool Success = Super::Initialize();

	if (Success == false)
	{
		return false;
	}

	KillCount_TextBlock->TextDelegate.BindUFunction(this, "SetKillCountText");
	Health_ProgressBar->PercentDelegate.BindUFunction(this, "SetHealthProgressBar");

	return true;
}

FText UUICharacter::SetKillCountText()
{
	// プレイヤ―の情報をもらう
	auto Player = UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0);
	TObjectPtr<AGUNMANCharacter> PlayerRef = Cast<AGUNMANCharacter>(Player);
	if (PlayerRef)
	{
		// 敵を5体倒すごとに UI のアニメーションを再生
		int KillCount = PlayerRef->GetKillCount();
		if ((KillCount % 5 == 4) && KillCount != 0)
		{
			PlayAnimation(MoveKillCount);
			return UKismetTextLibrary::Conv_IntToText(KillCount);
		}
		else
		{
			return UKismetTextLibrary::Conv_IntToText(KillCount);
		}
	}
	else
	{
		return FText();
	}
}

float UUICharacter::SetHealthProgressBar()
{
	// プレイヤ―の情報をもらう
	auto Player = UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0);
	TObjectPtr<AGUNMANCharacter> PlayerRef = Cast<AGUNMANCharacter>(Player);
	if (PlayerRef)
	{
		return PlayerRef->CalcHealthPercent();
	}
	else
	{
		return 0.0f;
	}
}
