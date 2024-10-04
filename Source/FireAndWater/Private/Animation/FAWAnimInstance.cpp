// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/FAWAnimInstance.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

void UFAWAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (Character == nullptr)
	{
		return;
	}

	// Temp solution, fix later
	const float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	const FVector TraceStart(Character->GetActorLocation());
	const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - 100000.0f - CapsuleHalfHeight));
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	GroundDistance = FMath::Max(HitResult.Distance - CapsuleHalfHeight, 0.0f);
}
