// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/FAWInteractableActor.h"

#include "UI/Components/FAWInteractWidget.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

AFAWInteractableActor::AFAWInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(SceneComponent);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	CollisionComponent->SetupAttachment(SceneComponent);

	CollisionComponent->SetCapsuleSize(200.0f, 200.0f);
	CollisionComponent->bHiddenInGame = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComponent->SetupAttachment(SceneComponent);

	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetVisibility(false);
}

void AFAWInteractableActor::StartCanInteract(APawn* Pawn)
{
	if (Pawn == nullptr || InteractedPawn.Get() != nullptr)
	{
		return;
	}

	Multicast_StartCanInteract(Pawn);
}

void AFAWInteractableActor::StopCanInteract(APawn* Pawn)
{
	if (Pawn == nullptr || InteractedPawn.Get() != Pawn)
	{
		return;
	}

	Multicast_StopCanInteract(Pawn);
}

void AFAWInteractableActor::StartInteract()
{
	if (ShouldHold)
	{
		GetWorldTimerManager().SetTimer(HoldTimer, this, &AFAWInteractableActor::Complete, HoldTime, false);
	}
	else
	{
		Multicast_SetEnabled(false);
		if (IsReusable)
		{
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "Multicast_SetEnabled", true);

			GetWorldTimerManager().SetTimer(CooldownTimer, Delegate, CooldownTime, false);
		}
	}

	ReceiveStartInteract();
}

void AFAWInteractableActor::StopInteract()
{
	if (GetWorldTimerManager().IsTimerActive(HoldTimer))
	{
		GetWorldTimerManager().ClearTimer(HoldTimer);
	}

	ReceiveStopInteract();
}

void AFAWInteractableActor::StartHold()
{
	if (ShouldHold)
	{
		UFAWInteractWidget* InteractWidget = Cast<UFAWInteractWidget>(WidgetComponent->GetUserWidgetObject());
		if (InteractWidget != nullptr)
		{
			InteractWidget->StartHold(HoldTime);
		}
	}
}

void AFAWInteractableActor::StopHold()
{
	if (ShouldHold)
	{
		UFAWInteractWidget* InteractWidget = Cast<UFAWInteractWidget>(WidgetComponent->GetUserWidgetObject());
		if (InteractWidget != nullptr)
		{
			InteractWidget->StopHold();
		}
	}
}

void AFAWInteractableActor::Multicast_SetEnabled_Implementation(bool NewValue)
{
	if (NewValue != IsEnabled)
	{
		NewValue ? Enable() : Disable();
	}
}

void AFAWInteractableActor::Multicast_StartCanInteract_Implementation(APawn* Pawn)
{
	InteractedPawn = TWeakObjectPtr<APawn>(Pawn);

	AController* PlayerController = Pawn->GetController();
	if (PlayerController != nullptr && PlayerController->IsLocalPlayerController())
	{
		ShowWidget();
	}
}

void AFAWInteractableActor::Multicast_StopCanInteract_Implementation(APawn* Pawn)
{
	InteractedPawn.Reset();

	AController* PlayerController = Pawn->GetController();
	if (PlayerController != nullptr && PlayerController->IsLocalPlayerController())
	{
		HideWidget();
	}
}

void AFAWInteractableActor::Complete()
{
	Multicast_SetEnabled(false);
	if (IsReusable)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "Multicast_SetEnabled", true);

		GetWorldTimerManager().SetTimer(CooldownTimer, Delegate, CooldownTime, false);
	}

	ReceiveComplete();
}

void AFAWInteractableActor::ShowWidget()
{
	WidgetComponent->SetVisibility(true);
	GetWorldTimerManager().ClearTimer(HideWidgetTimer);

	FTimerHandle TimerHandler;
	GetWorldTimerManager().SetTimer(TimerHandler, [this]() {
		UFAWInteractWidget* InteractWidget = Cast<UFAWInteractWidget>(WidgetComponent->GetUserWidgetObject());
		if (InteractWidget != nullptr)
		{
			InteractWidget->StartShow();
		}
	}, 0.01f, false);
}

void AFAWInteractableActor::HideWidget()
{
	if (!WidgetComponent->IsVisible())
	{
		return;
	}

	UFAWInteractWidget* InteractWidget = Cast<UFAWInteractWidget>(WidgetComponent->GetUserWidgetObject());
	if (InteractWidget != nullptr)
	{
		float TimeToHide = InteractWidget->StartHide();

		FTimerDelegate Delegate;
		Delegate.BindUFunction(WidgetComponent, "SetVisibility", false);

		GetWorldTimerManager().SetTimer(HideWidgetTimer, Delegate, TimeToHide, false);
	}
}

void AFAWInteractableActor::Enable()
{
	IsEnabled = true;

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->bHiddenInGame = false;

	ReceiveEnable();
}

void AFAWInteractableActor::Disable()
{
	InteractedPawn.Reset();

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->bHiddenInGame = true;

	IsEnabled = false;

	HideWidget();

	ReceiveDisable();
}

void AFAWInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	IsEnabled ? Enable() : Disable();
}