// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/GameplayStatics.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{
}

void UEnemyAnimInstance::NativeBeginPlay()
{
	//Superの呼び出し
	Super::NativeBeginPlay();
	EnemyCharacterRef = Cast<ACharacter>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	//Superの呼び出し
	Super::NativeUpdateAnimation(DeltaTime);

	DeltaTimeX = DeltaTime;

	if (IsValid(EnemyCharacterRef) == true)
	{
		// 接地の設定（ステートマシンで使用される）
		bIsInAir = EnemyCharacterRef->GetMovementComponent()->IsFalling();

		// スピードの設定（1Dブレンドスペースで使用）
		Speed = EnemyCharacterRef->GetVelocity().Size();

		// 方向の設定
		FVector Velocity = EnemyCharacterRef->GetVelocity();
		FRotator BaseRotation = EnemyCharacterRef->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation);

		// 武器を構えているかの設定
		FRotator CurrentRotation = UKismetMathLibrary::MakeRotator(0.0f, AimPitch, AimYaw);

		FRotator ControlRotation = EnemyCharacterRef->GetControlRotation();
		FRotator ActorRotation = EnemyCharacterRef->GetActorRotation();
		FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);

		float InterpSpeed = 5.0f;
		FRotator BreakRotator = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTimeX, InterpSpeed);

		double PitchAngleDegree = BreakRotator.Pitch;
		double YawAngleDegree = BreakRotator.Yaw;
		double MinAngleDegree = -50.0;
		double MaxAngleDegree = 50.0;
		AimPitch = UKismetMathLibrary::ClampAngle(PitchAngleDegree, MinAngleDegree, MaxAngleDegree);
		AimYaw = UKismetMathLibrary::ClampAngle(YawAngleDegree, MinAngleDegree, MaxAngleDegree);
	}
}
