// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/FAWAnimInstance.h"

#include "GameFramework/Character.h"
#include "Components/FAWCharacterMovementComponent.h"

void UFAWAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (Character == nullptr)
	{
		return;
	}

	UFAWCharacterMovementComponent* CharMoveComp = CastChecked<UFAWCharacterMovementComponent>(Character->GetCharacterMovement());
	const FFAWCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}
