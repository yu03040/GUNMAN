// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathPoint.generated.h"

UCLASS()
class GUNMAN_API APathPoint : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget="true"))
	TArray<FVector> PathPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	APathPoint();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
