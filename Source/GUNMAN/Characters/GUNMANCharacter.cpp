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
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "GUNMAN/SpawnActors/FirstPersonProjectile.h"
#include "GUNMAN/ArmedWeapon/Weapon.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GUNMAN/UMG/UICharacter.h"
#include "GUNMAN/LevelScript/BattleMapScript.h"

AGUNMANCharacter::AGUNMANCharacter()
{
	// フレームごとに Tick() を呼び出すように、このアクタを設定する
	PrimaryActorTick.bCanEverTick = true;

	// collision capsule のサイズを設定する
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// turn rates を設定する
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// コントローラーが回転しても回転させず、カメラに影響を与えるだけにする
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// キャラクターの動きを設定する
	GetCharacterMovement()->bOrientRotationToMovement = true; // 入力された方向にキャラクターが動く...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...この回転率で
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// CameraBoom の作成
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->ProbeSize = 10.0f;
	CameraBoom->ProbeChannel = ECollisionChannel::ECC_Camera;

	// ThirdPersonCameraComponent の作成
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	// FirstPersonCameraComponent の作成
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// FirstPersonMeshComponent の作成
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// FirstPersonGunMeshComponent の作成
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	// FirstPersonMuzzleLocationComponent の作成
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	GunOffset = FVector(100.0f, 0.0f, 10.0f); // 投射物がスポーンするキャラクタの位置からのデフォルトオフセット

	// ThirdPersonGunMeshComponent の作成
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh());

	// StimuliSourceComponent を作成
	StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	// 敵AI がプレイヤーを完治できるように視覚に登録する
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->RegisterWithPerceptionSystem();

	// Enhanced Input のアセットをロード
	LoadEnhancedInput();

	// タイムラインの設定
	CreateTimeline();
}

void AGUNMANCharacter::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーコントローラーを取得
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// EnhancedInputSubsytem に MappingContext を追加
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// マウスを非表示
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());

	// ThirdPerson のアニメーションインスタンスをセット
	TPMeshAnimInstance = GetMesh()->GetAnimInstance();

	// 最初は ThirdPerson にする
	ToggleFlipflop(true, false);

	// ダメージを受けたときに に HandleAnyDamage を関連付ける
	OnTakeAnyDamage.AddDynamic(this, &AGUNMANCharacter::HandleAnyDamage);

	// ウィジェットを作成・表示する
	DisplayCharacterUI(PlayerController);
	DisplayGunSight(PlayerController);
}

void AGUNMANCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 銃を腕にアタッチする
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

void AGUNMANCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// タイムライン実行処理
	if (RunTimeline != nullptr && RunTimeline->IsPlaying())
	{
		RunTimeline->TickTimeline(DeltaTime);
	}

	ChangeCameraOffset(DeltaTime);
}

void AGUNMANCharacter::ChangeCameraOffset(float& DeltaTime)
{
	if (bIsFP == false)
	{
		FHitResult HitResult;
		FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
		FVector End = Start - 80 * GetActorForwardVector();
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<TObjectPtr<AActor>>{this}, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Green, FColor::Red, 5.0f);
		if (HitResult.bBlockingHit)
		{
			// カメラが壁に当たっている（目的の値まで線形補間）
			CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 100.0f, DeltaTime);
			CameraBoom->SocketOffset = FMath::Lerp(CameraBoom->SocketOffset, FVector(0.0f, 0.0f, 60.0f), DeltaTime);
			CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector(0.0f, 0.0f, 100.0f), DeltaTime);
		}
		else if (bIsAimingState)
		{
			// 銃を構えているとき（目的の値まで線形補間）
			CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 300.0f, DeltaTime);
			CameraBoom->SocketOffset = FMath::Lerp(CameraBoom->SocketOffset, CameraBoomSocketOffset, DeltaTime);
			CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector(0.0f, 0.0f, 0.0f), DeltaTime);
		}
		else
		{
			CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, 300.0f, DeltaTime);
			CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->SocketOffset, FVector(0.0f, 0.0f, 0.0f), DeltaTime);
			CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, FVector(0.0f, 0.0f, 0.0f), DeltaTime);
		}
	}
}

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

void AGUNMANCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ジャンプのバインド
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGUNMANCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopJump);

		// 攻撃のバインド
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopFire);
		EnhancedInputComponent->BindAction(FPFireAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartFPFire);
		EnhancedInputComponent->BindAction(FPFireAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopFPFire);

		// TPS/FPS 視点切り替えのバインド
		EnhancedInputComponent->BindAction(ToggleAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::ToggleBetweenTPSAndFPS);

		// 武器を構えるバインド
		EnhancedInputComponent->BindAction(ReadyGunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartReadyGun);
		EnhancedInputComponent->BindAction(ReadyGunAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::StopReadyGun);

		// 武器のつけ外しのバインド
		EnhancedInputComponent->BindAction(AttachGunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::AttachingAndRemovingGun);

		// 走る操作へのバインド
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::StartTimeline);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AGUNMANCharacter::ReverseTimeline);

		//// ポーズメニューのバインド
		EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::PressedActionPauseMenu);

		// 移動操作のバインド
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::MoveRight);

		// 視点移動のバインド
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGUNMANCharacter::Look);
	}
}

void AGUNMANCharacter::StartFire()
{
	// FiringInterval の間隔で連射する
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	FireTimerManager.SetTimer
	(
		FireTimerHandle,
		this,
		&AGUNMANCharacter::FiringEvent,
		FiringInterval,
		true
	);
}

void AGUNMANCharacter::StopFire()
{
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	if (FireTimerManager.IsTimerActive(FireTimerHandle))
	{
		// タイマーをクリア
		FireTimerManager.ClearTimer(FireTimerHandle);

		// タイマーハンドルを無効化
		FireTimerHandle.Invalidate();
	}
}

void AGUNMANCharacter::FiringEvent()
{
	if (bHasArms && bIsAimingState && bCanAttack && bIsFP == false)
	{
		OnFire();
	}
}

void AGUNMANCharacter::StartFPFire()
{
	// FiringInterval の間隔で連射する
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	FireTimerManager.SetTimer
	(
		FireTimerHandle,
		this,
		&AGUNMANCharacter::FPFiringEvent,
		FiringInterval,
		true
	);
}

void AGUNMANCharacter::StopFPFire()
{
	FTimerManager& FireTimerManager = GetWorldTimerManager();
	if (FireTimerManager.IsTimerActive(FireTimerHandle))
	{
		// タイマーをクリア
		FireTimerManager.ClearTimer(FireTimerHandle);

		// タイマーハンドルを無効化
		FireTimerHandle.Invalidate();
	}
}

void AGUNMANCharacter::FPFiringEvent()
{
	if (bIsFP)
	{
		OnFPFire();
	}
}

void AGUNMANCharacter::OnFire()
{
	// 発砲アニメーションを再生
	AnimationAtFiring();

	FHitResult HitResult;
	FVector StartLocation = ThirdPersonCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + ThirdPersonCamera->GetForwardVector() * 10000.0f;

	FCollisionQueryParams TraceParams; // 衝突クエリに関するパラメータを設定するための構造体
	TraceParams.AddIgnoredActor(this); // このアクターを無視する

	// ライントレースを実行
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f);
	if (bHit)
	{
		// hit したのが敵だったら
		TObjectPtr<AAIEnemy> Enemy = Cast<AAIEnemy>(HitResult.GetActor());
		if (Enemy)
		{
			if (Enemy->GetCurrentHealth() > 0.0f)
			{
				// ヒットした場所の情報を使用
				FVector HitLocation = HitResult.Location;
				UGameplayStatics::SpawnEmitterAtLocation(this, WeaponEmitter, HitLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::None, true);

				if (WeaponNumber == 0) // ライフル
				{
					TP_WeaponATK = TP_RifleATK;
				}
				else if (WeaponNumber == 1) // ショットガン
				{
					TP_WeaponATK = TP_ShotgunATK;
				}
				else //ピストル
				{
					TP_WeaponATK = TP_PistolATK;
				}
				// 敵に TP_WeaponATK だけダメージを与える
				UGameplayStatics::ApplyDamage(Enemy, TP_WeaponATK, nullptr, nullptr, nullptr);

				if (Enemy->GetCurrentHealth() <= 0.0f)
				{
					// 死んだらカウント
					KillCount++;
				}
			}
		}
	}
}

