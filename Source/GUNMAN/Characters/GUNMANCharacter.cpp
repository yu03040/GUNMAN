// Copyright Epic Games, Inc. All Rights Reserved.

#include "GUNMANCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Math/Vector.h"
#include "Engine/DataTable.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerInput.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "GUNMAN/SpawnActors/FirstPersonProjectile.h"
#include "GUNMAN/ArmedWeapon/Weapon.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GUNMAN/UMG/UICharacter.h"
#include "GUNMAN/LevelScript/BattleMapScript.h"

/**
 * @brief コンストラクタ。コンポーネントの生成と初期値の設定を行う。
 *
 * カプセルサイズ・カメラ・メッシュ・武器・AI感知刺激源を作成し、
 * Enhanced Input アセットとタイムラインのロードを実行する。
 */
AGUNMANCharacter::AGUNMANCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// コントローラーの回転をキャラクターに伝播させない（カメラのみに影響）
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 入力方向にキャラクターが向くように設定
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// カメラブームの作成（キャラクター背後にカメラを配置するスプリングアーム）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->ProbeSize = 10.0f;
	CameraBoom->ProbeChannel = ECollisionChannel::ECC_Camera;

	// TPS カメラの作成
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	// FPS カメラの作成
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// FPS 用の腕メッシュの作成（オーナーにのみ表示）
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// FPS 用の銃メッシュの作成
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	// 銃口位置コンポーネントの作成（投射物のスポーン基点）
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// TPS 用の武器メッシュの作成
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh());

	// AI がプレイヤーを視覚で感知できるよう刺激源コンポーネントを作成
	StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->RegisterWithPerceptionSystem();

	LoadEnhancedInput();
	CreateTimeline();
}

/**
 * @brief ゲーム開始時の初期化処理。
 *
 * Enhanced Input の MappingContext 登録、カーソル非表示、
 * アニメーションインスタンスの取得、UI の生成を行う。
 */
void AGUNMANCharacter::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		// EnhancedInput サブシステムにデフォルトの MappingContext を登録
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());

		DisplayCharacterUI(PlayerController);
		DisplayGunSight(PlayerController);
	}

	// TPS アニメーションインスタンスを取得
	TPMeshAnimInstance = GetMesh()->GetAnimInstance();

	// 初期状態は TPS モード
	ToggleFlipflop(true, false);

	OnTakeAnyDamage.AddDynamic(this, &AGUNMANCharacter::HandleAnyDamage);
}

/**
 * @brief Construction Script。FPS 用の銃を腕メッシュの GripPoint ソケットにアタッチする。
 *
 * @param Transform このアクターのワールドトランスフォーム
 */
void AGUNMANCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FP_Gun->AttachToComponent(
		Mesh1P,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			false
		),
		FName(TEXT("GripPoint"))
	);
}

/**
 * @brief 毎フレームの更新処理。タイムラインのティックとカメラオフセットの更新を行う。
 *
 * @param DeltaTime 前フレームからの経過時間（秒）
 */
void AGUNMANCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RunTimeline.IsPlaying())
	{
		RunTimeline.TickTimeline(DeltaTime);
	}

	ChangeCameraOffset(DeltaTime);
}

/**
 * @brief TPS 時にカメラブームのオフセットを状況に応じて補間する。
 *
 * 壁衝突・エイム中・通常の 3 状態を判定し、TargetArmLength・SocketOffset・
 * TargetOffset をそれぞれの目標値に向けて線形補間する。
 * FPS モード中は何もしない。
 *
 * @param DeltaTime 前フレームからの経過時間（秒）
 */
void AGUNMANCharacter::ChangeCameraOffset(float& DeltaTime)
{
	if (bIsFP)
	{
		return;
	}

	// キャラクター前方への短いトレースで壁への接近を検出
	FHitResult HitResult;
	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	const FVector End = Start - 80 * GetActorForwardVector();
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<TObjectPtr<AActor>>{this}, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Green, FColor::Red, 5.0f);

	if (HitResult.bBlockingHit)
	{
		// 壁衝突時：カメラを引き寄せてクリッピングを防ぐ
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 100.0f, DeltaTime);
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoom->SocketOffset, FVector(0.0f, 0.0f, 60.0f), DeltaTime);
		CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector(0.0f, 0.0f, 100.0f), DeltaTime);
	}
	else if (bIsAimingState)
	{
		// エイム中：エディタで設定した CameraBoomSocketOffset に補間
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 300.0f, DeltaTime);
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoom->SocketOffset, CameraBoomSocketOffset, DeltaTime);
		CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector::ZeroVector, DeltaTime);
	}
	else
	{
		// 通常時：デフォルト位置に戻す
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 300.0f, DeltaTime);
		CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector::ZeroVector, DeltaTime);
	}
}

