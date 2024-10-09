// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FAWBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class AFAWInteractableActor;

class UInputAction;
struct FInputActionValue;

UCLASS()
class FIREANDWATER_API AFAWBaseCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	AFAWBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Character|Interaction")
	TWeakObjectPtr<AFAWInteractableActor> InteractActor;

	virtual void  StartCanInteract(AFAWInteractableActor* InteractableActor);
	virtual void  StopCanInteract(AFAWInteractableActor* InteractableActor);

	UFUNCTION(Server, Unreliable)
	virtual void  Server_StartInteract();
	UFUNCTION(Server, Unreliable)
	virtual void  Server_StopInteract();

	virtual void StartInteract();
	virtual void StopInteract();

	virtual void BeginPlay() override;

private:
	void InputMove(const FInputActionValue& Value);
	void InputLook (const FInputActionValue& Value);

	void InputStartJump(const FInputActionValue& Value);
	void InputStopJump(const FInputActionValue& Value);

	void InputStartInteract(const FInputActionValue& Value);
	void InputStopInteract(const FInputActionValue& Value);

};
