// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GUNMAN/Enemy/AIEnemyController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EnemyTargetPoint.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API AEnemyTargetPoint : public ATargetPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	int EnemyCounter;

	/* 出現させる敵の最大数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	int MaxEnemyCount;

	/* 敵のブループリント */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APawn> BPEnemyPawnClass;

	/* 敵が出現する間隔 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	float SpawningIntervalTime;

	/* 出現する最小間隔 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	float MinIntervalTime;

	/* 出現する最大間隔 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	float MaxIntervalTime;

	/* 開始ハンドル */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle;

public:
	AEnemyTargetPoint();

	virtual void Tick(float DeltaTime) override;

	/* 敵にAIを付けスポーンさせる関数 */
	UFUNCTION(BlueprintCallable, Category = "EnemyTagetPoint")
	void GenerateEnemyAI(UWorld* World, TSubclassOf<APawn> PawnClass, const FVector &Location, const FRotator &Rotation);

protected:
	virtual void BeginPlay() override;
};