/**
 * @brief コンストラクタから呼ばれる Enhanced Input アセットの一括ロード。
 *
 * ConstructorHelpers::FObjectFinder を使って MappingContext・InputAction・
 * DataTable をコンテンツブラウザのパスから取得し、各メンバーに代入する。
 */
void AGUNMANCharacter::LoadEnhancedInput()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/GUNMAN/Input/IMC_Default.IMC_Default"));
	if (MappingContextFinder.Succeeded())
	{
		DefaultMappingContext = MappingContextFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(TEXT("/Game/GUNMAN/Input/IA_Jump.IA_Jump"));
	if (JumpActionFinder.Succeeded())
	{
		JumpAction = JumpActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionFinder(TEXT("/Game/GUNMAN/Input/IA_Fire.IA_Fire"));
	if (FireActionFinder.Succeeded())
	{
		FireAction = FireActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FPFireActionFinder(TEXT("/Game/GUNMAN/Input/IA_FPFire.IA_FPFire"));
	if (FPFireActionFinder.Succeeded())
	{
		FPFireAction = FPFireActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleActionFinder(TEXT("/Game/GUNMAN/Input/IA_Toggle.IA_Toggle"));
	if (ToggleActionFinder.Succeeded())
	{
		ToggleAction = ToggleActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ReadyGunActionFinder(TEXT("/Game/GUNMAN/Input/IA_ReadyGun.IA_ReadyGun"));
	if (ReadyGunActionFinder.Succeeded())
	{
		ReadyGunAction = ReadyGunActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttachGunActionFinder(TEXT("/Game/GUNMAN/Input/IA_SwitchAndEquipWeapons.IA_SwitchAndEquipWeapons"));
	if (AttachGunActionFinder.Succeeded())
	{
		AttachGunAction = AttachGunActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionFinder(TEXT("/Game/GUNMAN/Input/IA_Run.IA_Run"));
	if (RunActionFinder.Succeeded())
	{
		RunAction = RunActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> PauseMenuActionFinder(TEXT("/Game/GUNMAN/Input/IA_PauseMenu.IA_PauseMenu"));
	if (PauseMenuActionFinder.Succeeded())
	{
		PauseMenuAction = PauseMenuActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveForwardActionFinder(TEXT("/Game/GUNMAN/Input/IA_MoveForward.IA_MoveForward"));
	if (MoveForwardActionFinder.Succeeded())
	{
		MoveForwardAction = MoveForwardActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveRightActionFinder(TEXT("/Game/GUNMAN/Input/IA_MoveRight.IA_MoveRight"));
	if (MoveRightActionFinder.Succeeded())
	{
		MoveRightAction = MoveRightActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(TEXT("/Game/GUNMAN/Input/IA_Look.IA_Look"));
	if (LookActionFinder.Succeeded())
	{
		LookAction = LookActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/GUNMAN/Blueprint/ArmedWeapon/DT_Weapon.DT_Weapon"));
	if (DataTableFinder.Succeeded())
	{
		WeaponDataTable = DataTableFinder.Object;
	}
}

/**
 * @brief 入力コンポーネントへのアクションバインドを行う。
 *
 * Enhanced Input コンポーネントに各 InputAction と対応するコールバックを登録する。
 *
 * @param PlayerInputComponent バインド先の入力コンポーネント
 */
void AGUNMANCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGUNMANCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopJump);

		// TPS 射撃
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopFire);

		// FPS 射撃
		EnhancedInputComponent->BindAction(FPFireAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartFPFire);
		EnhancedInputComponent->BindAction(FPFireAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopFPFire);

		// TPS / FPS 視点切り替え
		EnhancedInputComponent->BindAction(ToggleAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::ToggleBetweenTPSAndFPS);

		// エイム
		EnhancedInputComponent->BindAction(ReadyGunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartReadyGun);
		EnhancedInputComponent->BindAction(ReadyGunAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopReadyGun);

		// 武器の装備 / 解除
		EnhancedInputComponent->BindAction(AttachGunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::AttachingAndRemovingGun);

		// ダッシュ（タイムラインで速度補間）
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartTimeline);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::ReverseTimeline);

		// ポーズメニュー
		EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::PressedActionPauseMenu);

		// 移動
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::MoveRight);

		// 視点操作
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::Look);
	}
}

/**
 * @brief TPS 連射タイマーを開始する。FiringInterval 秒ごとに FiringEvent を呼び出す。
 */
void AGUNMANCharacter::StartFire()
{
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AGUNMANCharacter::FiringEvent, FiringInterval, true);
}

/**
 * @brief TPS 連射タイマーを停止し、タイマーハンドルを無効化する。
 */
void AGUNMANCharacter::StopFire()
{
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	if (FireTimerManager.IsTimerActive(FireTimerHandle))
	{
		FireTimerManager.ClearTimer(FireTimerHandle);
		FireTimerHandle.Invalidate();
	}
}

/**
 * @brief TPS 連射タイマーから定期的に呼ばれる発射判定。
 *
 * 武器を装備・エイム中・攻撃可能・TPS モードの条件をすべて満たす場合に OnFire() を実行する。
 */
void AGUNMANCharacter::FiringEvent()
{
	if (bHasArms && bIsAimingState && bCanAttack && !bIsFP)
	{
		OnFire();
	}
}

/**
 * @brief FPS 連射タイマーを開始する。FiringInterval 秒ごとに FPFiringEvent を呼び出す。
 */
void AGUNMANCharacter::StartFPFire()
{
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AGUNMANCharacter::FPFiringEvent, FiringInterval, true);
}

