#include "State.h"

class StateMachine
{
	public:
		StateMachine(Enemy* owner, State* currentState);
		State* currentState;
		void changeState(State* newState, Enemy* enemy);
		void Update(Enemy* enemy);

	private:
		Enemy owner;
};