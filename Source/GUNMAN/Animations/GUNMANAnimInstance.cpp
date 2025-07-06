// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMANAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/GameplayStatics.h"

UGUNMANAnimInstance::UGUNMANAnimInstance()
{
}

void UGUNMANAnimInstance::NativeBeginPlay()
{
	//Superの呼び出し
	Super::NativeBeginPlay();
	PlayerCharacterRef = Cast<ACharacter>(TryGetPawnOwner());
}

void UGUNMANAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	//Superの呼び出し
	Super::NativeUpdateAnimation(DeltaTime);

	DeltaTimeX = DeltaTime;

	if (IsValid(PlayerCharacterRef) == true)
	{
		// 接地の設定（ステートマシンで使用される）
		bIsInAir = PlayerCharacterRef->GetMovementComponent()->IsFalling();

		// スピードの設定（1Dブレンドスペースで使用）
		Speed = PlayerCharacterRef->GetVelocity().Size();

		// 方向の設定
		FVector Velocity = PlayerCharacterRef->GetVelocity();
		FRotator BaseRotation = PlayerCharacterRef->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation);

		// 武器を構えているかの設定
		FRotator CurrentRotation = UKismetMathLibrary::MakeRotator(0.0f, AimPitch, AimYaw);

		FRotator ControlRotation = PlayerCharacterRef->GetControlRotation();
		FRotator ActorRotation = PlayerCharacterRef->GetActorRotation();
		FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);

		float InterpSpeed = 5.0f;
		FRotator BreakRotator = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTimeX, InterpSpeed);

		double PitchAngleDegree = BreakRotator.Pitch;
		double YawAngleDegree = BreakRotator.Yaw;
		double MinAngleDegree = -90.0;
		double MaxAngleDegree = 90.0;
		AimPitch = UKismetMathLibrary::ClampAngle(PitchAngleDegree, MinAngleDegree, MaxAngleDegree);
		AimYaw = UKismetMathLibrary::ClampAngle(YawAngleDegree, MinAngleDegree, MaxAngleDegree);
	}
}

void UGUNMANAnimInstance::AimingState_Implementation(bool bIsAiming)
{
	// 構えているか受け取る
	IsAiming = bIsAiming;
}

void UGUNMANAnimInstance::EquippedState_Implementation(bool bHasWeapon, bool bHasPistol)
{
	// 武器とピストルの状態を受け取る
	HasWeapon = bHasWeapon;
	HasPistol = bHasPistol;
}
