// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <list>
#include <vector>
#include <unordered_set>
#include "Body.h"

/**
*
*/
class ORBIT_API OcNode
{
public:
	struct CenterMass{
		FVector position;
		float mass;

		void adjust(const FVector& otherPosition, const float& otherMass){
			position = (mass*position + otherMass*otherPosition) / (mass + otherMass);
			mass += otherMass;
		}
	};

	const unsigned int MAX_OBJECTS = 1;
	static const unsigned int CHILD_COUNT = 8;
	const float DEBUG_THIKNESS = 8.0f;

	OcNode() = delete;
	OcNode(FVector origin, FVector size);
	~OcNode();

	void draw(UWorld *world);
	void insert(ABody* body);
	void insert(const std::vector<ABody*>& bodies);
	bool contains(ABody* body);
	const FVector& getSize() const;

	CenterMass centerMass;
	OcNode *children[CHILD_COUNT];
	bool isLeaf;
	bool isUsed;

	ABody* storedBody;

private:
	FVector origin;
	FVector size;
	//int objectCount;

	void split();
	std::size_t childIndexFor(const FVector& point) const;

};