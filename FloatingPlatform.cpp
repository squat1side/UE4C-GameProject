// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//初始化BeginPlay后，给两个FVector一些默认数值
	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	//初始化bInterping
	bInterping = false;

	//初始化InterpSpeed - 可以设置Floating移动速度
	InterpSpeed = 2.0f;

	//初始化计时器时间参数InterpTime
	InterpTime = 1.f;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();

	//处理终点位置 - Endpoint是属于FloatingPlatform的局部坐标轴
	EndPoint += StartPoint;

	//将布尔值设为False
	bInterping = false;

	//设置计时器 - WorldTimerMamager
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	//初始化起点和终点的距离 -用起点减终点后返回的是一个Vector
	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//检查是否有插值
	if (bInterping)
	{
		//设置在起点和终点间的float的漂浮 - 使用插值
		//获取两点间每一帧的位置
		FVector CurrentLocation = GetActorLocation();

		//获取位置后便可以插值 - V - Vector; InterpTo表示插入到3D空间指定位置
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);

		//利用Interp来设定在起点和终点的位置 - Interp就是设定的位置
		SetActorLocation(Interp);

		//计算移动的距离并判断位置
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping();

			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			//替换起点和终点的位置
			SwapVectors(StartPoint, EndPoint);
		}
	}

}


void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

//写入Swap函数的主体 - 设置一个临时变量Temp
void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}
