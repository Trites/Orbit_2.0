// Fill out your copyright notice in the Description page of Project Settings.

#include "BodyHandler.h"
#include "Orbit.h"
#include "OcNode.h"


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
void ABodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	delete ocNode;
	ocNode = new OcNode(FVector::ZeroVector, FVector(1000, 1000, 1000));
	ocNode->insert(Bodies);
	ocNode->draw(GetWorld());

	//FVector v(0, 0, 10);

	for (ABody* body : Bodies)
		CalculateForce(body, ocNode, DeltaTime);
	//for (int i = 0; i < Bodies.size(); i++){
	//	for (int o = 0; o < Bodies.size(); o++){

	//		if (i != o){

	//			FVector posA = Bodies[i]->GetActorLocation();
	//			FVector posB = Bodies[o]->GetActorLocation();

	//			FVector direction = (posB - posA).GetSafeNormal();
	//			float force = (100.0f * Bodies[i]->mass * Bodies[o]->mass) / FVector::DistSquared(posA, posB);

	//			Bodies[i]->ApplyForce(DeltaTime*force*direction);
	//		}
	//	}
	//}
}

void ABodyHandler::CalculateForce(ABody* body, const OcNode* node, float DeltaTime){


	if (node->contains(body) == false){

		OcNode::CenterMass centerMass = node->centerMass;

		if (FVector::DistSquared(body->GetActorLocation(), centerMass.position) > 100){

			FVector direction = (centerMass.position - body->GetActorLocation()).GetSafeNormal();
			float force = (100.0f * body->mass * centerMass.mass) / FVector::DistSquared(centerMass.position, body->GetActorLocation());
			body->ApplyForce(DeltaTime * force * direction);
			DrawDebugLine(GetWorld(), body->GetActorLocation(), centerMass.position, FColor::Blue, false, -1, 0, 2);
		}
	}
	else{

		for (const OcNode* child : node->children){

			CalculateForce(body, child, DeltaTime);
		}
	}
}

