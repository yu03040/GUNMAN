// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAIControllerInterface.h"
#include "AIEnemyController.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API AAIEnemyController : public AAIController, public IEnemyAIControllerInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess="true"))
	TObjectPtr<UBehaviorTree> BTAsset;

	/** 受け取ったアクターを格納する変数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> ReceivedActor;

	/* タイマーハンドル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle_TargetLost;

public:
	/** 敵が意思決定をするめための変数 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr <UAISenseConfig_Sight> SightConfig;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AAIEnemyController();

	/** 受け取ったアクターの情報を更新する関数 */
	void UpdateTargetActorKey_Implementation(AActor* TargetActor) override;

	void UpdateHasLineOfSightKey_Implementation(bool HasLineOfSight) override;	

	/**
	* OnTargetPerceptionUpdated イベントに関連付ける関数
	* @param Actor 視界に入ったアクター
	* @param Stimulus 更新された刺激
	*/
	UFUNCTION()
	void PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Set Timer by Event のイベント */
	UFUNCTION(BlueprintCallable, Category = Enemy)
	void TargetLost();

	TObjectPtr<UBehaviorTree> GetBTAsset() { return BTAsset; }
};
