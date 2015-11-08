// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "BodyHandler.h"


// Sets default values
ABodyHandler::ABodyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABodyHandler::BeginPlay()
{
	Super::BeginPlay();

	int c = 0;

	for (TActorIterator<ABody> it(GetWorld()); it; ++it){

		c++;
		Bodies.push_back(*it);
	}

	ocNode = new OcNode(FVector::ZeroVector, FVector(1000, 1000, 1000));
	ocNode->insert(Bodies);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Simulating %i bodies."), Bodies.size()));
}

// Called every frame
void ABodyHandler::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	delete ocNode;
	ocNode = new OcNode(FVector::ZeroVector, FVector(1000, 1000, 1000));
	ocNode->insert(Bodies);
	ocNode->draw(GetWorld());

	//FVector v(0, 0, 10);

	for (int i = 0; i < Bodies.size(); i++){
		for (int o = 0; o < Bodies.size(); o++){

			if (i != o){

				FVector posA = Bodies[i]->GetActorLocation();
				FVector posB = Bodies[o]->GetActorLocation();

				FVector direction = (posB - posA).GetSafeNormal();
				float force = (100.0f * Bodies[i]->mass * Bodies[o]->mass) / FVector::DistSquared(posA, posB);

				Bodies[i]->ApplyForce(DeltaTime*force*direction);
			}
		}
	}
}