/**
 * @brief FPS 連射タイマーを停止し、タイマーハンドルを無効化する。
 */
void AGUNMANCharacter::StopFPFire()
{
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	if (FireTimerManager.IsTimerActive(FireTimerHandle))
	{
		FireTimerManager.ClearTimer(FireTimerHandle);
		FireTimerHandle.Invalidate();
	}
}

/**
 * @brief FPS 連射タイマーから定期的に呼ばれる発射判定。
 *
 * FPS モード中のみ OnFPFire() を実行する。
 */
void AGUNMANCharacter::FPFiringEvent()
{
	if (bIsFP)
	{
		OnFPFire();
	}
}

/**
 * @brief TPS カメラ前方へライントレースを行い、最初のヒット情報を返す。
 *
 * OnFire・OnFPFire の両方で共通して使われる。自分自身はトレース対象から除外する。
 *
 * @param OutHit ヒット結果の出力先
 * @return ヒットした場合は true、何もヒットしなかった場合は false
 */
bool AGUNMANCharacter::PerformEnemyLineTrace(FHitResult& OutHit) const
{
	const FVector Start = ThirdPersonCamera->GetComponentLocation();
	const FVector End = Start + ThirdPersonCamera->GetForwardVector() * 10000.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
}

/**
 * @brief TPS モードの発射処理。ライントレースで敵に命中した場合にダメージを与える。
 *
 * 現在装備中の武器（WeaponNumber）に応じた攻撃力を選択する。
 * 撃破した場合は KillCount をインクリメントする。
 */
void AGUNMANCharacter::OnFire()
{
	AnimationAtFiring();

	FHitResult HitResult;
	if (!PerformEnemyLineTrace(HitResult))
	{
		return;
	}

	TObjectPtr<AAIEnemy> Enemy = Cast<AAIEnemy>(HitResult.GetActor());
	if (!Enemy || Enemy->GetCurrentHealth() <= 0.0f)
	{
		return;
	}

	// 着弾エフェクトをスポーン
	UGameplayStatics::SpawnEmitterAtLocation(this, WeaponEmitter, HitResult.Location, FRotator::ZeroRotator, true, EPSCPoolMethod::None, true);

	// WeaponNumber に対応する攻撃力を選択（0: ライフル, 1: ショットガン, その他: ピストル）
	float ATK;
	if (WeaponNumber == 0)
	{
		ATK = TP_RifleATK;
	}
	else if (WeaponNumber == 1)
	{
		ATK = TP_ShotgunATK;
	}
	else
	{
		ATK = TP_PistolATK;
	}

	UGameplayStatics::ApplyDamage(Enemy, ATK, nullptr, nullptr, nullptr);

	if (Enemy->GetCurrentHealth() <= 0.0f)
	{
		KillCount++;
	}
}

