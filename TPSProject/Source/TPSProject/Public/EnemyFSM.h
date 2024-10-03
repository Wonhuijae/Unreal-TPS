// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	// 적 상태 enum
	Idle,
	Move,
	Attack,
	Damage,
	Die
};

UCLASS()
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEnemyFSM();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 적 행동 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	EEnemyState eState = EEnemyState::Idle;

	// 상태별 처리 함수
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float moveCoolTime = 2;
	float currentTime = 0;

	// 공격 타겟
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;

	// 컴포넌트를 소유한 액터
	UPROPERTY()
	class AEnemy* me;

	// 피격 처리 함수
	void OnDamage();

	// 적 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = 3;

	// 피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelay = 2.f;

};
