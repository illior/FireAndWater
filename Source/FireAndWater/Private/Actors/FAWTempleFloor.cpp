// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/FAWTempleFloor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine\DamageEvents.h"
#include "Net/UnrealNetwork.h"

AFAWTempleFloor::AFAWTempleFloor()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(SceneComponent);

	DamageComponent = CreateDefaultSubobject<UStaticMeshComponent>("Damage");
	DamageComponent->SetupAttachment(GetRootComponent());

	DamageComponent->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
}

void AFAWTempleFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive && !(bActiveDeathLevel && TargetPositionZ == InitialPositionZ))
	{
		if (MovingPlates.Num() > 0)
		{
			for (int32 i = MovingPlates.Num() - 1; i > -1; i--)
			{
				UStaticMeshComponent* Component = GetCell(MovingPlates[i].X);
				if (Component == nullptr)
				{
					continue;
				}

				FVector CurrentLocation = Component->GetRelativeLocation();

				if (CurrentLocation.Z < MovingPlates[i].Y)
				{
					CurrentLocation.Z = FMath::Min(CurrentLocation.Z + DeltaTime * PlateSpeed, MovingPlates[i].Y);
				}
				else
				{
					CurrentLocation.Z = FMath::Max(CurrentLocation.Z - DeltaTime * PlateSpeed, MovingPlates[i].Y);
				}

				Component->SetRelativeLocation(CurrentLocation);
				if (FMath::IsNearlyEqual(CurrentLocation.Z, MovingPlates[i].Y))
				{
					MovingPlates.RemoveAt(i);
				}
			}
		}
		else
		{
			bActive = false;
		}
	}

	if (bActiveDeathLevel)
	{
		FVector CurrentLocation = DamageComponent->GetRelativeLocation();

		if (CurrentLocation.Z < TargetPositionZ)
		{
			CurrentLocation.Z = FMath::Min(CurrentLocation.Z + DeltaTime * PlateSpeed, TargetPositionZ);
		}
		else
		{
			CurrentLocation.Z = FMath::Max(CurrentLocation.Z - DeltaTime * PlateSpeed, TargetPositionZ);
		}

		DamageComponent->SetRelativeLocation(CurrentLocation);
		if (FMath::IsNearlyEqual(CurrentLocation.Z, TargetPositionZ))
		{
			bActiveDeathLevel = false;
		}
	}
}

void AFAWTempleFloor::MoveCells()
{
	MovingPlates.Empty();

	int32 i = 0;
	while (i < UpperPoints)
	{
		int32 RandomIndex = FMath::RandRange(0, Cells.Num() - 1);
		if (CheckDistanceByIndex(RandomIndex, 2.9f))
		{
			continue;
		}

		FIntPoint PlateData;
		PlateData.X = RandomIndex;
		PlateData.Y = PlateHeight * 2;

		MovingPlates.Add(PlateData);
		i++;
	}

	for (i = 0; i < Cells.Num(); i++)
	{
		if (CheckDistanceByIndex(i, 2.9f))
		{
			continue;
		}

		FIntPoint PlateData;
		PlateData.X = i;
		PlateData.Y = PlateHeight * -1.0f;

		MovingPlates.Add(PlateData);
	}

	for (i = 0; i < UpperPoints; i++)
	{
		FIntPoint Position;
		Position.X = MovingPlates[i].X % Colums;
		Position.Y = MovingPlates[i].X / Colums;
		for (int32 j = Position.X - 1; j <= Position.X + 1; j++)
		{
			for (int32 k = Position.Y - 1; k <= Position.Y + 1; k++)
			{
				if (j < 0 || j == Colums || k < 0 || k == Rows)
				{
					continue;
				}

				int32 Index = j + k * Colums;
				if (Cells.IsValidIndex(Index) && Index != MovingPlates[i].X)
				{
					FIntPoint PlateData;
					PlateData.X = Index;
					PlateData.Y = PlateHeight;

					MovingPlates.Add(PlateData);
				}
			}
		}
	}

	ReplicatedPlates = MovingPlates;

	FTimerHandle Timer;
	float TimeToStart = (PlateHeight * 1.5f) / PlateSpeed;
	GetWorldTimerManager().SetTimer(Timer, this, &AFAWTempleFloor::MoveDeathLevel, TimeToStart, false);

	bActive = true;
}