/**
 * @brief FPS モードの発射処理。ライントレースで敵に命中した場合にダメージを与える。
 *
 * FP_WeaponATK を固定で使用する。撃破した場合は KillCount をインクリメントする。
 */
void AGUNMANCharacter::OnFPFire()
{
	AnimationAtFiring();

	FHitResult HitResult;
	if (!PerformEnemyLineTrace(HitResult))
	{
		return;
	}

	TObjectPtr<AAIEnemy> Enemy = Cast<AAIEnemy>(HitResult.GetActor());
	if (!Enemy || Enemy->GetCurrentHealth() <= 0.0f)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(Enemy, FP_WeaponATK, nullptr, nullptr, nullptr);

	if (Enemy->GetCurrentHealth() <= 0.0f)
	{
		KillCount++;
	}
}

/**
 * @brief 発射時のエフェクト・アニメーション・投射物スポーンを処理する。
 *
 * FPS モードでは発射音・腕アニメーション・投射物スポーンを行う。
 * TPS モードでは発射音・マズルフラッシュ・身体アニメーション・薬莢スポーンを行う。
 */
void AGUNMANCharacter::AnimationAtFiring()
{
	if (bIsFP)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);

		// 腕アニメーションを再生
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireAnimation);
		}

		// 銃口から投射物をスポーン
		const FRotator SpawnRotation = GetControlRotation();
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		GetWorld()->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
	else
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeaponInfo.GunshotSound);
		UGameplayStatics::SpawnEmitterAttached(EquippedWeaponInfo.MuzzleFire, EquippedWeapon, EquippedWeaponInfo.MuzzleFireSoketName);
		PlayAnimMontage(EquippedWeaponInfo.FiringMontage, 1.0f);

		// 薬莢をスポーン
		GetWorld()->SpawnActor<AWeaponAmmunition>(
			EquippedWeaponInfo.AmmunitionClass,
			FTransform(EquippedWeapon->GetSocketTransform(TEXT("None")))
		);
	}
}

/**
 * @brief TPS と FPS を交互に切り替える。
 *
 * bIsFlipped フラグを反転させながら ToggleFlipflop を呼び出し、
 * 照準 UI の表示・非表示も合わせて制御する。
 */
void AGUNMANCharacter::ToggleBetweenTPSAndFPS()
{
	if (bIsFlipped)
	{
		UIGunSightRef->RemoveFromParent();
		ToggleFlipflop(false, true);
	}
	else
	{
		if (bHasArms && bIsAimingState)
		{
			UIGunSightRef->AddToViewport();
		}
		ToggleFlipflop(true, false);
	}
	bIsFlipped = !bIsFlipped;
}

/**
 * @brief TPS / FPS に応じてカメラ・メッシュ・武器の表示状態を切り替える。
 *
 * シーン内のすべての AWeapon アクターを検索し、TPS 時のみ表示する。
 *
 * @param bIsTPActive TPS カメラと TPS メッシュをアクティブにするか
 * @param bIsFPActive FPS カメラと FPS メッシュをアクティブにするか
 */
void AGUNMANCharacter::ToggleFlipflop(bool bIsTPActive, bool bIsFPActive)
{
	ThirdPersonCamera->SetActive(bIsTPActive);
	FirstPersonCamera->SetActive(bIsFPActive);

	// FPS 時はコントローラーの Yaw 回転でキャラクターを向かせる
	bUseControllerRotationYaw = bIsFPActive;

	GetMesh()->SetVisibility(bIsTPActive);

	// TPS / FPS に関わらず、TPS 用のデフォルト武器スロットは常に非表示
	Weapon->SetVisibility(false);

	// シーン内の装備可能武器アクターの表示切替
	TArray<TObjectPtr<AActor>> WeaponList;
	UGameplayStatics::GetAllActorsOfClass(this, AWeapon::StaticClass(), WeaponList);
	for (TObjectPtr<AActor> SearchActor : WeaponList)
	{
		TObjectPtr<AWeapon> NewActor = Cast<AWeapon>(SearchActor);
		NewActor->SetActorHiddenInGame(!bIsTPActive);
	}

	Mesh1P->SetVisibility(bIsFPActive);
	FP_Gun->SetVisibility(bIsFPActive);

	bIsFP = bIsFPActive;
}

