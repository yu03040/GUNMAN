// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"
#include "AIEnemyController.h"
#include "GUNMAN/UMG/UIEnemy.h"
#include "GUNMAN/ArmedWeapon/WeaponStructure.h"
#include "Perception/AIPerceptionComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIEnemyController::StaticClass();

	// Weapon を Mesh の子に
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh());

	// Widget を Mesh の子に
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(GetMesh());

	// スポーンされた敵に対してもAIを適用
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// DataTable をロード
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/GUNMAN/Blueprint/ArmedWeapon/DT_Weapon.DT_Weapon"));
	if (DataTableFinder.Succeeded())
	{
		WeaponDataTable = DataTableFinder.Object;
	}

	// カーブアセットの取得
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Find(TEXT("CurveFloat'/Game/GUNMAN/Blueprint/Enemy/CB_EnemyRun.CB_EnemyRun'"));
	if (Find.Succeeded())
	{
		FloatCurve = Find.Object;
	}

	// タイムライン初期化
	RunTimeline = new FTimeline();

	// タイムラインモード設定
	RunTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	RunTimeline->SetTimelineLength(1.0f);

	// タイムライン更新時のデリゲート
	FOnTimelineFloat TimeLineFloat;
	TimeLineFloat.BindUFunction(this, TEXT("TimeLineFunc"));
	RunTimeline->AddInterpFloat(FloatCurve, TimeLineFloat);

	bIsAlive = true;
	CurrentHealth = 30.0f;
	MaxHealth = CurrentHealth;
	DeadHealth = 0.0f;
	EnemyATK = 10.0f;
	InLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーキャラクター（ACharacter）を取得する
	Player = UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0);
	// 敵のコントローラーを取得する
	EnemyController = GetController();

	// AnyDamageデリゲートをバインド
	OnTakeAnyDamage.AddDynamic(this, &AAIEnemy::HandleAnyDamage);

	// ウィジェットをもらう（ダメージ処理で使う）
	TObjectPtr<UUIEnemy> WidgetClass = Cast<UUIEnemy>(Widget->GetUserWidgetObject());
	if (WidgetClass)
	{
		EnemyWidget = WidgetClass;
		EnemyWidget->SetOwningEnemy(this);
	}

	FName RowName = FName(TEXT("Rifle"));
	FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
	if (Row)
	{
		// 武器をメッシュにアタッチ
		Weapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules(
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepRelative,
				true),
			Row->EquipSocketName
		);
	}
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIEnemy::AttackCharacter_Implementation()
{
	if (bIsAlive)
	{
		FName RowName = FName(TEXT("Rifle"));
		FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
		if (Row)
		{
			EquippedWeaponInformation.GunshotSound = Row->GunshotSound;
			EquippedWeaponInformation.MuzzleFire = Row->MuzzleFire;
			EquippedWeaponInformation.MuzzleFireSoketName = Row->MuzzleFireSoketName;
			EquippedWeaponInformation.FiringMontage = Row->FiringMontage;
			EquippedWeaponInformation.AmmunitionClass = Row->AmmunitionClass;
			EquippedWeaponInformation.AmmunitionSocketName = Row->AmmunitionSocketName;

			// 効果音再生
			UGameplayStatics::PlaySound2D(this, EquippedWeaponInformation.GunshotSound);

			// アニメーション再生
			PlayAnimMontage(EquippedWeaponInformation.FiringMontage, 0.1);

			// 銃口エフェクトを発生
			UGameplayStatics::SpawnEmitterAttached(
				EquippedWeaponInformation.MuzzleFire,
				Weapon,
				EquippedWeaponInformation.MuzzleFireSoketName,
				FVector(0.0f, 0.0f, 0.0f),
				FRotator(0.0f, 0.0f, 0.0f),
				FVector(1.0f, 1.0f, 1.0f),
				EAttachLocation::KeepRelativeOffset,
				true,
				EPSCPoolMethod::None,
				true
			);

			// 弾薬生成
			CreateAmmunition();
		}
	}
}

void AAIEnemy::CreateAmmunition()
{
	FTransform SpawnTransform = Weapon->GetSocketTransform(
		EquippedWeaponInformation.MuzzleFireSoketName,
		ERelativeTransformSpace::RTS_World
	);

	// 弾薬が銃口から飛ぶ
	TObjectPtr<AActor> SpawnActor = GetWorld()->SpawnActor<AActor>(
		EquippedWeaponInformation.AmmunitionClass,
		SpawnTransform
	);

	FVector Start = Weapon->GetSocketLocation(EquippedWeaponInformation.AmmunitionSocketName);
	FVector End = Start + 10000.0f * UKismetMathLibrary::GetForwardVector(Weapon->GetSocketRotation(EquippedWeaponInformation.MuzzleFireSoketName));
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	FHitResult OutHit;

	// レーザー光を飛ばす
	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f
	);

	// プレイヤーに当たったら
	if (OutHit.GetActor() == Player)
	{
		// ダメージを与える
		UGameplayStatics::ApplyDamage(Player, EnemyATK, nullptr, nullptr, nullptr);
	}
}

void AAIEnemy::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// ダメージ分、体力を減らす
	CurrentHealth -= Damage;

	if (CurrentHealth <= DeadHealth)
	{
		// 敵の動きを止める
		GetCharacterMovement()->DisableMovement();
		TObjectPtr<UCapsuleComponent> EnemyCapsuleComponent = GetCapsuleComponent();
		if (EnemyCapsuleComponent)
		{
			// コリジョンをオフ
			EnemyCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// 物理シュミレーションをオン
			GetMesh()->SetSimulatePhysics(true);
			// 指定された秒数が過ぎると崩れる
			SetLifeSpan(InLifeSpan);
			bIsAlive = false;
		}
	}
	else
	{
		// 攻撃されたらランダムにアニメーションを再生する
		int32 i = UKismetMathLibrary::RandomIntegerInRange(0, 3);
		PlayAnimMontage(HitAnimMontage[i]);
	}
}

float AAIEnemy::GetCurrentHealth() const
{
	return CurrentHealth;
}

float AAIEnemy::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

void AAIEnemy::TimeLineFunc(float Value)
{
	// 走るスピードを滑らかに
	GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FInterpTo(
		GetCharacterMovement()->MaxWalkSpeed,
		EnemySpeed,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		5.0f
	);
}

void AAIEnemy::ChangeRunningSpeed_Implementation(float Speed)
{
	// タイムライン開始
	if (RunTimeline)
	{
		EnemySpeed = Speed;
		RunTimeline->PlayFromStart();
	}
}
