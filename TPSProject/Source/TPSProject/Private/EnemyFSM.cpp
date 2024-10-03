// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnim.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

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

	// 월드에서 플레이어 타입의 타겟 찾기
	auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(Actor);
	
	// 자신을 소유한 객체 가져오기
	me = Cast<AEnemy>(GetOwner());
	// 사용 중인 UEnemyAnim 가져오기
	Anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());

	// AAIController 할당
	AI = Cast<AAIController>(me->GetController());

	// 체력 초기화
	hp = 0;
	UpdateHP(maxHp);
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행 창에 상태 메시지 출력
	FString LogMsg = UEnum::GetValueAsString(eState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, LogMsg);

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
		Anim->AnimState = eState;
		
		// 타겟이 없을 때는 랜덤한 위치로 이동함
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, RandomPos);
	}
}

void UEnemyFSM::MoveState()
{
	// 타겟 방향 및 거리 구하기
	FVector dest = target->GetActorLocation();
	FVector dir = dest - me->GetActorLocation();

	// 네비게이션
	auto NS = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FPathFindingQuery query;
	FAIMoveRequest req;

	// 목적지 인식 범위
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(dest);

	// 길찾기 쿼리 생성
	AI->BuildPathfindingQuery(req, query);

	// 쿼리 결과
	FPathFindingResult R = NS->FindPathSync(query);

	// 길찾기 성공
	if (R.Result == ENavigationQueryResult::Success)
	{
		AI->MoveToLocation(dest);
	}
	// 실패
	else
	{
		// 랜덤 이동
		auto r = AI->MoveToLocation(RandomPos);
		// 도착 시
		if (r == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, RandomPos);
		}
	}

	// 타겟과 거리가 가까우면 공격 상태로 전환
	if (dir.Size() < attackRange)
	{
		eState = EEnemyState::Attack;
		currentTime = 0;
		AI->StopMovement();

		Anim->AnimState = eState;
		Anim->bAttackPlay = true;

		// 대기 시간 종료
		currentTime = attackDelay;
	}
	
}

void UEnemyFSM::AttackState()
{
	// 일정 시간에 한 번씩 공격
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > attackDelay)
	{
		// 경과시간 초기화하고 공격 상태 변수 true
		currentTime = 0;
		Anim-> bAttackPlay = true;
	}

	// 멀어지면 상태 전환
	float Distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	if (Distance > attackRange)
	{
		eState = EEnemyState::Move;
		Anim->AnimState = eState;

		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, RandomPos);
	}
}

void UEnemyFSM::DamageState()
{
	// 몽타주로 처리
	/*currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > damageDelay)
	{
		eState = EEnemyState::Idle;
		currentTime = 0;
	}*/
}

void UEnemyFSM::DieState()
{
	// 아직 죽음 애니메이션이 끝나지 않았다면
	// 바닥으로 내려가지 않도록 처리
	if (Anim->bDieDone == false)
	{
		return;
	}

	// 계속 아래로 내려간다
	// 등속 운동 공식 P = P0 + VT
	FVector P0 = me->GetActorLocation();
	FVector VT = FVector::DownVector * DieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + VT;
	me->SetActorLocation(P);

	// 만약 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		// 제거시킨다.
		me->Destroy();
	}
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
		AI->StopMovement();
		// 피격 애니메이션 재생
		Anim->PlayDamageAnim(TEXT("Damage"));
	}
	// 그렇지 않다면
	else
	{
		// 상태를 죽음으로 전환
		eState = EEnemyState::Die;

		// 캡슐 컴포넌트 충돌체 비활성화
		me -> GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 죽음 애니메이션 재생
		Anim->PlayDamageAnim(TEXT("Die"));

		// me->Destroy();
	}
	// 애니메이션 상태 동기화
	Anim->AnimState = eState;
}

void UEnemyFSM::OnChangeMoveState()
{
	eState = EEnemyState::Move;
}

bool UEnemyFSM::GetRandomPositionInNavMesh(FVector CenterLocation, float Radius, FVector& Dest)
{
	auto NS = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation Loc;
	bool Result = NS->GetRandomReachablePointInRadius(CenterLocation, Radius, Loc);
	Dest = Loc.Location;
	return Result;
}

void UEnemyFSM::UpdateHP(int32 NewHP)
{
	hp = FMath::Max(0, hp + NewHP);
}