/**
 * @brief エイムを開始する。武器装備中かつ TPS モードの場合のみ有効。
 *
 * カメラオフセットをエイム用に設定し、照準 UI を表示する。
 */
void AGUNMANCharacter::StartReadyGun()
{
	if (bHasArms && !bIsFP)
	{
		CameraBoom->SocketOffset = CameraBoomSocketOffset;
		GunPreparationProcess(true, false, true);
		if (UIGunSightRef)
		{
			UIGunSightRef->AddToViewport();
		}
	}
}

/**
 * @brief エイムを終了する。武器装備中かつ TPS モードの場合のみ有効。
 *
 * カメラオフセットをデフォルトに戻し、照準 UI を非表示にする。
 */
void AGUNMANCharacter::StopReadyGun()
{
	if (bHasArms && !bIsFP)
	{
		CameraBoom->SocketOffset = FVector::ZeroVector;
		GunPreparationProcess(false, true, false);
		if (UIGunSightRef)
		{
			UIGunSightRef->RemoveFromParent();
		}
	}
}

/**
 * @brief エイム状態の各種フラグとアニメーション状態を更新する内部処理。
 *
 * @param bIsAiming        エイム中にするか
 * @param bOrientRotationToMovement 移動方向にキャラクターを向けるか
 * @param bYawRotation     コントローラーの Yaw 回転をキャラクターに反映するか
 */
void AGUNMANCharacter::GunPreparationProcess(bool bIsAiming, bool bOrientRotationToMovement, bool bYawRotation)
{
	bIsAimingState = bIsAiming;

	// TPS アニメーションインスタンスにエイム状態を通知
	IAnimationInterface* AnimInterface = Cast<IAnimationInterface>(TPMeshAnimInstance);
	if (AnimInterface)
	{
		AnimInterface->Execute_AimingState(TPMeshAnimInstance, bIsAimingState);
	}

	GetCharacterMovement()->bOrientRotationToMovement = bOrientRotationToMovement;
	bUseControllerRotationYaw = bYawRotation;
}

/**
 * @brief 武器の装備・解除をトグルする。FPS モード中は無効。
 *
 * 未装備の場合は WeaponMeshes[WeaponNumber] を装備し、DataTable から武器情報を取得する。
 * 装備済みの場合は現在の武器を解除してデフォルトソケットに戻す。
 */
void AGUNMANCharacter::AttachingAndRemovingGun()
{
	if (bIsFP)
	{
		return;
	}

	WeaponNumber = WeaponNumberCounter;

	if (!bHasArms)
	{
		// WeaponNumber に対応する武器を表示・装備
		EquippedWeapon = WeaponMeshes[WeaponNumber];
		EquippedWeapon->SetHiddenInGame(false, false);

		// ComponentTag をキーに DataTable から武器情報を取得
		const FName RowName = EquippedWeapon->ComponentTags[0];
		FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
		if (Row)
		{
			EquippedWeaponInfo.GunshotSound       = Row->GunshotSound;
			EquippedWeaponInfo.MuzzleFire          = Row->MuzzleFire;
			EquippedWeaponInfo.MuzzleFireSoketName = Row->MuzzleFireSoketName;
			EquippedWeaponInfo.FiringMontage       = Row->FiringMontage;
			EquippedWeaponInfo.AmmunitionClass     = Row->AmmunitionClass;
			EquippedWeaponInfo.AmmunitionSocketName = Row->AmmunitionSocketName;

			EquipWeapon(true, Row->HasPistol, Row->EquipSocketName);
			UGameplayStatics::PlaySound2D(this, Row->EquipmentNoise);
		}

		CountWeapon(WeaponMeshes, WeaponNumber);
	}
	else
	{
		// 現在装備中の武器を DataTable から参照して解除ソケットに戻す
		const FName RowName = RemoveWeapon(WeaponMeshes, WeaponNumber)->ComponentTags[0];
		FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
		if (Row)
		{
			EquipWeapon(false, false, Row->AttachSocketName);
		}
	}
}

/**
 * @brief 武器メッシュを指定ソケットにアタッチし、アニメーションの装備状態を更新する。
 *
 * @param bHasWeapon 武器を装備するか（false で解除）
 * @param bHasPistol ピストル系の武器か（アニメーション分岐に使用）
 * @param SoketName  アタッチ先のソケット名
 */
