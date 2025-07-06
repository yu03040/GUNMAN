// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_RunningToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UBTT_RunningToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed;

	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActorKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTT_RunningToTarget();
};
