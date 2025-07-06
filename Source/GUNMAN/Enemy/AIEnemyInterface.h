// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIEnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAIEnemyInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class GUNMAN_API IAIEnemyInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** キャラクターを攻撃する関数 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interface)
	void AttackCharacter();

	/** キャラクターを追いかけるスピードを変更する関数 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interface)
	void ChangeRunningSpeed(float Speed);
};
