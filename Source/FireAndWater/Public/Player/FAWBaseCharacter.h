// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "FAWBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;

class AFAWInteractableActor;

class UMaterialParameterCollection;
class UDamageType;

class UInputAction;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS()
class FIREANDWATER_API AFAWBaseCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	AFAWBaseCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	void AddCheckPoint(AActor* InActor);

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	//~ End AActor Interface

	//~ Begin APawn Interface.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End APawn Interface
protected:
	//~ Begin Input Actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;
	//~ End Input Actions

	//~ Begin replicated variables.
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Character|Interaction")
	TWeakObjectPtr<AFAWInteractableActor> InteractActor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRepIsDead, Category = "Death")
	bool IsDead = false;
	//~ End replicated variables

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<AActor*> CheckPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	TSubclassOf<UDamageType> ResistDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	TObjectPtr<UCurveFloat> DissolveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	TObjectPtr<UMaterialParameterCollection> DissolveParameterCollection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	FName DissolveVFXMaskName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	FName DissolveParameterCollectionMaskName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	FName VFXColorName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	FLinearColor VFXColor;

	FTimeline DisappearTimeline;

	bool CanMove = true;

	//~ Begin RPC functions.
	UFUNCTION()
	void OnRepIsDead();

	UFUNCTION(Server, Unreliable)
	virtual void  Server_StartInteract();
	virtual void  Server_StartInteract_Implementation();
	UFUNCTION(Server, Unreliable)
	virtual void  Server_StopInteract();
	virtual void  Server_StopInteract_Implementation();
	//~ End RPC functions

	void OnDissolve(float Value);

	void OnDissolveComplete();

	virtual void StartCanInteract(AFAWInteractableActor* InteractableActor);
	virtual void StopCanInteract(AFAWInteractableActor* InteractableActor);

	virtual void StartInteract();
	virtual void StopInteract();

private:
	void InputMove(const FInputActionValue& Value);
	void InputLook (const FInputActionValue& Value);

	void InputStartJump(const FInputActionValue& Value);
	void InputStopJump(const FInputActionValue& Value);

	void InputStartInteract(const FInputActionValue& Value);
	void InputStopInteract(const FInputActionValue& Value);

};
