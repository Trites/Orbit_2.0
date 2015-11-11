// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Body.h"
#include "OcNode.h"
#include "GameFramework/Actor.h"
#include "BodyHandler.generated.h"

UCLASS()
class ORBIT_API ABodyHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABodyHandler();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = "Template")
	TSubclassOf<class ABody> BodyTemplate;

private:

	const unsigned int MAX_THETA_SQUARED = 150;

	std::vector<ABody*> Bodies;
	OcNode *ocNode;
	int checkCounter = 0;
	void CalculateForce(ABody* body, OcNode* node, float DeltaTime);
	void SetInitialVelocity(ABody* body, float force);
	void GetGalaxyArea(FVector& center, float& size, float padding = 100.0f);
	void SpawnDisc(const FVector& center, float radius, int objectCount, float zJitter = 0);
	
};
