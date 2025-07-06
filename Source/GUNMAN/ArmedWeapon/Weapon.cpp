// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
	// SceneComponentを作成する
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// SceneComponent を RootComponentに設定する
	RootComponent = DefaultSceneRoot;

	// SkeletalMeshComponen を作成する
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));

	// RootComponent の子に
	WeaponMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/GUNMAN/Blueprint/ArmedWeapon/DT_Weapon.DT_Weapon"));
	if (DataTableFinder.Succeeded())
	{
		WeaponDataTable = DataTableFinder.Object;
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// 武器を装備するまで見えなくする
	WeaponMesh->SetHiddenInGame(true, true);

	// データテーブルにアクセス
	FName RowName = WeaponMesh->ComponentTags[0];
	FWeaponStructure* Row = WeaponDataTable->FindRow<FWeaponStructure>(RowName, "");
	if (Row)
	{
		TObjectPtr<AGUNMANCharacter> Player = Cast<AGUNMANCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (Player)
		{
			IWeaponInterface* Interface = Cast<IWeaponInterface>(Player);
			if (Interface)
			{
				// 武器をアタッチ
				Interface->Execute_AttachWeapon(Player, WeaponMesh, Row->AttachSocketName);
			}
		}
	}
}