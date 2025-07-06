// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimationInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class GUNMAN_API IAnimationInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//	武器を装備する関数
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimInterface")
	void EquippedState(bool bHasWeapon, bool bHasPistol);

	//	エイミングフラグ関数
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimInterface")
	void AimingState(bool bIsAiming);

	// 発砲の状態関数
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AnimInterface")
	void FireState(bool CanATK);
};
