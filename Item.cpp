// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "particles/ParticleSystemComponent.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//创建CollisionVolume
	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	//将CollisionVolume设定为根组件
	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//将网格绑定到Root
	Mesh->SetupAttachment(GetRootComponent());

	//UParticleSystemComponent
	IdelParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticleComponent"));
	//将IdelParticlesComponent绑定到Root
	IdelParticlesComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	//初始化Rotationrate
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	//添加重叠
    //BeginOverlap
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	//EndOverlap
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//实现旋转
	if (bRotate)
	{
		//再创建一个FRotator - Rotation
		FRotator Rotation = GetActorRotation();
		//添加到Yaw - 获得每一帧旋转所需的值
		Rotation.Yaw += DeltaTime * RotationRate;
		//调用SetActorRotation
		SetActorRotation(Rotation);
	}

}


//创建OnOverlapBegin函数
void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
//创建OnOverlapEnd函数
//OnOverlapEnd函数主体
void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
