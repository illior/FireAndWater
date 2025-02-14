// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/FAWCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

namespace FAWCharacter
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("FAWCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};

const FFAWCharacterGroundInfo& UFAWCharacterMovementComponent::GetGroundInfo()
{
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - FAWCharacter::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = FAWCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void UFAWCharacterMovementComponent::ForceSprint()
{
	bSprint = true;
	TimeOnMaxSpeed = MaxTimeToSprint;
}

void UFAWCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleSprint(DeltaTime);
}

void UFAWCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFAWCharacterMovementComponent, bSprint);
	DOREPLIFETIME(UFAWCharacterMovementComponent, TimeOnMaxSpeed);
}

float UFAWCharacterMovementComponent::GetMaxSpeed() const
{
	if (bSprint)
	{
		return MaxSprintSpeed;
	}

	return Super::GetMaxSpeed();
}

void UFAWCharacterMovementComponent::HandleSprint(const float& DeltaTime)
{
	FVector CurrentVelocity = Velocity;
	CurrentVelocity.Z = 0.0f;
	float CurrentSpeed = CurrentVelocity.Length();

	if (bSprint)
	{
		if (CurrentSpeed > (MaxWalkSpeed - SpeedTolerance))
		{
			TimeOnMaxSpeed = FMath::Min(TimeOnMaxSpeed + DeltaTime, MaxTimeToSprint);
		}
		else
		{
			TimeOnMaxSpeed = FMath::Max(TimeOnMaxSpeed - DeltaTime, 0.0f);
		}

		if (TimeOnMaxSpeed < TimeToStartSprint)
		{
			bSprint = false;
		}
	}
	else
	{
		if (MovementMode == EMovementMode::MOVE_Walking && CurrentSpeed > (MaxWalkSpeed - SpeedTolerance))
		{
			TimeOnMaxSpeed += DeltaTime;
		}
		else
		{
			TimeOnMaxSpeed = 0.0f;
		}

		if (TimeOnMaxSpeed >= TimeToStartSprint)
		{
			bSprint = true;
		}
	}
}
