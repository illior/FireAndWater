// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWBaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AFAWBaseCharacter::AFAWBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	JumpMaxHoldTime = 0.2f;
	JumpMaxCount = 2;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));

	// SpringArmComponet
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 600.0f;
	SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 50.0f);
	SpringArmComponent->TargetOffset = FVector(0.0f, 0.0f, 80.0f);

	SpringArmComponent->bUsePawnControlRotation = true;

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 40.0f;
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 30.0f;

	// Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 620.0f;
}

void AFAWBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFAWBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFAWBaseCharacter::InputMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFAWBaseCharacter::InputLook);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputStartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFAWBaseCharacter::InputStopJump);
	}
}

void AFAWBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AFAWBaseCharacter::InputMove(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

	FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
	FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AFAWBaseCharacter::InputLook(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AFAWBaseCharacter::InputStartJump(const FInputActionValue& Value)
{
	Jump();
}

void AFAWBaseCharacter::InputStopJump(const FInputActionValue& Value)
{
	StopJumping();
}