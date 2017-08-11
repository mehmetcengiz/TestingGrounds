// Fill out your copyright notice in the Description page of Project Settings.

#include "TestingGrounds.h"
#include "Tile.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "ActorPool.h"

// Sets default values
ATile::ATile(){
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	NavigationBoundOffset = FVector(2000, 0, 0);

	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);
}

// Called when the game starts or when spawned
void ATile::BeginPlay(){
	Super::BeginPlay();

	
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason){
	Pool->Return(NavMeshBoundsVolume);

}

// Called every frame
void ATile::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

void ATile::SetPool(UActorPool* PoolToSet){
	Pool = PoolToSet;

	PositionNavMeshBoundsVolume();

}



void ATile::PositionNavMeshBoundsVolume(){
	NavMeshBoundsVolume = Pool->Checkout();
	if(NavMeshBoundsVolume == NULL){
		UE_LOG(LogTemp, Error, TEXT("Not Enough actors in pool"))
		return;
	}
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundOffset);

	GetWorld()->GetNavigationSystem()->Build();
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale){

	TArray<FSpawnPosition> SpawnPositions = RandomSpawnPositions(MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);

	for(FSpawnPosition SpawnPosition : SpawnPositions){
		PlaceActor(ToSpawn, SpawnPosition);
	}
			
}

TArray<FSpawnPosition> ATile::RandomSpawnPositions(int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale){
	TArray<FSpawnPosition> SpawnPositions;

	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

	for (int i = 0; i < NumberToSpawn; ++i) {
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (found) {
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			SpawnPositions.Add(SpawnPosition);
		}
	}

	return SpawnPositions;
}


bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius){

	FBox Bounds(MinExtent, MaxExtent);

	const int MAX_ATTEMPTS = 100;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++){
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, Radius)){
			OutLocation = CandidatePoint;
			return true;
		}
	}

	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition){
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius){
	FHitResult HitResult;

	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);


	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);

	FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	//DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true,100); // For debug.

	return !HasHit;
}
