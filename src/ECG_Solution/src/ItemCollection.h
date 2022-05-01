#pragma once
#include "Physics.h"
#include <vector>

class item_collection
{
public:
	struct item_info {
		string name;
		string price;
	};

	void collect(Physics::PhysicsObject* object);
	float get_total_monetary_value() const;
	float get_total_weight() const;
	std::vector<item_info> get_list_of_items() const;
	size_t size() const;
private:
	std::vector<Physics::PhysicsObject*> collectedItems;
	float total_weight_ = 0;
	float total_monetary_value_ = 0;
};