void AGUNMANCharacter::EquipWeapon(bool bHasWeapon, bool bHasPistol, FName SoketName)
{
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SoketName);

	bHasArms = bHasWeapon;

	// アニメーションインスタンスに装備状態を通知
	IAnimationInterface* AnimInterface = Cast<IAnimationInterface>(TPMeshAnimInstance);
	if (AnimInterface)
	{
		AnimInterface->Execute_EquippedState(TPMeshAnimInstance, bHasWeapon, bHasPistol);
	}
}

/**
 * @brief 次に装備する武器のインデックスを WeaponNumberCounter に記録する。
 *
 * 配列末尾を超えた場合は 0 に折り返す。
 *
 * @param Arms   武器メッシュの配列
 * @param Number 現在装備中の武器インデックス
 */
void AGUNMANCharacter::CountWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number)
{
	WeaponNumberCounter = (Number + 1 < Arms.Num()) ? (Number + 1) : 0;
}

/**
 * @brief 現在装備中の武器メッシュを返す。
 *
 * WeaponNumberCounter はすでに次のインデックスを指しているため、
 * Number - 1 が実際に装備している武器に対応する。
 * Number が 0 の場合は配列末尾を返す（折り返し）。
 *
 * @param Arms   武器メッシュの配列
 * @param Number WeaponNumberCounter（次の武器インデックス）
 * @return       現在装備中の武器メッシュ
 */
USkeletalMeshComponent* AGUNMANCharacter::RemoveWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number)
{
	const int PickIndex = (Number - 1 < 0) ? Arms.Num() - 1 : Number - 1;
	return Arms[PickIndex];
}

/**
 * @brief ポーズメニューを開く。
 *
 * BattleMapScript を取得し、MappingContext をポーズメニュー用に切り替えた上で
 * ボタン位置を初期化する。バトルマップ以外では何もしない。
 */
void AGUNMANCharacter::PressedActionPauseMenu()
{
	TObjectPtr<ABattleMapScript> BattleMapRef = Cast<ABattleMapScript>(GetWorld()->GetLevelScriptActor());
	if (BattleMapRef)
	{
		// MappingContext を切り替える前に、進行中の連続入力アクションを明示的に停止する。
		// MappingContext を削除すると入力の Completed イベントが発火しないため、
		// タイマーや状態フラグが残ったままポーズ復帰後に動作し続けるのを防ぐ。
		StopFire();        // TPS 連射タイマーをクリア
		StopFPFire();      // FPS 連射タイマーをクリア
		StopReadyGun();    // エイム状態を解除（照準 UI も非表示にする）
		ReverseTimeline(); // ダッシュ速度を通常速度へ戻す

		TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// デフォルト MappingContext をポーズメニュー用に切り替え
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(BattleMapRef->PauseMenuMappingContext, 0);
		}

		BattleMapRef->InitializeButtonPosition();
	}
}

/**
 * @brief ジャンプを開始する。JumpZVelocity を 400 に設定してから Jump() を呼ぶ。
 */
void AGUNMANCharacter::StartJump()
{
	Jump();
	JumpButtonDown = CanJump();
	GetCharacterMovement()->JumpZVelocity = 400.0f;
}

/**
 * @brief ジャンプを終了する。StopJumping() を呼び JumpButtonDown をリセットする。
 */
void AGUNMANCharacter::StopJump()
{
	StopJumping();
	JumpButtonDown = false;
	GetCharacterMovement()->JumpZVelocity = 400.0f;
}

/**
 * @brief 前後方向への移動入力を処理する。
 *
 * コントローラーの Yaw 回転から前方ベクトルを算出し、AddMovementInput に渡す。
 *
 * @param Value 入力値（正で前進、負で後退）
 */
void AGUNMANCharacter::MoveForward(const FInputActionValue& Value)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value.Get<float>());
	}
}

/**
 * @brief 左右方向への移動入力を処理する。
 *
 * コントローラーの Yaw 回転から右方向ベクトルを算出し、AddMovementInput に渡す。
 *
 * @param Value 入力値（正で右、負で左）
 */
void AGUNMANCharacter::MoveRight(const FInputActionValue& Value)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value.Get<float>());
	}
}

/**
 * @brief 視点移動の入力を処理する。
 *
 * X 軸を Yaw（水平）、Y 軸を Pitch（垂直）に割り当てる。
 *
 * @param Value 2D 入力値（X: 水平回転量、Y: 垂直回転量）
 */
