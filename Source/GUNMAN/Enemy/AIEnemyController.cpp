// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemyController.h"
#include "AIEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GamePlayStatics.h"



AAIEnemyController::AAIEnemyController()
{
    // AI Perception の作成
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

    // 視覚の設定
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 500.0f;
    SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(*SightConfig->GetSenseImplementation());

    // AI の視界入ったとき PerceptionUpdated に関連づける
    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AAIEnemyController::PerceptionUpdated);

    // BehaviorTree のロード
    ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetFinder(TEXT("/Game/GUNMAN/AI/BT_PatrolAI.BT_PatrolAI"));
    if (BTAssetFinder.Succeeded())
    {
        BTAsset = BTAssetFinder.Object;
    }
}

void AAIEnemyController::UpdateTargetActorKey_Implementation(AActor* TargetActor)
{
	// BlackboardComponentの取得
    TObjectPtr<UBlackboardComponent> BlackboardComp = GetBlackboardComponent();

    if (BlackboardComp)
    {
        // オブジェクトをセットするためのキー（キーID）を取得
        FName ObjectKey = FName("TargetActor");

        // SetValueAsObjectを使用してオブジェクトをセット
        BlackboardComp->SetValueAsObject(ObjectKey, TargetActor);
    }
}

void AAIEnemyController::UpdateHasLineOfSightKey_Implementation(bool HasLineOfSight)
{
    // BlackboardComponentの取得
    TObjectPtr<UBlackboardComponent> BlackboardComp = GetBlackboardComponent();

    if (BlackboardComp)
    {
        // オブジェクトをセットするためのキー（キーID）を取得
        FName ObjectKey = FName("HasLIneOfSight");

        // SetValueAsObjectを使用してオブジェクトをセット
        BlackboardComp->SetValueAsBool(ObjectKey, HasLineOfSight);
    }
}

void AAIEnemyController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ReceivedActor = Actor;
    TObjectPtr<ACharacter> Player = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    TObjectPtr<AAIEnemy> EnemyActor = Cast<AAIEnemy>(GetPawn());


	// 視界にいるのがプレイヤーか？
	if (ReceivedActor == Player)
	{
		if (Stimulus.WasSuccessfullySensed() && EnemyActor->GetIsAlive())
		{
			// 見つけたものをセット
			ReceivedActor = Player;

			// 時間をリセット
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_TargetLost);
			if (this->Implements<UEnemyAIControllerInterface>())
			{
				// ブラックボードにアクターの情報をセット
				IEnemyAIControllerInterface::Execute_UpdateTargetActorKey(this, Player);

				// 視界に入ったフラグをセット
				IEnemyAIControllerInterface::Execute_UpdateHasLineOfSightKey(this, true);
			}
		}
		else
		{
			// 視界から外れたとき、元々見ていたものがプレイヤーか？
			if (Player == ReceivedActor)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_TargetLost, this, &AAIEnemyController::TargetLost, 5.0f, false);
                UpdateHasLineOfSightKey_Implementation(false);
			}
		}
	}
}

void AAIEnemyController::TargetLost()
{
    UpdateTargetActorKey_Implementation(nullptr);
}

void AAIEnemyController::BeginPlay()
{
    Super::BeginPlay();

    RunBehaviorTree(BTAsset);
}
