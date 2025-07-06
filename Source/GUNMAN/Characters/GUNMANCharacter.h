// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GUNMAN/ArmedWeapon/WeaponStructure.h"
#include "GUNMAN/UMG/UIGunSight.h"
#include "GUNMAN/Animations/AnimationInterface.h"
#include "GUNMAN/ArmedWeapon/WeaponInterface.h"
#include "GUNMANCharacter.generated.h"


UCLASS(config = Game)
class AGUNMANCharacter : public ACharacter, public IAnimationInterface, public IWeaponInterface
{
	GENERATED_BODY()

private:
	/* カメラをキャラクターの背後に配置するカメラブーム */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/* 左右視点移動の割合（単位：deg/sec）、その他のスケーリングが最終的なレートに影響する場合もある。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/* 上下視点移動の割合（単位：deg/sec）、その他のスケーリングが最終的なレートに影響する場合もある。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/* ポーンメッシュ：1人称視点（腕） */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/* 銃メッシュ：1人称視点（自分しか見えない） */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FP_Gun;

	/* 銃のメッシュ上で弾がスポーンする場所 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> FP_MuzzleLocation;

	/* FirstPerson の武器の攻撃力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	float FP_WeaponATK = 5.0f;

	/* FirstPerson の武器がスポーンする銃弾 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AFirstPersonProjectile> ProjectileClass;

	/* 銃口とキャラクターの位置のオフセット */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	FVector GunOffset;

	/* 発射アニメーション */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FirstPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> FireAnimation;

	/* ThirdPerson の武器 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Weapon;

	/* 玉が着弾したときのエフェクト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> WeaponEmitter;

	/* ThirdPerson の武器の配列 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<USkeletalMeshComponent>> WeaponMeshes;

	/* ThirdPerson の武器の index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	int WeaponNumber;

	/* ThirdPerson の武器の攻撃力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	float TP_WeaponATK;

	/* ThirdPerson のライフルの攻撃力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	float TP_RifleATK = 5.0f;

	/* ThirdPerson のショットガンの攻撃力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	float TP_ShotgunATK = 15.0f;

	/* ThirdPerson のピストルの攻撃力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	float TP_PistolATK = 10.0f;

	/* ThirdPerson の武器のカウント */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	int WeaponNumberCounter;

	/* 装備している武器のメッシュ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> EquippedWeapon;

	/* ThirdPerson のアニメーションインスタンスをセットする */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimInstance> TPMeshAnimInstance;

	/* 装備した武器の情報 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	FWeaponStructure EquippedWeaponInfo;

	/* 武器を構えているか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ThirdPerson, meta = (AllowPrivateAccess = "true"))
	bool bIsAimingState;

	/* 発射サウンドエフェクト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> FireSound;

	/* フリップフロップのための変数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	bool bIsFlipped = true;

	/* 武器を持っているか？ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	bool bHasArms;

	/* FirstPerson か？ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	bool bIsFP = false;

	/* 攻撃できるか？ */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack = true;

	/* 攻撃のタイマーハンドル */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	FTimerHandle FireTimerHandle;

	/* 連射間隔 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FPAndTP, meta = (AllowPrivateAccess = "true"))
	float FiringInterval = 0.1f;

	/* ジャンプボタンが押されたかどうか */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	bool JumpButtonDown = false;

	/* プレイヤーの体力 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 1000.0f;

	/* 現在のプレイヤ―の体力 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = MaxHealth;

	/* 死んだプレイヤ―の体力 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	float DeadHealth = 0.0f;

	/* 倒した敵の数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	int KillCount = 0;

	/* 照準の UI の参照 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UUIGunSight> UIGunSightRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	// 攻撃アクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	// FPS用攻撃アクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FPFireAction;

	// FPS, TPS切り替えアクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReadyGunAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttachGunAction;

	// ダッシュアクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;

	// ポーズメニューアクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PauseMenuAction;

	// 前後移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveForwardAction;

	// 左右移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveRightAction;

	// 視点変更アクション
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	// 武器の各種情報
	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UDataTable> WeaponDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraBoom, meta = (AllowPrivateAccess = "true"))
	FVector CameraBoomSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraBoom, meta = (AllowPrivateAccess = "true"))
	FVector CameraBoomTargetOffset;

protected:
	// プレイヤーのスピード変化させるタイムライン
	FTimeline* RunTimeline;

	// プレイヤーのスピード変化させる CurveAsset
	TObjectPtr<class UCurveFloat> RunCurve;

	// AI に感知してもらために必要な刺激
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSourceComponent;

public:
	// プレイヤーの入力を実行するために必要な各種情報
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 攻撃を開始する関数
	void StartFire();

	// TimerManager に呼ばれる関数
	void FiringEvent();

	// 攻撃を終了する関数
	void StopFire();

	void StartFPFire();

	void StopFPFire();

	void FPFiringEvent();
	/**
	* 発砲の状態を管理する関数
	* @param bCanATK 攻撃できるか？
	*/
	void FireState_Implementation(bool bCanATK) override;

