// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FAWSpear.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpearTakenSignature, AFAWSpear*, SpearRef);

class UStaticMeshComponent;
class UCapsuleComponent;
class UProjectileMovementComponent;

class UCurveFloat;

UCLASS()
class FIREANDWATER_API AFAWSpear : public AActor
{
	GENERATED_BODY()


public:
	AFAWSpear();

	UPROPERTY(BlueprintAssignable)
	FSpearTakenSignature OnTaken;

	UFUNCTION(BlueprintCallable)
	void LaunchAlongTrajectory(FVector InTargetLocation, UCurveFloat* InTrajectory);

	UFUNCTION(BlueprintCallable)
	void LaunchForward(float InitialSpeed, float MaxSpeed, float GravityScale = 1.0f);

	void Taken();

	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTakeable = false;

	UPROPERTY(BlueprintReadOnly)
	bool bLaunched;

	FVector InitialLocation;
	FRotator InitialRotation;

	FVector TargetLocation;

	float TimeFromLaunch;

	TObjectPtr<UCurveFloat> Trajectory;
	float TrajectoryMinTime;
	float TrajectoryMaxTime;

	FTimerHandle TrajectoryHandle;

	void FlightAlongTrajectory();

	FVector GetExpectedPosition(int32 Frame = 1);

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
};
