// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyTargetPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyTargetPoint.h"

AEnemyTargetPoint::AEnemyTargetPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyCounter = 0;
	MaxEnemyCount = 0;
	BPEnemyPawnClass = NULL;
	SpawningIntervalTime = 0;
	MinIntervalTime = 0.0f;
	MaxIntervalTime = 0.0f;
}

void AEnemyTargetPoint::BeginPlay()
{
	Super::BeginPlay();

	// 出現間隔をランダムに
	SpawningIntervalTime = UKismetMathLibrary::RandomFloatInRange(MinIntervalTime, MaxIntervalTime);

	FTimerManager& TimerManager = GetWorldTimerManager();
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(
		this,
		FName(TEXT("GenerateEnemyAI")),
		GetWorld(),
		BPEnemyPawnClass,
		this->GetActorLocation(),
		this->GetActorRotation());

	// 出現間隔ごとに敵を生成
	TimerManager.SetTimer(
		TimerHandle,
		TimerDelegate,
		SpawningIntervalTime,
		true);
}

void AEnemyTargetPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyTargetPoint::GenerateEnemyAI(UWorld* World, TSubclassOf<APawn> PawnClass, const FVector& Location, const FRotator& Rotation)
{
	if (EnemyCounter++ < MaxEnemyCount)
	{
		if (!World || !PawnClass)
		{
			return;
		}

		// パラメータを設定
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// AIキャラクターをスポーン
		TObjectPtr<APawn> SpawnedPawn = World->SpawnActor<APawn>(PawnClass, Location, Rotation, SpawnParams);
		if (SpawnedPawn)
		{
			// AIコントローラを取得
			TObjectPtr<AAIEnemyController> AIController = Cast<AAIEnemyController>(SpawnedPawn->GetController());
			if (!AIController)
			{
				// AIコントローラがない場合は、新しく作成
				FString path = "/Game/GUNMAN/Blueprint/Enemy/BP_AIEnemyController.BP_AIEnemyController_C";
				TSubclassOf<class AAIEnemyController> AIControllerClass = TSoftClassPtr<AAIEnemyController>(FSoftObjectPath(*path)).LoadSynchronous();
				AIController = World->SpawnActor<AAIEnemyController>(AIControllerClass, Location, Rotation, SpawnParams);
				if (AIController)
				{
					AIController->Possess(SpawnedPawn);
				}
			}
		}
	}
}
