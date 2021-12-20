
#include "Queen.h"

#include "DrawDebugHelpers.h"
#include "QueenSpawner.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AQueen::AQueen()
{
	// creating components and basic initiation.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	MoveCount = 0;
}


void AQueen::BeginPlay()
{
	Super::BeginPlay();

}


bool AQueen::TraceInEveryDirection()const
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
		return false;;

	// setting up trace params.
	FHitResult HitResult;
	FVector StartLocation;
	FVector EndLocation;
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams CollisionQueryParams;

	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	CollisionQueryParams.AddIgnoredActor(this);

	StartLocation.Set(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);

	//UE_LOG(LogTemp, Warning, TEXT("Channel 1 : %s"), )

	// trace in 8 direction from the current Queen's location.
	for (size_t i = 0; i < 8; ++i)
	{
		switch (i)
		{
		case 0:
			EndLocation.Set(StartLocation.X + 3200.f, StartLocation.Y, StartLocation.Z);
			break;
		case 1:
			EndLocation.Set(StartLocation.X - 3200.f, StartLocation.Y, StartLocation.Z);
			break;
		case 2:
			EndLocation.Set(StartLocation.X, StartLocation.Y + 3200.f, StartLocation.Z);
			break;
		case 3:
			EndLocation.Set(StartLocation.X, StartLocation.Y - 3200.f, StartLocation.Z);
			break;
		case 4:
			EndLocation.Set(StartLocation.X + 3200.f, StartLocation.Y + 3200.f, StartLocation.Z);
			break;
		case 5:
			EndLocation.Set(StartLocation.X + 3200.f, StartLocation.Y - 3200.f, StartLocation.Z);
			break;
		case 6:
			EndLocation.Set(StartLocation.X - 3200.f, StartLocation.Y + 3200.f, StartLocation.Z);
			break;
		case 7:
			EndLocation.Set(StartLocation.X - 3200.f, StartLocation.Y - 3200.f, StartLocation.Z);
			break;
		}

		// debug helper : visualize the trace lines.
		DrawDebugLine(World, StartLocation, EndLocation, FColor::Blue, false, 1.f, 0, 10.f);


		// if there is a conflict in any of the 8 directions then the spot(which Queen is currently is in) is not available.
		if (World->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, ObjectQueryParams, CollisionQueryParams))
		{
			//UE_LOG(LogTemp, Warning, TEXT("HitResult : %s"), *HitResult.GetActor()->GetName())
			return false;
		}
	}

	// if there is no conflict then the spot is available.
	return true;
}

void AQueen::VerifyQueen()
{
	bIsSafe = TraceInEveryDirection();

	//UE_LOG(LogTemp, Warning, TEXT("bIsSafe %i"), bIsSafe)
	
	AQueenSpawner* QueenSpawner = Cast<AQueenSpawner>(GetOwner());
	if (!ensure(QueenSpawner != nullptr))
		return;

	// 
	if (bIsSafe)
	{
		QueenSpawner->CallSpawnQueen();
	}
	else
	{
		// then move in Row by 400 (remember 400 is based on the floor size) 
		FTimerHandle SpawnTimerHandle;
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AQueen::MoveInRow, 1.f, false, 1.f);
	}
}


void AQueen::MoveInRow()
{
	// if the Queen is out of chessboard, then there is no spot in this row (means the prev Queens's are not in right spots)
	++MoveCount;
	if (MoveCount >= 8)
	{
		//This Object should be destroyed
		AQueenSpawner* QueenSpawner = Cast<AQueenSpawner>(GetOwner());
		if (!ensure(QueenSpawner != nullptr))
			return;

		QueenSpawner->QueenDestroyed(this);
		return;     
	}

	// Move the Queen one block down and verify it's new spot.
	NewLocation = FVector(GetActorLocation().X, GetActorLocation().Y + 400, GetActorLocation().Z);
	SetActorLocation(NewLocation);
	VerifyQueen();
}


