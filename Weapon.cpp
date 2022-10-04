// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SKeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SKeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


//����Weapon���캯��
AWeapon::AWeapon()
{
	//����SkeletalMesh
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMesh"));
	//���ӵ������
	SkeletalMesh->SetupAttachment(GetRootComponent());

	//������������ӵ���
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	//Ϊ���������ײԤ��
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);



	//bWeaponParticles
	bWeaponParticles = false;

	//WeaponState����Ĭ��ֵ
	WeaponState = EWeaponState::EWS_Pickup;

	//ΪDamage����Ĭ��ֵ -�����˺�ֵ
	Damage = 75.f;
}

//BeginPlay
void AWeapon::BeginPlay()
{
	//���ø���
	Super::BeginPlay();

	//����CombatCollisionʵ�ֹ���
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);



}

//����OnOverlapBegin����
void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//����Super�����ø����汾
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}

}

//����OnOverlapEnd����
void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}



//ʵ�ֽ�ɫװ������  - Equip����
void AWeapon::Equip(AMain* Char)
{
	if (Char)
	{
		//����Instigator
		SetInstigator(Char->GetController());

		//�������������Խ�ɫ�������֮���Ӱ�죬����Ӱ����ҽǶȵķŴ�
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//��CollisionResponseToChannel���óɱ�Pawn���� -��������Pawn��ײ
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		//ֹͣ����ģ��
		SkeletalMesh->SetSimulatePhysics(false);

		//��ȡRightHandSocket
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		//�ж�RightHandSocket��Ч��
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			//����Weapon��װ������ʱ����Ҫratate
			bRotate = false;

			//��EquippedWeapon�趨���ض�������ʵ��
			Char->SetEquippedWeapon(this);
			//��SetActiveOverlappingOverItem��Ϊnullptr
			Char->SetActiveOverlappingItem(nullptr);
		}
		//�ж�EquipSound������UGmaeplayStatics
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		//�ж����ʰȡ�������������ж�Weapon Particles��true����false
		if (!bWeaponParticles)
		{
			IdelParticlesComponent->Deactivate();
		}
	}
}


//����ս����ײOnOverlapBegin����
void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				//��Socketλ�����ɷ�����
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					//��������ϵͳ
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			//����ApplyDamage
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

//����ս����ײOnOverlapEnd����
void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


//Activate/DeactivateCollision����
void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
