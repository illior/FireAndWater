// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FAWTempleFloor.generated.h"

UCLASS()
class FIREANDWATER_API AFAWTempleFloor : public AActor
{
	GENERATED_BODY()
	

public:	
	AFAWTempleFloor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void MoveCells();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Release();

	void MoveDeathLevel();

	virtual void OnConstruction(const FTransform& Transform) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (ClampMin = "1", ClampMax = "20"))
	int32 Rows = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (ClampMin = "1", ClampMax = "20"))
	int32 Colums = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (ClampMin = "1", ClampMax = "1000"))
	float CellSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<UStaticMesh> CellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CellScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UpperPoints = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlateSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlateHeight = 250.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bActive = false;

	UPROPERTY(BlueprintReadOnly)
	bool bActiveDeathLevel = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<UStaticMeshComponent*> Cells;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnReplicatedPlatesChanged)
	TArray<FIntPoint> ReplicatedPlates;

	TArray<FIntPoint> MovingPlates;

	float InitialPositionZ;
	float TargetPositionZ;

	UFUNCTION()
	void OnReplicatedPlatesChanged();

	UFUNCTION()
	void BeginOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	void MakeGrid();

	bool CheckDistanceByIndex(const int32& Index, const float& Distance);

	UStaticMeshComponent* GetCell(const FIntPoint& Position);
	UStaticMeshComponent* GetCell(const int32& Index);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DamageComponent;

};