void AGUNMANCharacter::OnFPFire()
{
	// 発砲アニメーションを再生
	AnimationAtFiring();

	FHitResult HitResult;
	FVector StartLocation = ThirdPersonCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + ThirdPersonCamera->GetForwardVector() * 10000.0f;

	FCollisionQueryParams TraceParams(FName(TEXT("LineTraceByChannel")), true, this); // 衝突クエリに関するパラメータを設定するための構造体
	TraceParams.bTraceComplex = false; // 詳細な衝突判定を行うか
	TraceParams.AddIgnoredActor(this); // このアクターを無視する

	// ライントレースを実行
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);
	if (bHit)
	{
		// hit したのが敵だったら
		TObjectPtr<AAIEnemy> Enemy = Cast<AAIEnemy>(HitResult.GetActor());
		if (Enemy)
		{
			if (Enemy->GetCurrentHealth() > 0.0f)
			{
				if (bIsFP)
				{
					// 敵にダメージを与える
					UGameplayStatics::ApplyDamage(Enemy, FP_WeaponATK, nullptr, nullptr, nullptr);
				}

				if (Enemy->GetCurrentHealth() <= 0.0f)
				{
					// 死んだらカウント
					KillCount++;
				}
			}
		}
	}
}

void AGUNMANCharacter::AnimationAtFiring()
{
	if (bIsFP)
	{
		// 指定された場合、サウンドを再生
		UGameplayStatics::PlaySound2D(this, FireSound);
		// 発射するときのアニメーション
		Mesh1P->GetAnimInstance()->Montage_Play(FireAnimation);

		const FRotator SpawnRotation = GetControlRotation();
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset); // MuzzleOffset はカメラ空間にあるため、キャラクター位置からオフセットする前にワールド空間に変換し、最終的な銃口位置を求める。
		FActorSpawnParameters ActorSpawnParams; //スポーン衝突処理のオーバーライドを設定する
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// ノズルから玉を発射する
		GetWorld()->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
	else
	{
		// 発砲音を出す
		UGameplayStatics::PlaySound2D(this, EquippedWeaponInfo.GunshotSound);
		// 発砲炎アニメーションを出す
		UGameplayStatics::SpawnEmitterAttached(EquippedWeaponInfo.MuzzleFire, EquippedWeapon, EquippedWeaponInfo.MuzzleFireSoketName);
		// 身体の動くアニメーションを出す
		PlayAnimMontage(EquippedWeaponInfo.FiringMontage, 1.0f);
		// 弾薬を生成
		GetWorld()->SpawnActor<AWeaponAmmunition>(
			EquippedWeaponInfo.AmmunitionClass,
			FTransform(EquippedWeapon->GetSocketTransform(TEXT("None")))
		);
	}
}

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

void AGUNMANCharacter::ToggleFlipflop(bool bIsTPActive, bool bIsFPActive)
{
	// カメラはアクティブか？
	ThirdPersonCamera->SetActive(bIsTPActive);
	FirstPersonCamera->SetActive(bIsFPActive);

	// キャラクターを正面に向かせるか？
	bUseControllerRotationYaw = bIsFPActive;

	// ThirdPerson のメッシュを表示するか？
	TObjectPtr<USkeletalMeshComponent> MeshComponent = GetMesh();
	MeshComponent->SetVisibility(bIsTPActive);

	// TPS/FPS の切り替えでは ThirdPerson の武器を非表示にする
	Weapon->SetVisibility(false);

	TArray<TObjectPtr<AActor>> WeaponList;
	UGameplayStatics::GetAllActorsOfClass(this, AWeapon::StaticClass(), WeaponList);
	for (TObjectPtr<AActor> SearchActor : WeaponList)
	{
		// 所持していない武器を表示するか？
		TObjectPtr<AWeapon> NewActor = Cast<AWeapon>(SearchActor);
		NewActor->SetActorHiddenInGame(!bIsTPActive);
	}

	// FirstPerson のメッシュと武器を表示するか？
	Mesh1P->SetVisibility(bIsFPActive);
	FP_Gun->SetVisibility(bIsFPActive);

	// FirstPerson がアクティブか？
	bIsFP = bIsFPActive;
}

void AGUNMANCharacter::StartReadyGun()
{
	if (bHasArms == true && bIsFP == false)
	{
		CameraBoom->SocketOffset = CameraBoomSocketOffset;
		GunPreparationProcess(true, false, true);
		if (UIGunSightRef)
		{
			// 照準を画面に表示する
			UIGunSightRef->AddToViewport();
		}
	}
}

void AGUNMANCharacter::StopReadyGun()
{
	if (bHasArms == true && bIsFP == false)
	{
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
		GunPreparationProcess(false, true, false);
		if (UIGunSightRef)
		{
			// 照準を画面から取り除く
			UIGunSightRef->RemoveFromParent();
		}
	}
}

