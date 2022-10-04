// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	//创建BoxComponent - 体积重叠后，触发功能
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		class UBoxComponent* TriggerBox;

	//角色踏上去的开关
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		class UStaticMeshComponent* FloorSwitch;

	//当踩踏地板开关时，门将打开
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		UStaticMeshComponent* Door;

	//门的初始位置
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
		FVector InitialDoorLocation;

	//地面开关的初始位置
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
		FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		float SwitchTime;

	//创建一个布尔，表示角色站在开关上
	bool bCharacterOnSwitch;

	void CloseDoor();




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void LowerFloorSwitch();


	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
		void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
		void UpdateFloorSwitchLocation(float Z);



};
