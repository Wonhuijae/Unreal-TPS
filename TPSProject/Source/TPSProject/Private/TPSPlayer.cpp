// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "PlayerAnim.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	// TargetArmLength: 카메라<->캐릭터 사이 거리
	SpringArmComp->TargetArmLength = 400;

	// 카메라 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	// 생성자 도우미를 이용해 스켈레탈 메시 로드
	// * 파일 경로를 직접 사용하는 것은 유지보수가 곤란해지기 때문에 권장되지 않음
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith'"));
	// 로드 성공 시	
	if (tempMesh.Succeeded())
	{
		// 로드된 메시 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 메시 위치 설정 및 회전
		// 위치 (X=0.000000,Y=0.000000,Z=-90.000000)
		// 회전 (Pitch=0.000000,Yaw=-90.000000,Roll=0.000000)
		// 회전은 pitch, yaw, roll 순서대로 할당해야 함
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		
		// 캐릭터가 입력에 따라 Yaw축(좌우)방향으로 회전
		bUseControllerRotationYaw = true;
		// 컨트롤러의 회전 정보를 사용->캐릭터의 이동에 따라 회전
		SpringArmComp->bUsePawnControlRotation = true;
		// 카메라는 스프링 암의 자식이므로 부모 객체의 설정을 사용
		CameraComp->bUsePawnControlRotation = false;
		// 캐릭터가 움직이는 방향으로 자동 회전
		GetCharacterMovement()->bOrientRotationToMovement = true;

		// 다단점프
		JumpMaxCount = 2;
		// 공중 컨트롤(공중에서 이동)
		GetCharacterMovement()->AirControl = 1;
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기 속도는 걷기
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subSystem)
		{
			subSystem->AddMappingContext(IMC_TPS, 0);
		}
	}
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}

// Called to bind functionality to input
// 입력에 맞게 함수 바인딩
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		// 이동
		PlayerInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(IA_PlayerMove, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		PlayerInput->BindAction(IA_InputJump, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
		// 뛰기 애니메이션
		PlayerInput->BindAction(IA_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);

		// 발사
		// Started: 누를 때 한 번만 처리되도록 함. 
		PlayerInput->BindAction(IA_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
	}
}

void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	// inputValue의 Float(1D)값을 가져옴
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	// 상하 입력
	Direction.X = value.X;
	// 좌우 입력
	Direction.Y = value.Y;
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	// 언리얼 내장 함수
	Jump();
}

void ATPSPlayer::InputRun()
{
	auto Movement = GetCharacterMovement();
	// 만약, 현재 달리기 모드라면
	if (Movement->MaxWalkSpeed > WalkSpeed)
	{
		// 걷기 속도로 전환
		Movement->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		Movement->MaxWalkSpeed = RunSpeed;
	}
}

void ATPSPlayer::PlayerMove()
{
	// 이동 처리를 위한 방향은 Tick에서 갱신해줘야 함
	Direction = FTransform(GetControlRotation()).TransformVector(Direction);
	// 등속 운동 공식 (직접 갱신)
	// P = P0(현재 위치) + V (속도 * 방향) * T(시간)
	// FVector P0 = GetActorLocation;
	// FVector VT = Direction * WalkSpeed * DeltaTime;
	// SetActorLocation(Direction);

	// 언리얼 엔진 내장 함수
	AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	// 발사 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// 총알 스폰
	FTransform firePos = GetMesh()->GetSocketTransform(TEXT("Muzzle_01"));
	GetWorld()->SpawnActor<ABullet>(BulletFactory, firePos);

	 
}