void AGUNMANCharacter::GunPreparationProcess(bool bIsAiming, bool bOrientRotationToMovement, bool bYawRotation)
{
	// エイムのフラグを設定
	bIsAimingState = bIsAiming;

	// エイムの状態をアニメーション（ThirdPerson）に反映
	IAnimationInterface* AnimInterface = Cast<IAnimationInterface>(TPMeshAnimInstance);
	if (AnimInterface)
	{
		AnimInterface->Execute_AimingState(TPMeshAnimInstance, bIsAimingState);
	}

	// カメラの向きの設定
	GetCharacterMovement()->bOrientRotationToMovement = bOrientRotationToMovement;

	// 歩く向きの設定
	bUseControllerRotationYaw = bYawRotation;
}

void AGUNMANCharacter::AttachingAndRemovingGun()
{
	if (bIsFP == false)
	{
		// 武器のインデックスをセット
		WeaponNumber = WeaponNumberCounter;

		// 武器を持っているか？
		if (!bHasArms)
		{
			// インデックスにあたる武器をセット
			EquippedWeapon = WeaponMeshes[WeaponNumber];

			// 武器を表示
			EquippedWeapon->SetHiddenInGame(false, false);

			// データテーブルにアクセス
			FName RowName = EquippedWeapon->ComponentTags[0];
			FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
			if (Row)
			{
				// 発砲時の情報をセット
				EquippedWeaponInfo.GunshotSound = Row->GunshotSound;
				EquippedWeaponInfo.MuzzleFire = Row->MuzzleFire;
				EquippedWeaponInfo.MuzzleFireSoketName = Row->MuzzleFireSoketName;
				EquippedWeaponInfo.FiringMontage = Row->FiringMontage;
				EquippedWeaponInfo.AmmunitionClass = Row->AmmunitionClass;
				EquippedWeaponInfo.AmmunitionSocketName = Row->AmmunitionSocketName;

				// 武器を装備してサウンドを再生する
				EquipWeapon(true, Row->HasPistol, Row->EquipSocketName);
				UGameplayStatics::PlaySound2D(this, Row->EquipmentNoise);
			}

			// カウント処理
			CountWeapon(WeaponMeshes, WeaponNumber);
		}
		else
		{
			// 外す武器を選ぶ
			FName RowName = RemoveWeapon(WeaponMeshes, WeaponNumber)->ComponentTags[0];

			// データテーブルにアクセス
			FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
			if (Row)
			{
				// 武器を外す
				EquipWeapon(false, false, Row->AttachSocketName);
			}
		}
	}
}

void AGUNMANCharacter::EquipWeapon(bool bHasWeapon, bool bHasPistol, FName SoketName)
{
	// ソケットに武器をアタッチする
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SoketName);

	bHasArms = bHasWeapon;

	IAnimationInterface* AnimInterface = Cast<IAnimationInterface>(TPMeshAnimInstance);
	if (AnimInterface)
	{
		// ステートを更新
		AnimInterface->Execute_EquippedState(TPMeshAnimInstance, bHasWeapon, bHasPistol);
	}
}

void AGUNMANCharacter::CountWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number)
{
	// Arms のインデックスを超えていないか？
	bool bHasNextWeaponNumber = (Number + 1 < Arms.Num()) ? true : false;

	// Arms のインデックスを超えないようにする
	WeaponNumberCounter = bHasNextWeaponNumber ? (Number + 1) : 0;
}

USkeletalMeshComponent* AGUNMANCharacter::RemoveWeapon(TArray<TObjectPtr<USkeletalMeshComponent>> Arms, int Number)
{
	// インデックスがマイナスか？
	bool bPickAllWeaponNumber = (Number - 1 < 0) ? true : false;

	// Arms のインデックスがマイナスになったときは元に戻す
	int PickIndex = bPickAllWeaponNumber ? Arms.Num() - 1 : Number - 1;
	return Arms[PickIndex];
}

void AGUNMANCharacter::PressedActionPauseMenu()
{
	// バトルマップにアクセス
	TObjectPtr<ABattleMapScript> BattleMapRef = Cast<ABattleMapScript>(GetWorld()->GetLevelScriptActor());
	if (BattleMapRef)
	{
		// プレイヤーコントローラーを取得
		TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 現在の MappingContext からポーズメニューの MappingContext に変更する
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(BattleMapRef->PauseMenuMappingContext, 0);
		}

		// ポーズメニューのボタンの位置を初期化
		BattleMapRef->InitializeButtonPosition();
	}
}

