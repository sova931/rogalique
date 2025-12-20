#pragma once
#include "Component.h"
#include "StatsComponent.h"
#include "GameObject.h"

namespace XYZEngine

{
	class AttackComponent : public Component
	{
	public:
		AttackComponent(GameObject* gameObject, float attackPower) : Component(gameObject), attackPower(attackPower)
		{

	   }

		float GetAttackPower() const
		{
			return attackPower;
		}

		void SetAttackPower(float newAttackPower)
		{
			attackPower = newAttackPower;

		}

		void Attack(GameObject* target)
		{
			if (!target)
			{
				return;
			}

			auto targetStatsComponent = target->GetComponent <StatsComponent>();
			if (targetStatsComponent)
			{
				float damage = attackPower;
				targetStatsComponent->TakeDamage(damage);
			}
		}

	private:
		
		float attackPower;

	};



}


