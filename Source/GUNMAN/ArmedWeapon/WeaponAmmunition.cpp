// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAmmunition.h"
#include "GameFramework/Character.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWeaponAmmunition::AWeaponAmmunition()
{
	// SceneComponent を作成する
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// SceneComponent を RootComponent に設定する
	RootComponent = DefaultSceneRoot;

	// StaticMeshComponent を作成する
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// StaticMeshComponentをRootComponentにAttachする
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponAmmunition::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーにアクセス
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		// 飛ばす力
		FVector Impulse = Player->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(1.0f, 5.0f);
		// 物理シュミレーションをオン
		StaticMesh->SetSimulatePhysics(true);
		// Impulse 分、弾薬を飛ばす
		StaticMesh->AddImpulse(Impulse);
	}
}

