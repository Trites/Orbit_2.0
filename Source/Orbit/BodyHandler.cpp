// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "BodyHandler.h"
#include "OcNode.h"
#include <unordered_set>


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

	OcNode *node = new OcNode(FVector::ZeroVector, FVector(1000, 1000, 1000));
	std::unordered_set<OcNode*> objects;
	delete node;
	objects.insert(NULL);

	int i = objects.count(NULL);



	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Found %i hashed bodies."), i));
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
	checkCounter = 0;
	for (ABody* body : Bodies)
		CalculateForce(body, ocNode, DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Checks %i "), checkCounter));
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

void ABodyHandler::CalculateForce(ABody* body, OcNode* node, float DeltaTime){

	if (node->contains(body) == false){

		OcNode::CenterMass centerMass = node->centerMass;

		if (centerMass.mass > 0 && FVector::DistSquared(body->GetActorLocation(), centerMass.position) > 100){

			FVector direction = (centerMass.position - body->GetActorLocation()).GetSafeNormal();
			float force = (10.0f * body->mass * centerMass.mass) / FVector::DistSquared(centerMass.position, body->GetActorLocation());
			body->ApplyForce(DeltaTime * force * direction);
				
			checkCounter++;
			//DrawDebugLine(GetWorld(), body->GetActorLocation(), centerMass.position, FColor::Blue, false, -1, 0, 2);
		}
	}
	else{

		if (!node->isLeaf)
			for (OcNode* child : node->children){

				CalculateForce(body, child, DeltaTime);
			}
	}
}