void AGUNMANCharacter::Look(const FInputActionValue& Value)
{
	if (Controller)
	{
		const FVector2D LookAxisVector = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

/**
 * @brief OnTakeAnyDamage イベントのコールバック。体力を減らしゲームオーバーを判定する。
 *
 * @param DamagedActor  ダメージを受けたアクター（自身）
 * @param Damage        受けたダメージ量
 * @param DamageType    ダメージの種類
 * @param InstigatedBy  ダメージを与えたコントローラー
 * @param DamageCauser  ダメージを発生させたアクター
 */
void AGUNMANCharacter::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	if (CurrentHealth <= DeadHealth)
	{
		UGameplayStatics::OpenLevel(this, "GameOverMap");
	}
}

/**
 * @brief 走り速度補間用のタイムラインを初期化する。
 *
 * CB_Run カーブを読み込み、長さ 1 秒のタイムラインに登録する。
 * Timeline_LinearInterpCharacterSpeed が毎フレームのコールバックとして設定される。
 */
void AGUNMANCharacter::CreateTimeline()
{
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Find(TEXT("CurveFloat'/Game/GUNMAN/Blueprint/Characters/CB_Run.CB_Run'"));
	if (Find.Succeeded())
	{
		RunCurve = Find.Object;
	}

	RunTimeline.SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	RunTimeline.SetTimelineLength(1.0f);

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, TEXT("Timeline_LinearInterpCharacterSpeed"));
	RunTimeline.AddInterpFloat(RunCurve, TimelineProgress);
}

/**
 * @brief タイムラインのフレームコールバック。カーブ値を MaxWalkSpeed に反映する。
 *
 * @param Value タイムラインが現在のフレームで返すカーブの値（移動速度）
 */
void AGUNMANCharacter::Timeline_LinearInterpCharacterSpeed(float Value)
{
	GetCharacterMovement()->MaxWalkSpeed = Value;
}

/**
 * @brief 走りタイムラインを先頭から再生（加速）する。
 */
void AGUNMANCharacter::StartTimeline()
{
	RunTimeline.PlayFromStart();
}

/**
 * @brief 走りタイムラインを末尾から逆再生（減速）する。
 */
void AGUNMANCharacter::ReverseTimeline()
{
	RunTimeline.ReverseFromEnd();
}

/**
 * @brief 照準 UI（WBP_UIGunSight）を生成する。ビューポートへの追加は行わない。
 *
 * @param PlayerController ウィジェットを所有するプレイヤーコントローラー
 */
void AGUNMANCharacter::DisplayGunSight(TObjectPtr<APlayerController>& PlayerController)
{
	const FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_UIGunSight.WBP_UIGunSight_C";
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	if (WidgetClass && PlayerController)
	{
		UIGunSightRef = Cast<UUIGunSight>(CreateWidget(PlayerController, WidgetClass));
	}
}

/**
 * @brief キャラクター UI（WBP_UICharacter）を生成してビューポートに追加する。
 *
 * @param PlayerController ウィジェットを所有するプレイヤーコントローラー
 */
void AGUNMANCharacter::DisplayCharacterUI(TObjectPtr<APlayerController>& PlayerController)
{
	const FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_UICharacter.WBP_UICharacter_C";
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	if (WidgetClass && PlayerController)
	{
		TObjectPtr<UUICharacter> UICharacterRef = Cast<UUICharacter>(CreateWidget(PlayerController, WidgetClass));
		if (UICharacterRef)
		{
			UICharacterRef->AddToViewport();
		}
	}
}

/**
 * @brief IAnimationInterface::FireState の実装。アニメーション通知から攻撃可否を受け取る。
 *
 * @param bCanATK true なら攻撃可能、false なら不可
 */
void AGUNMANCharacter::FireState_Implementation(bool bCanATK)
{
	bCanAttack = bCanATK;
}

/**
 * @brief IWeaponInterface::AttachWeapon の実装。武器メッシュを配列に登録してアタッチする。
 *
 * Weapon アクターの BeginPlay から呼ばれ、WeaponMeshes に武器を追加する。
 *
 * @param WeaponMesh     アタッチする武器のスケルタルメッシュコンポーネント
 * @param AttachSoketName アタッチ先のソケット名
 */
void AGUNMANCharacter::AttachWeapon_Implementation(USkeletalMeshComponent* WeaponMesh, FName AttachSoketName)
{
	WeaponMeshes.Add(WeaponMesh);
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), AttachSoketName);
}
