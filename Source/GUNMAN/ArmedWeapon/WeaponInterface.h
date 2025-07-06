// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class GUNMAN_API IWeaponInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	/**
	* 武器をアタッチする関数
	* @param WeaponMesh 武器のメッシュ
	* @param WeaponSoketName 武器をアタッチする場所の名前
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponInterface")
	void AttachWeapon(USkeletalMeshComponent* WeaponMesh, FName AttachSoketName);
};