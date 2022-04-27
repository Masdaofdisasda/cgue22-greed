#pragma once
#include "Physics.h"
#include <vector>

class ItemCollection
{
public:
	struct ItemInfo {
		string name;
		string price;
	};

	void collect(Physics::PhysicsObject* object);
	float getTotalMonetaryValue();
	float getTotalWeight();
	std::vector<ItemInfo> getListOfItems();
	int size();
private:
	std::vector<Physics::PhysicsObject*> collectedItems;
	float totalWeight;
	float totalMonetaryValue;
};