void AGUNMANCharacter::StartJump()
{
	// ジャンプする
	Jump();

	// ジャンプしたか
	JumpButtonDown = CanJump();

	// MovementComponent が CharacterMovementComponent だったら
	TObjectPtr<UCharacterMovementComponent> MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent)
	{
		// ベクトルの長さが0以上か
		if (MovementComponent->Velocity.Size() > 0.0f)
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
		else
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}

	}
}

void AGUNMANCharacter::StopJump()
{
	// ジャンプをやめる
	StopJumping();

	// ジャンプしていないのでオフ
	JumpButtonDown = false;

	// MovementComponent が CharacterMovementComponent だったら
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent)
	{
		// ベクトルの長さが0以上か
		if (MovementComponent->Velocity.Size() > 0.0f)
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
		else
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}

	}
}

void AGUNMANCharacter::MoveForward(const FInputActionValue& Value)
{
	if ((Controller != nullptr) && (Value.Get<float>() != 0.0f))
	{
		// どちらが前方向か調べる
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 前方向ベクトルの取得
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// その方向に移動する
		AddMovementInput(Direction, Value.Get<float>());
	}
}

void AGUNMANCharacter::MoveRight(const FInputActionValue& Value)
{
	if ((Controller != nullptr) && (Value.Get<float>() != 0.0f))
	{
		// どちらが右方向か調べる
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 右方向ベクトルの取得 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// その方向に移動する
		AddMovementInput(Direction, Value.Get<float>());
	}
}

void AGUNMANCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// コントローラにヨーとピッチの入力を追加
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGUNMANCharacter::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// ダメージ分、体力を減らす
	CurrentHealth -= Damage;
	// 体力がなくなったら
	if (CurrentHealth <= DeadHealth)
	{
		UGameplayStatics::OpenLevel(this, "GameOverMap");
	}
}

void AGUNMANCharacter::CreateTimeline()
{
	// カーブアセットの取得
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Find(TEXT("CurveFloat'/Game/GUNMAN/Blueprint/Characters/CB_Run.CB_Run'"));
	if (Find.Succeeded())
	{
		RunCurve = Find.Object;
	}

	// タイムラインのインスタンス生成
	RunTimeline = new FTimeline();

	// タイムラインの長さを 1 秒に設定
	RunTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	RunTimeline->SetTimelineLength(1.0f);

	// タイムライン更新時のデリゲート
	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, TEXT("Timeline_LinearInterpCharacterSpeed"));
	RunTimeline->AddInterpFloat(RunCurve, TimelineProgress);
}

void AGUNMANCharacter::Timeline_LinearInterpCharacterSpeed(float Value)
{
	GetCharacterMovement()->MaxWalkSpeed = Value;
}

void AGUNMANCharacter::StartTimeline()
{
	// タイムライン開始
	if (RunTimeline != nullptr)
	{
		RunTimeline->PlayFromStart();
	}
}

void AGUNMANCharacter::ReverseTimeline()
{
	// タイムラインをリバース
	if (RunTimeline != nullptr)
	{
		RunTimeline->ReverseFromEnd();
	}
}

void AGUNMANCharacter::DisplayGunSight(TObjectPtr<APlayerController>& PlayerController)
{
	// WidgetBlueprint の Class を取得する
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_UIGunSight.WBP_UIGunSight_C";
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	// WidgetClass と PlayerController が取得できたか判定する
	if (WidgetClass && PlayerController)
	{
		// 銃の照準用のウィジェットを作成する
		UIGunSightRef = Cast<UUIGunSight>(CreateWidget(PlayerController, WidgetClass));
	}
}

void AGUNMANCharacter::DisplayCharacterUI(TObjectPtr<APlayerController>& PlayerController)
{
	// WidgetBlueprint の Class を取得する
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_UICharacter.WBP_UICharacter_C";
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	// WidgetClass と PlayerController が取得できたか判定する
	if (WidgetClass && PlayerController)
	{
		// キャラクター用のウィジェットを作成する
		TObjectPtr<UUICharacter> UICharacterRef = Cast<UUICharacter>(CreateWidget(PlayerController, WidgetClass));

		if (UICharacterRef)
		{
			// キャラクターの UI を画面に表示する
			UICharacterRef->AddToViewport();
		}
	}
}

void AGUNMANCharacter::FireState_Implementation(bool bCanATK)
{
	bCanAttack = bCanATK;
}

void AGUNMANCharacter::AttachWeapon_Implementation(USkeletalMeshComponent* WeaponMesh, FName AttachSoketName)
{
	// 武器をアタッチ
	WeaponMeshes.Add(WeaponMesh);
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), AttachSoketName);
}