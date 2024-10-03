// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 카메라 관련 컴포넌트
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* CameraComp;

	// 입력 처리 함수
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_TPS;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Turn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_InputJump;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Run;

	// 좌우 회전
	void Turn(const struct FInputActionValue& inputValue);
	// 상하 회전
	void LookUp(const struct FInputActionValue& inputValue);

	// 이동
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_PlayerMove;

	// 속도
	UPROPERTY(EditAnywhere, Category = "PlayerSetting")
	float WalkSpeed = 300;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float RunSpeed = 600;
	// 방향
	FVector Direction;
	// 이동 처리 함수
	void Move(const struct FInputActionValue& inputValue);

	// 내장 Jump()함수와 구분하기 위해 다르게 써야 함
	void InputJump(const struct FInputActionValue& inputValue);

	// 달리기
	void InputRun();

	// move 관련 tick 함수 내 갱신 사항 정리 함수
	void PlayerMove();

	// 총알 발사
	UPROPERTY(EditAnywhere, Category = BulletFactory)
	// 템플릿에 등록된 클래스를 언리얼 에디터로부터 할당받음
	TSubclassOf<class ABullet> BulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Fire;
	// 총알 발사 처리
	void InputFire(const struct FInputActionValue& inputValue);

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* BulletSound;
};
