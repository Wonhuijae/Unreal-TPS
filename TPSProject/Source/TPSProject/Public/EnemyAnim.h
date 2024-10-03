// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
    // 애니메이션 상태 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
    EEnemyState AnimState;

    // 공격 애니메이션 재생 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
    bool bAttackPlay = false;

    // 공격 애니메이션 종료 이벤트 함수
    UFUNCTION(BlueprintCallable, Category = FSMEvent)
    void OnEndAttackAnimation();

    // 피격 애니메이션 재생 함수
    UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
    void PlayDamageAnim(FName SectionName);

    UFUNCTION(BlueprintCallable)
    void OnDamageEnd();

    // 죽음 상태 애니메이션 종료 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
    bool bDieDone = false;
};
