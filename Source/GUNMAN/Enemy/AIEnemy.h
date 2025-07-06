// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "GUNMAN/Enemy/AIEnemyInterface.h"
#include "GUNMAN/Enemy/EnemyAIControllerInterface.h"
#include "GUNMAN/UMG/UIEnemy.h"
#include "GUNMAN/ArmedWeapon/WeaponStructure.h"
#include "AIEnemy.generated.h"

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class GUNMAN_API AAIEnemy : public ACharacter, public IAIEnemyInterface, public IEnemyAIControllerInterface
{
	GENERATED_BODY()

private:
	/** ThirdPerson の武器 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Weapon;

	/** Healthバーを表示するための変数 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> Widget;

	/** プレイヤーかどうか判断する変数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> Player;

	/** Enemy のコントローラーを判断する変数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AController> EnemyController;

	/** 受け取ったアクターを格納する変数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> ReceivedActor;

	/** 攻撃が当たったときのアニメーション */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HitAnimMontage;

	/** Healthバーを表示するための変数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UUIEnemy> EnemyWidget;

	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UDataTable> WeaponDataTable;

	FWeaponStructure EquippedWeaponInformation;

	/* カーブ */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> FloatCurve;

	/* タイムライン */
	FTimeline* RunTimeline;

	float EnemySpeed;

protected:
	/* 敵の生死 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	bool bIsAlive;

	/* タイマーハンドル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	FTimerHandle TimerHandle_TargetLost;

	/* 現在の敵の体力 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	float CurrentHealth;

	/* 敵の体力 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	float MaxHealth;

	/* 敵が死んだときの体力 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	float DeadHealth;

	/* 敵の攻撃力 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	float EnemyATK;

	/* 敵が死んで倒れるまでの時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemy)
	float InLifeSpan;

public:
	int index = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* 被ダメージ処理関数 */
	UFUNCTION()
	void HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

public:
	// Sets default values for this character's properties
	AAIEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** キャラクターを攻撃する関数 */
	void AttackCharacter_Implementation() override;

	void CreateAmmunition();

	/** キャラクターを追いかけるスピードを変更する関数 */
	void ChangeRunningSpeed_Implementation(float Speed) override;

	UFUNCTION()
	void TimeLineFunc(float Value);

	bool GetIsAlive() const { return bIsAlive; }

	/* 現在の敵の体力のgetter */
	UFUNCTION(BlueprintCallable, Category = Enemy)
	float GetCurrentHealth() const;

	/* 敵の体力の割合を返す関数 */
	UFUNCTION(BlueprintCallable, Category = Enemy)
	float GetHealthPercent() const;
};
