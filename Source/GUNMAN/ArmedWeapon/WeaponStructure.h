// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h" // FTableRowBase の定義を含むヘッダーファイル
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "Animation/AnimMontage.h"
#include "WeaponAmmunition.h"
#include "WeaponStructure.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStructure : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 武器をアタッチするソケットの名前（アニメーションシーケンス : ThirdPersonidle） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FName AttachSocketName = "None";

	/** 武器をアタッチするソケットの名前（アニメーションシーケンス : Idle_Rifle_Hip） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FName EquipSocketName = TEXT("Weapon_hand_rソケット");

	/** 装備するときの音 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	USoundBase* EquipmentNoise = NULL;

	/** ピストルを持っているか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool HasPistol = false;

	/** 武器の発射音 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	USoundBase* GunshotSound = NULL;

	/** 武器の発砲炎 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UParticleSystem* MuzzleFire = NULL;

	/** 発砲炎がでる場所 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FName MuzzleFireSoketName = "MuzzleFlash";

	/** 発砲したときの身体のアニメーション */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	UAnimMontage* FiringMontage = NULL;

	/** 弾薬クラス */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TSubclassOf<AWeaponAmmunition> AmmunitionClass = NULL;

	/** 銃弾が出る場所 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	FName AmmunitionSocketName ="AmmoEject";
};
