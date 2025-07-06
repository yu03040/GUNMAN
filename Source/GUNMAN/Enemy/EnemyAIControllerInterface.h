// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyAIControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyAIControllerInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class GUNMAN_API IEnemyAIControllerInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 受け取ったアクターの情報を更新する関数 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interface)
	void UpdateTargetActorKey(AActor* TargetActor);

	/** 視界に入ったか確認するフラグ関数 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interface)
	void UpdateHasLineOfSightKey(bool HasLineOfSight);
};
