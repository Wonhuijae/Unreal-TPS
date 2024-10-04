// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Meshes/Grux.Grux'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));

	// 애니메이션 블루프린트 할당하기
	// 블루프린트 클래스는 반드시 _C를 붙여줌
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	// 캡슐 컴포넌트와 메시 컴포넌트의 충돌 채널을 설정한다.
	// 캡슐 컴포넌트 Visibility : Block / Camera : Ignore
	// 메시 컴포넌트 Visibility : Ignore / Camera : Ignore
	UCapsuleComponent* Cap = GetCapsuleComponent();
	auto EMesh = GetMesh();
	Cap->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Cap->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	EMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	EMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// 월드 배치 또는 스폰 시 자동으로 Possess
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 체력 UI
	HPComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPComp"));
	HPComp->SetupAttachment(RootComponent);
	ConstructorHelpers::FClassFinder<UUserWidget> tempHP(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/WBP_EnemyHP.WBP_EnemyHP'"));
	if (tempHP.Succeeded())
	{
		HPComp->SetWidgetClass(tempHP.Class);
		// Draw Size = 150, 20
		HPComp->SetDrawSize(FVector2D(150, 20));
		// Location = 0, 0, 90
		HPComp->SetRelativeLocation(FVector(0, 0, 90));
		// 충돌체 설정
		HPComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UKismetMathLibrary::FindLookAtRotation를 이용해서 HPComp를 카메라 쪽으로 회전
	FVector Start = HPComp->K2_GetComponentLocation();
	FVector Target = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();

	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

	HPComp->SetWorldRotation(NewRotation);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
