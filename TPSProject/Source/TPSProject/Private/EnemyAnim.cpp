// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "Enemy.h"

void UEnemyAnim::OnEndAttackAnimation()
{
    bAttackPlay = false;
}

void UEnemyAnim::OnDamageEnd()
{
    // Enemy를 가져와서 EnemyFSM 의 함수를 호출하고 싶다.
    AEnemy* Enemy = Cast<AEnemy>(TryGetPawnOwner());
    if (Enemy)
    {
        // Enemy->EnemyFSM->OnChangeMoveState();
    }
}