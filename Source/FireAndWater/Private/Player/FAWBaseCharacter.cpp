// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWBaseCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/FAWCharacterMovementComponent.h"
#include "Components/FAWHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"

#include "Materials\MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "EngineUtils.h"
#include "Engine/DamageEvents.h"
#include "Player/FAWPlayerController.h"
#include "Player/FAWPlayerState.h"

#include "Interaction/FAWInteractableActor.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AFAWBaseCharacter::AFAWBaseCharacter(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<UFAWCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

	// Health
	HealthComponent = CreateDefaultSubobject<UFAWHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);

	// Setup Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 620.0f;
	GetCharacterMovement()->AirControl = 0.4f;
	GetCharacterMovement()->GravityScale = 1.4f;
	GetCharacterMovement()->SetIsReplicated(true);
}

void AFAWBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFAWBaseCharacter, InteractActor);
	DOREPLIFETIME(AFAWBaseCharacter, bDashing);
}

void AFAWBaseCharacter::ResetJumpState()
{
	Super::ResetJumpState();
	/*bPressedJump = false;
	bWasJumping = false;
	JumpKeyHoldTime = 0.0f;
	JumpForceTimeRemaining = 0.0f;

	if (GetCharacterMovement() && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		JumpCurrentCount = 0;
		JumpCurrentCountPreJump = 0;

		LastTimeDashing = 0.0f;
	}*/
}

void AFAWBaseCharacter::AddCheckPoint(AActor* InActor)
{
	AFAWPlayerState* State = GetPlayerState<AFAWPlayerState>();
	if (State != nullptr)
	{
		State->AddCheckPoint(InActor);
	}
}

void AFAWBaseCharacter::ResetJumps()
{
	JumpCurrentCount = 2;
}

void AFAWBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(HealthComponent);
	check(NiagaraComponent);

	HealthComponent->OnDeath.AddDynamic(this, &AFAWBaseCharacter::OnDeath);

	if (DissolveCurve != nullptr)
	{
		FOnTimelineFloatStatic OnTimelineCallback;
		OnTimelineCallback.BindUObject(this, &AFAWBaseCharacter::OnDissolve);
		DisappearTimeline.AddInterpFloat(DissolveCurve, OnTimelineCallback);

		FOnTimelineEventStatic OnTimelineEvent;
		OnTimelineEvent.BindUObject(this, &AFAWBaseCharacter::OnDissolveComplete);
		DisappearTimeline.SetTimelineFinishedFunc(OnTimelineEvent);

		DisappearTimeline.SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	}

	NiagaraComponent->SetVariableLinearColor(VFXColorName, VFXColor);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, 1.0f);
}

void AFAWBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DisappearTimeline.TickTimeline(DeltaTime);
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

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputFire);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputAim);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputDash);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputStartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFAWBaseCharacter::InputStopJump);
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFAWBaseCharacter::InputStartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AFAWBaseCharacter::InputStopInteract);
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

void AFAWBaseCharacter::Multicat_Death_Implementation()
{
	DisappearTimeline.PlayFromStart();

	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

	if (!IsNetMode(NM_DedicatedServer))
	{
		NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, 1.0f);
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, 1.0f);
		NiagaraComponent->Activate();
	}
}

void AFAWBaseCharacter::Server_StartDash_Implementation()
{
	bDashing = true;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	FVector Direction = GetActorForwardVector() * 1200;
	Direction.Z = 0.0f;

	GetCharacterMovement()->AddImpulse(Direction, true);

	FTimerHandle DashTimer;
	GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &AFAWBaseCharacter::Server_EndDash, 0.3f, false);

	OnCharacterDashed();
}

void AFAWBaseCharacter::Server_EndDash_Implementation()
{
	bDashing = false;

	UFAWCharacterMovementComponent* MovementComponent = GetCharacterMovement<UFAWCharacterMovementComponent>();
	if (MovementComponent != nullptr)
	{
		const FFAWCharacterGroundInfo& GroundInfo = MovementComponent->GetGroundInfo();

		if (GroundInfo.GroundDistance < 100.0f)
		{
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

			MovementComponent->ForceSprint();
		}
		else
		{
			MovementComponent->Velocity = MovementComponent->Velocity / 2;
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
}

void AFAWBaseCharacter::OnDeath()
{
	Multicat_Death();

	AFAWPlayerController* PlayerController = GetController<AFAWPlayerController>();
	if (PlayerController != nullptr)
	{
		PlayerController->RestartWithDelay(DisappearTimeline.GetTimelineLength() + 0.1f);
	}
}

void AFAWBaseCharacter::OnDissolve(float Value)
{
	if (IsNetMode(ENetMode::NM_DedicatedServer))
	{
		return;
	}

	NiagaraComponent->SetFloatParameter(DissolveVFXMaskName, Value);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), DissolveParameterCollection, DissolveParameterCollectionMaskName, Value);
}

void AFAWBaseCharacter::OnDissolveComplete()
{
	Destroy();
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
		InteractActor->StartHold();
	}
}

void AFAWBaseCharacter::StopInteract()
{
	if (InteractActor.IsValid() && InteractActor->GetInteractedPawn() == this)
	{
		InteractActor->StopHold();
	}
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

void AFAWBaseCharacter::InputFire(const FInputActionValue& Value)
{
}

void AFAWBaseCharacter::InputAim(const FInputActionValue& Value)
{
}

void AFAWBaseCharacter::InputDash(const FInputActionValue& Value)
{
	if (!bDashing && LastTimeDashing < GetWorld()->GetTimeSeconds())
	{
		LastTimeDashing = GetWorld()->GetTimeSeconds() + DashCooldown;

		Server_StartDash();
	}
}

void AFAWBaseCharacter::InputStartJump(const FInputActionValue& Value)
{
	Jump();
}

void AFAWBaseCharacter::InputStopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void AFAWBaseCharacter::InputStartInteract(const FInputActionValue& Value)
{
	StartInteract();
	Server_StartInteract();
}

void AFAWBaseCharacter::InputStopInteract(const FInputActionValue& Value)
{
	StopInteract();
	Server_StopInteract();
}
