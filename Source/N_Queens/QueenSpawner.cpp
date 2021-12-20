
#include "QueenSpawner.h"


#include "MyGameInstance.h"
#include "Queen.h"
#include "TimerManager.h"
#include "Engine/World.h"

AQueenSpawner::AQueenSpawner()
{
	// basic initiation.
	// (200,200,0) is the firs spot's location in the chessboard.
	DefaultSpawnLocation.Set(200.f, 200.f, 0.0f);
	CurrentQueen = nullptr;
	PreviousQueen = nullptr;
}


void AQueenSpawner::BeginPlay()
{
	Super::BeginPlay();

	// start spawning the first Queen after a delay (3 secs)
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AQueenSpawner::SpawnQueen, 1.f, false, 3.f);
}


void AQueenSpawner::CallSpawnQueen()
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AQueenSpawner::SpawnQueen, 1.f, false, 1.5); 
}


void AQueenSpawner::SpawnQueen()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnLocation set for Null Current Queen"))


	FVector SpawnLocation;
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (CurrentQueen == nullptr)
	{
		SpawnLocation = DefaultSpawnLocation;
	}
	else
	{
		// Spawning in the next-right column.
		SpawnLocation.Set(CurrentQueen->GetActorLocation().X + 400.f, 200.f, CurrentQueen->GetActorLocation().Z);
	}



	// spawn the Queen object (bp version)
	if (!ensure(QueenToSpawn != nullptr))
		return;
	Queen = GetWorld()->SpawnActor<AQueen>(QueenToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if (!ensure(Queen != nullptr))
		return;

	// set the data members based on the new and prev Queen. 
	PreviousQueen = CurrentQueen;
	CurrentQueen = Queen;
	QueenArray.Add(Queen);


	// if the 9'th Queen is spawned then the game is finished, destroy the 9th and bring a UI from bp using bGameFinished.
	if (QueenArray.Num() > 8)
	{
		CurrentQueen->Destroyed();

		UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (!ensure(GameInstance != nullptr))
			return;

		GameInstance->bGameFinished = true;
		Destroy();
		return;
	}


	// set the owner of the object in order to make calls from Queen obj.
	Queen->SetOwner(this);

	// verify the Queen (this call will give the handle of the moving queen to the right spot to Queen obj itself).
	Queen->VerifyQueen();
}


void AQueenSpawner::QueenDestroyed(AQueen* QueenToDestroy)
{
	// remove the last queen from the array and set current and prev using the array.
	QueenArray.Remove(QueenToDestroy); 
	CurrentQueen = PreviousQueen;
	PreviousQueen = QueenArray.Last(1);

	// destroy the actor
	QueenToDestroy->Destroy();

	// move the current queen (which is the now in the prev column) to the new location (down in the rows)
	CurrentQueen->MoveInRow();
}