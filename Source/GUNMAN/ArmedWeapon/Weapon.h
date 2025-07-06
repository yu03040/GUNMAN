// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GUNMAN/ArmedWeapon/WeaponStructure.h"
#include "Weapon.generated.h"

UCLASS()
class GUNMAN_API AWeapon : public AActor
{
	GENERATED_BODY()

		/** Scene Component */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rifle, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<USceneComponent> DefaultSceneRoot;

	/** 武器のメッシュ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rifle, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UDataTable> WeaponDataTable;
public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
