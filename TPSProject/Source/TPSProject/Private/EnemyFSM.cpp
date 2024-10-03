// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
UEnemyFSM::UEnemyFSM()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 생애 동안 상태를 갱신한다
	switch (eState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	// 시간이 흐르면 이동 가능 상태로 전환
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > moveCoolTime)
	{
		eState = EEnemyState::Move;
		currentTime = 0;
	}

	// 타겟이 없을 때는 랜덤한 위치로 이동함
	
}

void UEnemyFSM::MoveState()
{
	// 타겟이 있으면 그 방향으로 이동
	FVector dest = target->GetActorLocation();

	// 타겟과 거리가 가까우면 공격 상태로 전환
	
}

void UEnemyFSM::AttackState()
{
	
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > damageDelay)
	{
		eState = EEnemyState::Idle;
		currentTime = 0;
	}
}

void UEnemyFSM::DieState()
{
}

void UEnemyFSM::OnDamage()
{
	// 만약 체력이 남아있다면
	hp--;
	if (hp > 0)
	{
		// 상태를 피격으로 전환
		eState = EEnemyState::Damage;

		currentTime = 0;
		//AI->StopMovement();
		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString SectionName = FString::Printf(TEXT("Damage%d"), index);
		//Anim->PlayDamageAnim(FName(*SectionName));
	}
	// 그렇지 않다면
	else
	{
		// 상태를 죽음으로 전환
		eState = EEnemyState::Die;

		// 캡슐 컴포넌트 충돌체 비활성화
		me -> GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 죽음 애니메이션 재생
		//Anim->PlayDamageAnim(TEXT("Die"));
	}
	// 애니메이션 상태 동기화
	//Anim->AnimState = eState;
}
