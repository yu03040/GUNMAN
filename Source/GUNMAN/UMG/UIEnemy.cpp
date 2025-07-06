// Fill out your copyright notice in the Description page of Project Settings.

#include "UIEnemy.h"
#include "Kismet/GamePlayStatics.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "Components/ProgressBar.h"

bool UUIEnemy::Initialize()
{
	bool Success = Super::Initialize();

	if (Success == false)
	{
		return false;
	}

	Health_ProgressBar->PercentDelegate.BindUFunction(this, "SetHealthProgressBar");

	return true;
}

float UUIEnemy::SetHealthProgressBar()
{
	if (OwningEnemy)
	{
		return OwningEnemy->GetHealthPercent();
	}
	else
	{
		return 0.0f;
	}
}

void UUIEnemy::SetOwningEnemy(TObjectPtr<class AAIEnemy> NewOwner)
{
	OwningEnemy = NewOwner;
}
