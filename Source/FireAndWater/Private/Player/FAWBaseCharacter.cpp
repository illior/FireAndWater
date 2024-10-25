// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWBaseCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"

#include "Materials\MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "EngineUtils.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/PlayerStart.h"

#include "Interaction/FAWInteractableActor.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AFAWBaseCharacter::AFAWBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

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

	// Niagara
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(GetMesh());
	NiagaraComponent->bAutoActivate = false;

	// Setup Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 620.0f;
	GetCharacterMovement()->AirControl = 0.4f;
}

void AFAWBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFAWBaseCharacter, InteractActor);
	DOREPLIFETIME(AFAWBaseCharacter, IsDead);
}

void AFAWBaseCharacter::AddCheckPoint(AActor* InActor)
{
	if (InActor != nullptr && !CheckPoints.Contains(InActor))
	{
		CheckPoints.Add(InActor);
	}
}

void AFAWBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	const ENetMode NetMode = GetNetMode();
	if (DissolveCurve != nullptr)
	{
		FOnTimelineFloatStatic OnTimelineCallback;
		OnTimelineCallback.BindUObject(this, &AFAWBaseCharacter::OnDissolve);
		DisappearTimeline.AddInterpFloat(DissolveCurve, OnTimelineCallback);

		if (HasAuthority())
		{
			FOnTimelineEventStatic OnTimelineFinishedCallback;
			OnTimelineFinishedCallback.BindUObject(this, &AFAWBaseCharacter::OnDissolveComplete);
			DisappearTimeline.SetTimelineFinishedFunc(OnTimelineFinishedCallback);
		}
	}

	NiagaraComponent->SetVariableLinearColor(VFXColorName, VFXColor);
}

void AFAWBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DisappearTimeline.TickTimeline(DeltaTime);
}

float AFAWBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageEvent.DamageTypeClass != nullptr)
	{
		if (DamageEvent.DamageTypeClass->IsChildOf(ResistDamageType))
		{
			return ActualDamage;
		}
	}

	IsDead = true;

	DisappearTimeline.PlayFromStart();

	if (!IsNetMode(NM_DedicatedServer))
	{
		NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, 1.0f);
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, 1.0f);
		NiagaraComponent->Activate();
	}

	return ActualDamage;
}

void AFAWBaseCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AFAWInteractableActor* InteractableActor = Cast<AFAWInteractableActor>(OtherActor);
	if (InteractableActor != nullptr && InteractableActor->GetIsEnabled() && HasAuthority())
	{
		StartCanInteract(InteractableActor);
	}
}

void AFAWBaseCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AFAWInteractableActor* InteractableActor = Cast<AFAWInteractableActor>(OtherActor);
	if (InteractableActor != nullptr && InteractableActor->GetIsEnabled() && HasAuthority())
	{
		StopCanInteract(InteractableActor);
	}
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
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputStartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AFAWBaseCharacter::InputStopInteract);
	}
}

void AFAWBaseCharacter::OnRepIsDead()
{
	NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, 1.0f);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, 1.0f);

	if (IsDead)
	{
		NiagaraComponent->Activate();
		DisappearTimeline.PlayFromStart();
	}
	else
	{
		NiagaraComponent->Deactivate();
	}
}

void AFAWBaseCharacter::Server_StartInteract_Implementation()
{
	if (InteractActor.IsValid() && InteractActor->GetInteractedPawn() == this)
	{
		InteractActor->StartInteract();
	}
}

void AFAWBaseCharacter::Server_StopInteract_Implementation()
{
	if (InteractActor.IsValid() && InteractActor->GetInteractedPawn() == this)
	{
		InteractActor->StopInteract();
	}
}

void AFAWBaseCharacter::OnDissolve(float Value)
{
	if (!IsDead || IsNetMode(ENetMode::NM_DedicatedServer))
	{
		return;
	}

	NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, Value);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, Value);
}

void AFAWBaseCharacter::OnDissolveComplete()
{
	IsDead = false;

	if (!IsNetMode(NM_DedicatedServer))
	{
		NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, 1.0f);
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, 1.0f);
		NiagaraComponent->Deactivate();
	}

	FVector RespawnLocation;
	if (CheckPoints.Num() != 0)
	{
		RespawnLocation = CheckPoints[CheckPoints.Num() - 1] != nullptr ? CheckPoints[CheckPoints.Num() - 1]->GetActorLocation() : FVector::ZeroVector;
	}
	else
	{
		for (TActorIterator<AActor> It(GetWorld(), APlayerStart::StaticClass()); It; ++It)
		{
			RespawnLocation = (*It)->GetActorLocation();
			break;
		}
	}

	SetActorLocation(RespawnLocation);
}

void AFAWBaseCharacter::StartCanInteract(AFAWInteractableActor* InteractableActor)
{
	InteractActor = TWeakObjectPtr<AFAWInteractableActor>(InteractableActor);

	InteractableActor->StartCanInteract(this);
}

void AFAWBaseCharacter::StopCanInteract(AFAWInteractableActor* InteractableActor)
{
	InteractActor.Reset();

	InteractableActor->StopCanInteract(this);
}

void AFAWBaseCharacter::StartInteract()
{
	if (InteractActor.IsValid() && InteractActor->GetInteractedPawn() == this)
	{
		CanMove = false;
		InteractActor->StartHold();
	}
}

void AFAWBaseCharacter::StopInteract()
{
	CanMove = true;

	if (InteractActor.IsValid() && InteractActor->GetInteractedPawn() == this)
	{
		InteractActor->StopHold();
	}
}

void AFAWBaseCharacter::InputMove(const FInputActionValue& Value)
{
	if (Controller == nullptr || IsDead || !CanMove)
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
	if (IsDead)
	{
		return;
	}

	Jump();
}

void AFAWBaseCharacter::InputStopJump(const FInputActionValue& Value)
{
	if (IsDead)
	{
		return;
	}

	StopJumping();
}

void AFAWBaseCharacter::InputStartInteract(const FInputActionValue& Value)
{
	if (IsDead)
	{
		return;
	}

	StartInteract();
	Server_StartInteract();
}

void AFAWBaseCharacter::InputStopInteract(const FInputActionValue& Value)
{
	if (IsDead)
	{
		return;
	}

	StopInteract();
	Server_StopInteract();
}
