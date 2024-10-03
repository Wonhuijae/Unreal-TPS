// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	// 오너를 가져와서 유효성 체크
	if (nullptr == Player)
	{
		// 애니메이션을 제어하는 캐릭터 클래스에 접근
		// 폰 타입으로 가져오므로 TPSPlayer 타입으로 캐스팅함
		auto Owner = TryGetPawnOwner();
		Player = Cast<ATPSPlayer>(Owner);
	}
	if (nullptr == Player)
	{
		return; // 책에 없는 부분
	}

	// 오너한테 Actor Forward Vector 와 Velocity 를 알아내어
	FVector Forward = Player->GetActorForwardVector();
	FVector Velocity = Player->GetVelocity();

	// Speed 의 값에 할당.
	Speed = FVector::DotProduct(Forward, Velocity);
	// Actor Right Vector 를 알아내어
	FVector Right = Player->GetActorRightVector();
	// Direction 값에 할당.
	Direction = FVector::DotProduct(Right, Velocity);
	
	// 캐릭터 무브먼트에서 Is Falling 을 가져와서 IsInAir에 할당
	UCharacterMovementComponent* Movement = Player->GetCharacterMovement();
	IsInAir = Movement->IsFalling();
}

void UPlayerAnim::PlayAttackAnim()
{
	if (AttackAnimMontage)
	{
		Montage_Play(AttackAnimMontage);
	}
}
