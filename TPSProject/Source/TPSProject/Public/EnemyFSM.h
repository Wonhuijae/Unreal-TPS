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

	// 공격 범위
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 200.0f;
	// 공격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelay = 2.5f;

	// 피격 처리 함수
	void OnDamage();
	// 피격 애니메이션 전환 이벤트
	void OnChangeMoveState();

	// 적 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = 0;
	int32 maxHp = 3;

	// 피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelay = 2.f;

	// 애니메이션 블루프린트 변수
	UPROPERTY()
	class UEnemyAnim* Anim;

	// Enemy를 소유하고 있는 AIController
	UPROPERTY()
	class AAIController* AI;

	// 길을 찾을 경우 랜덤한 위치 변수
	FVector RandomPos;
	// 랜덤한 위치를 가져올 함수
	bool GetRandomPositionInNavMesh(FVector CenterLocation, float Radius, FVector& Dest);

	void UpdateHP(int32 NewHP);
};
