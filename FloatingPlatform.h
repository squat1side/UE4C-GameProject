// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	//添加一个网格
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
		class UStaticMeshComponent* Mesh;

	//添加几个Vector来代替空间返回的点 - 起点 和 终点
	UPROPERTY(EditAnywhere)
		FVector StartPoint;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
		FVector EndPoint;

	//创建InterSpeed变量 - 针对Endpoint  -  DeltaTime的帧数前一帧
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		float InterpSpeed;

	//为计时器设置一个时间参数 -来获取插值计时器的时间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		float InterpTime;

	//创建计时器 - InterpTimer可以打开或关闭对应是否在插值
	FTimerHandle InterpTimer;

	//判断是否在插值 - 创建布尔值 - bInterping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		bool bInterping;

	//到达终点后，将起点和终点互换，获取两点的距离
	float Distance;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//更改Interping -在Tick下创建一个函数 - ToggleInterping的void函数
	void ToggleInterping();

	//判断完两点位置后，为true则创建Swap函数来交换位置
	void SwapVectors(FVector& VecOne, FVector& VecTwo);


};
