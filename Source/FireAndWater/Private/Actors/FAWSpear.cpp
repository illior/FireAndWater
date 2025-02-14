// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/FAWSpear.h"

#include "Components\StaticMeshComponent.h"
#include "Components\CapsuleComponent.h"
#include "GameFramework\ProjectileMovementComponent.h"

#include "Components/FAWHealthComponent.h"
#include "Engine\DamageEvents.h"
#include "Curves\CurveFloat.h"

AFAWSpear::AFAWSpear()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(MeshComponent);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComponent->SetupAttachment(MeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
}

void AFAWSpear::LaunchAlongTrajectory(FVector InTargetLocation, UCurveFloat* InTrajectory)
{
	if (InTrajectory == nullptr)
	{
		return;
	}

	Trajectory = InTrajectory;
	Trajectory->GetTimeRange(TrajectoryMinTime, TrajectoryMaxTime);

	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();

	TargetLocation = InTargetLocation;
	TimeFromLaunch = 0.0f;

	bLaunched = true;

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &AFAWSpear::FlightAlongTrajectory);
	GetWorldTimerManager().SetTimer(TrajectoryHandle, Delegate, 0.001f, FTimerManagerTimerParameters{.bLoop = true, .bMaxOncePerFrame = true, .FirstDelay = 0.001f});
}

void AFAWSpear::LaunchForward(float InitialSpeed, float MaxSpeed, float GravityScale)
{
	if (InitialSpeed <= 0 || MaxSpeed <= 0)
	{
		return;
	}

	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = GravityScale;

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;

	bLaunched = true;
}

void AFAWSpear::Taken()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnTaken.Broadcast(this);
}

void AFAWSpear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFAWSpear::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor == nullptr || OtherActor->IsA(GetClass()) || OtherActor == GetOwner())
	{
		return;
	}

	if (!bLaunched && bTakeable)
	{
		if (OtherActor->IsA<APawn>())
		{
			Taken();
		}
	}
	else if (bLaunched)
	{
		if (OtherActor->GetComponentByClass<UFAWHealthComponent>() != nullptr)
		{
			GetWorldTimerManager().ClearTimer(TrajectoryHandle);

			FDamageEvent DamageEvent = FDamageEvent();
			OtherActor->TakeDamage(50.0f, DamageEvent, nullptr, GetOwner());

			Destroy();
			return;
		}

		bLaunched = false;
		GetWorldTimerManager().ClearTimer(TrajectoryHandle);

		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		AttachToActor(OtherActor, TransformRules);
		SetOwner(nullptr);

		if (!bTakeable)
		{
			SetLifeSpan(3.0f);
		}
	}
}

void AFAWSpear::FlightAlongTrajectory()
{
	if (Trajectory == nullptr)
	{
		return;
	}

	if (TimeFromLaunch == TrajectoryMaxTime)
	{
		ProjectileMovementComponent->InitialSpeed = 1000.0f;
		ProjectileMovementComponent->MaxSpeed = 1000.0f;

		ProjectileMovementComponent->Velocity = GetActorForwardVector() * 1000.0f;

		GetWorldTimerManager().ClearTimer(TrajectoryHandle);
		return;
	}

	float DeltaTime = GetWorld() != nullptr ? GetWorld()->DeltaTimeSeconds : 0.0f;
	TimeFromLaunch = FMath::Clamp(TimeFromLaunch + DeltaTime, 0.0f, TrajectoryMaxTime);

	FVector FirstLocation = GetExpectedPosition(1);
	FVector SecondLocation = GetExpectedPosition(2);

	FRotator Rotation = FMath::IsNearlyEqual(TimeFromLaunch, TrajectoryMaxTime, DeltaTime) ? GetActorRotation() : FRotationMatrix::MakeFromX(SecondLocation - FirstLocation).Rotator();

	SetActorLocationAndRotation(FirstLocation, Rotation);
}

FVector AFAWSpear::GetExpectedPosition(int32 Frame)
{
	FVector NextPosition;

	float DeltaTime = GetWorld() != nullptr ? GetWorld()->DeltaTimeSeconds : 0.0f;
	FVector CurrentLocation = GetActorLocation();

	NextPosition.X = (( - (InitialLocation.X - TargetLocation.X)) / TrajectoryMaxTime * DeltaTime * Frame) + CurrentLocation.X;

	NextPosition.Y = (( - (InitialLocation.Y - TargetLocation.Y)) / TrajectoryMaxTime * DeltaTime * Frame) + CurrentLocation.Y;
	
	FVector ExpectedForwardLocation = InitialLocation +
		(InitialRotation.Vector() *
			(FVector::Distance(InitialLocation, TargetLocation) *
				(TimeFromLaunch + DeltaTime * (Frame - 1))));

	NextPosition.Z = FMath::Lerp(TargetLocation.Z, ExpectedForwardLocation.Z, Trajectory->GetFloatValue(TimeFromLaunch + DeltaTime * (Frame - 1)));

	return NextPosition;
}

void AFAWSpear::BeginPlay()
{
	Super::BeginPlay();
	
}
