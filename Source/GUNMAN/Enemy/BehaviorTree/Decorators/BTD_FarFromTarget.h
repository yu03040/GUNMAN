// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_FarFromTarget.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UBTD_FarFromTarget : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	

private:
	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess="true"))
	FBlackboardKeySelector TargetActorKey;

	// プレイヤーを見失わない距離
	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	float Distance;

protected:
	// Blackboard に基づく条件チェックする関数
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UBTD_FarFromTarget();
};