void AFAWTempleFloor::Release()
{
	MovingPlates.Empty();

	for (int32 i = 0; i < Cells.Num(); i++)
	{
		FIntPoint PlateData;
		PlateData.X = i;
		PlateData.Y = 0;

		MovingPlates.Add(PlateData);
	}

	ReplicatedPlates = MovingPlates;

	TargetPositionZ = InitialPositionZ;

	bActive = true;
	bActiveDeathLevel = true;
}

void AFAWTempleFloor::MoveDeathLevel()
{
	bActiveDeathLevel = true;
	TargetPositionZ = InitialPositionZ + PlateHeight * 3;
}

void AFAWTempleFloor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CellMesh != nullptr)
	{
		MakeGrid();
	}
}

void AFAWTempleFloor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFAWTempleFloor, ReplicatedPlates);
}

void AFAWTempleFloor::OnReplicatedPlatesChanged()
{
	MovingPlates = ReplicatedPlates;

	bActive = true;

	if (MovingPlates.Num() == Cells.Num())
	{
		TargetPositionZ = InitialPositionZ;
		bActiveDeathLevel = true;
	}
	else
	{
		FTimerHandle Timer;
		float TimeToStart = (PlateHeight * 1.5f) / PlateSpeed;
		GetWorldTimerManager().SetTimer(Timer, this, &AFAWTempleFloor::MoveDeathLevel, TimeToStart, false);
	}
}

void AFAWTempleFloor::BeginOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	FDamageEvent DamageEvent = FDamageEvent();
	OtherActor->TakeDamage(1000.0f, DamageEvent, nullptr, this);
}

void AFAWTempleFloor::BeginPlay()
{
	Super::BeginPlay();
	
	check(DamageComponent);
	DamageComponent->OnComponentBeginOverlap.AddDynamic(this, &AFAWTempleFloor::BeginOverlapDamage);

	InitialPositionZ = DamageComponent->GetRelativeLocation().Z;
}

void AFAWTempleFloor::MakeGrid()
{
	if (Cells.Num() > 0)
	{
		for (UStaticMeshComponent* Component : Cells)
		{
			if (Component != nullptr)
			{
				Component->DestroyComponent();
			}
		}

		Cells.Empty();
	}

	for (int32 i = 0; i < Rows; i++)
	{
		for (int32 j = 0; j < Colums; j++)
		{
			UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
			NewComponent->RegisterComponent();

			NewComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

			NewComponent->SetRelativeLocation(FVector(CellSize * i, CellSize * j, 0.0f));
			NewComponent->SetRelativeScale3D(CellScale);
			NewComponent->SetStaticMesh(CellMesh);
			NewComponent->SetCollisionProfileName(FName(TEXT("BlockAllDynamic")));

			NewComponent->SetGenerateOverlapEvents(true);

			Cells.Add(NewComponent);
		}
	}
}

bool AFAWTempleFloor::CheckDistanceByIndex(const int32& Index, const float& Distance)
{
	if (MovingPlates.Num() == 0)
	{
		return false;
	}

	FIntPoint IndexPosition;
	IndexPosition.X = Index % Colums;
	IndexPosition.Y = Index / Colums;

	for (int32 i = 0; i < FMath::Min(UpperPoints, MovingPlates.Num()); i++)
	{
		FIntPoint ItemPosition;
		ItemPosition.X = MovingPlates[i].X % Colums;
		ItemPosition.Y = MovingPlates[i].X / Colums;

		float Length = FMath::Sqrt((float)FMath::Square(IndexPosition.X - ItemPosition.X) + (float)FMath::Square(IndexPosition.Y - ItemPosition.Y));

		if (Length < Distance)
		{
			return true;
		}
	}

	return false;
}

UStaticMeshComponent* AFAWTempleFloor::GetCell(const FIntPoint& Position)
{
	return GetCell(Position.X + Position.Y * Colums);
}

UStaticMeshComponent* AFAWTempleFloor::GetCell(const int32& Index)
{
	if (Cells.IsValidIndex(Index))
	{
		return Cells[Index];
	}

	return nullptr;
}
