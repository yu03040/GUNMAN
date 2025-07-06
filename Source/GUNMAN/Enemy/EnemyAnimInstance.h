// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	/* プレイヤーの参照 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<ACharacter> EnemyCharacterRef;

	/* 毎フレーム */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float DeltaTimeX;

	/* 空中にいるか？ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsInAir = false;

	/* 歩行速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float Speed = 0.0f;

	/* 向いている方向 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float Direction = 0.0f;

	/* 構える向き（y方向） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float AimPitch = 0.0f;

	/* 構える向き（z方向） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float AimYaw = 0.0f;

public:

	UEnemyAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
