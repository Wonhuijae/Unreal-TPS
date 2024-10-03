// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 생성 후 반지름 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(12.5f);
	// 충돌 설정
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));

	// 외관을 충돌체에 붙이고 크기 설정
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetRelativeScale3D(FVector(0.1f));
	// Mesh Comp - Collision 설정 
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Move Component 를 만들고 속력 설정
	// 특정 컴포넌트에 대한 이동을 지원하는 컴포넌트
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 5000.f;
	MovementComp->MaxSpeed = 5000.f;


	MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = .3f;

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::Die()
{
	Destroy();
}

