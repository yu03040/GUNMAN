// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GUNMANGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API AGUNMANGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGUNMANGameModeBase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