	/**
	* 武器をアタッチする関数
	* @param WeaponMesh 武器のメッシュ
	* @param WeaponSoketName 武器をアタッチする場所の名前
	*/
	void AttachWeapon_Implementation(USkeletalMeshComponent* WeaponMesh, FName AttachSoketName) override;

	/* TPS と FPS を切り替える関数 */
	void ToggleBetweenTPSAndFPS();

	/**
	* ThirdPerson と FirstPerson の設定を行う関数
	* @param bIsTPActive ThirdPerson をアクティブにするか？
	* @param bIsFPActive FirstPerson をアクティブにするか？
	*/
	void ToggleFlipflop(bool bIsTPActive, bool bIsFPActive);

	/* 銃を構える関数 */
	void StartReadyGun();

	/*
	 * 銃を構える過程
	 * @param bIsAiming 銃を構えているか？
	 * @param bOrientRotationToMovement キャラクターの移動方向に応じて自動的にキャラクターの向きを調整するか？
	 * @param bYawRotation キャラクターがコントローラーの回転（Yaw）に従って回転するか？
	 */
	void GunPreparationProcess(bool bIsAiming, bool bOrientRotationToMovement, bool bYawRotation);

	// 銃を構えることを止める関数
	void StopReadyGun();

	// 銃の付け外しする関数
	void AttachingAndRemovingGun();

	// ポーズメニューを開く関数
	void PressedActionPauseMenu();

	// ジャンプを開始する関数
	void StartJump();

	// ジャンプを停止する関数
	void StopJump();

	// 前後に移動する関数
	void MoveForward(const FInputActionValue& Value);

	// 左右に移動する関数
	void MoveRight(const FInputActionValue& Value);

	// 視点移動
	void Look(const FInputActionValue& Value);

	// 弾丸を発射する
	void OnFire();

	void OnFPFire();

	// 照準を表示する関数
	void DisplayGunSight(TObjectPtr<APlayerController>& PlayerController);

	// キャラクターの UI を表示する関数 
	void DisplayCharacterUI(TObjectPtr<APlayerController>& PlayerController);

	// OnTakeAnyDamage イベントに関連付ける関数
	UFUNCTION()
	void HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// タイムラインを開始する関数
	UFUNCTION(BlueprintCallable, Category = Timeline)
	void StartTimeline();

	// タイムラインを逆再生する関数
	UFUNCTION(BlueprintCallable, Category = Timeline)
	void ReverseTimeline();

	// タイムラインでキャラクターのスピードを補間する関数
	UFUNCTION()
	void Timeline_LinearInterpCharacterSpeed(float Value);

	// 発砲時のアニメーションを出す関数
	void AnimationAtFiring();

	/*
	 * 武器を外す関数
	 * @param weapon 武器メッシュの配列
	 * @param number 現在装備している武器の index
	 * @return 外す武器のメッシュを返す
	 */
	USkeletalMeshComponent* RemoveWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number);

	/*
	 * 装備した武器を数える関数
	 * @param weapon 武器メッシュの配列
	 * @param number 現在装備している武器の index
	 */
	void CountWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number);

	/*
	 * 武器を装備する関数
	 * @param HasWeapon 武器を装備しているか？
	 * @param HasPistol ピストルを装備しているか？
	 * @param SoketName 武器をアタッチする場所の名前
	 */
	void EquipWeapon(bool bHasWeapon, bool bHasPistol, FName SoketName);

public:
	AGUNMANCharacter();

	// タイムラインを生成する関数
	void CreateTimeline();

	// EnhancedInput を読み込む関数
	void LoadEnhancedInput();

	// Construction Script
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	/*
	* ThirdPersonCamera の基準位置を変更する関数
	* @param DeltaTime フレーム間の合計時間
	*/
	void ChangeCameraOffset(float& DeltaTime);

	// KillCount の Getter
	int GetKillCount() const { return KillCount; }

	// 体力の割合を計算する関数
	float CalcHealthPercent() const { return CurrentHealth / MaxHealth; }
};