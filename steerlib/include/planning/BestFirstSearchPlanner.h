//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_BEST_FIRST_SEARCH_PLANNER_H__
#define __STEERLIB_BEST_FIRST_SEARCH_PLANNER_H__

/// @file BestFirstSearchPlanner.h
/// @brief Declares and implements a templatized best-first search planner.
///

#include <vector>
#include <stack>
#include <set>
#include <map>

namespace SteerLib {


	/**
	 * @brief The default class used to describe a transition between states, if it was not specified by the user.
	 *
	 * @see
	 *   - Documentation of the BestFirstSearchPlanner class, which describes how states and actions are used.
	 */
	template < class PlanningState >
	class DefaultAction {
	public:
		float cost;
		PlanningState state;
	};


	/**
	 * @brief Internal helper class used to describe a node in the best-first search.
	 *
	 * This class is not intended to be used directly.  Instead, use the BestFirstSearchPlanner that
	 * uses this class for internal representation.
	 *
	 * @see
	 *   - Documentation of the BestFirstSearchPlanner class, which describes how states and actions are used.
	 */
	template < class PlanningState, class PlanningAction >
	class BestFirstSearchNode {
	public:
		BestFirstSearchNode() { }
		BestFirstSearchNode(float _g, float _f, const PlanningState & _previousState, const PlanningAction & _nextAction) 
			: g(_g), f(_f), previousState(_previousState), action(_nextAction), alreadyExpanded(false)
		{	}

		BestFirstSearchNode(float _g, float _f, const PlanningState & _previousState, const PlanningState & _nextState) 
			: g(_g), f(_f), previousState(_previousState), alreadyExpanded(false)
		{
			action.cost = 0.0f;
			action.state = _nextState;
		}

		float g;
		float f;
		PlanningState previousState;
		PlanningAction action;
		bool alreadyExpanded;
	};


	/**
	 * @brief A functor class used to compare the costs of two BestFirstSearchNodes.
	 *
	 * @see
	 *   - Documentation of the BestFirstSearchPlanner class, which describes how states and actions are used.
	 */
	template < class PlanningState, class PlanningAction >
	class CompareCosts {
	public:
		bool operator () (const BestFirstSearchNode<PlanningState, PlanningAction> & n1, const BestFirstSearchNode<PlanningState, PlanningAction> & n2) const { 
			if (n1.f != n2.f) {
				return (n1.f < n2.f);
			}
			else {
				return (n1.g > n2.g);
			}
		}
	};


	/**
	 * @brief A base class for implementing a planning domain used by the BestFirstSearchPlanner.
	 *
	 * To use the BestFirstSearchPlanner, you must provide a class that implements the search heuristic, state transitions, and
	 * the meaning of a goal state.  To make such a class, you can inherit this class and implement the functionality.
	 * Because the BestFirstSearchPlanner is a template class, it is not necessary to
	 * inherit this class directly, but you may find it helpful to do so.  The real requirement is that
	 * your planning domain class has at least the same functionality that is contained in this class.
	 *
	 * <b>Note:</b> You should not instantiate this class directly, even though it is also templatized, because 
	 * you may wish to have different planning domains that use the same data type to represent state.  Instead,
	 * inherit this class and then implement the functionality in your derived class.
	 *
	 * @see
	 *   - Refer to the details of each function in this class.
	 *   - Documentation of the BestFirstSearchPlanner class, which describes how states and actions are used.
	 */
	template < class PlanningState, class PlanningAction >
	class PlanningDomainBase {
	public:
		/**
		 * @brief Returns true if state is a valid goal state for the planning problem.
		 *
		 * The parameter idealGoalState is only a reference to the goal state that the user specified
		 * when calling BestFirstSearchPlanner::computePlan().  Depending on your planning task,
		 * you may want to ignore this value, for example, if you have multiple goals and only need
		 * to reach one of those goal states.
		 *
		 * In most cases, however, this parameter is a helpful optimization, that allows you to
		 * compute "distance" or "cost" to the goal state without having to store extra data in
		 * your planning domain class.  In some cases, this allows you to use the same instance of the
		 * planning domain for all agents that are performing the same planning task, even though 
		 * they have different start and goal states.
		 *
		 */
		bool isAGoalState( const PlanningState & state, const PlanningState & idealGoalState );

		/**
		 * @brief Populates the list of actions that can be taken from the current state.
		 *
		 * When you implement this function, make sure that you initialize both the (1) cost and (2) new state 
		 * of the action being generated.
		 *
		 * You may wish to use the previousState or the idealGoalState as additional information
		 * that helps you decide how to generate actions.  The classic example of this is when the
		 * state space is a graph, you should avoid generating an action that will take you back to 
		 * the previous graph node.  In another example, you may choose to generate goal-dependent
		 * actions.
		 *
		 */
		void generateTransitions( const PlanningState & currentState, const PlanningState & previousState, const PlanningState & idealGoalState, std::vector<PlanningAction> & transitions );

		/**
		 * @brief Computes the heuristic function (often denoted as f) that estimates the "goodness" of a state towards the goal.
		 *
		 * Note carerfully that this function should compute f, not h.  For generalized best-first search, there is no explicit concept of h.
		 * For example, to implement A*, this function would return f, where f = currentg + h.  In this case, h is the estimated distance or cost from the current state to the goal state.
		 *
		 */
		float estimateTotalCost( const PlanningState & currentState, const PlanningState & idealGoalState, float currentg );
	};


	/**
	 * @brief A state-space planning algorithm that uses a generalized best-first search.
	 *
	 * This class implements an easy-to-use, generalized best-first search.  This class has no knowledge of the actual planning task.
	 * To use this class, you must provide information about the planning task using three template parameters:
	 *   - PlanningDomain is a simple user-defined class that defines the search heuristic, state transitions, and the meaning of a goal state.
	 *   - PlanningState is the data type used to represent a state.
	 *   - PlanningAction is an optional data type used to represent an action.  If an action data type is not specified, the compact DefaultAction &lt;PlanningState&gt; class is used.
	 *
	 * Depending on how you define the action costs and heuristic function, the search can work like A*, near-optimal best-first search,
	 * greedy best-first, or any other best-first search technique.  An example of using this planner is given below.
	 *
	 * <h3> Background </h3>
	 *
	 * To compute a plan, the planner needs the following information (i.e., the planning domain):
	 *   - A state space, which is the space of all possible states that can occur in the problem,
	 *   - An action space, which describes the space of actions that transition from one state to another,
	 *   - Costs associated with each action,
	 *   - A heuristic to estimate the "goodness" of a given state, denoted as f, which tells the planner what to search next.
	 *   - A notion of what it means to be a goal state.
	 *
	 * The user implicitly provides all this information with the template parameters when instantiating the planner.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Using this class is very straightforward:
	 *
	 *   -# Choose the data types you will use to represent state and action.
	 *   -# Implement a class that describes the state and action spaces, providing functionality that is the same as SteerLib::PlanningDomainBase.
	 *   -# Instantiate the BestFirstSearchPlanner specifying the template parameters for your planning domain class, the data type for a state, and (optionally) the 
	 *      data type for an action.
	 *   -# Initialize the planner using init().
	 *   -# Call computePlan(), specifying your start state and goal state.
	 *
	 * <b> Notes: </b>
	 *
	 *   - The data type you use for a State must implement the assignment "=", equals "==", and comparison "<" operators
	 *     so that it can be used by this planner.  It may be a good idea to test these operators using an STL set and an STL map,
	 *     because that is what we use for internal data structures.
	 *
	 *   - It is optional to specify an action data type, otherwise the DefaultAction &lt;PlanningState&gt; data type will be used.
	 *     This default simply contains the cost of the action, and the resulting new state of the action, which is compact and
	 *     suitable for most common planning tasks.
	 *
	 *   - When computePlan() is called, the planner will compute either a sequence of states or a sequence of 
	 *     actions, depending on which overloaded function was called.
	 *
	 *   - Depending on how you implemented the planning domain class, it may be possible to call computePlan() any 
	 *     number of times using the same instance of the planner.  Furthermore, the planner class maintains no
	 *     state internally, so it is possible to use separate threads for separate planning tasks using the 
	 *     same instance of the planner  (assuming the search horizon stays the same).
	 *
	 *   - During initialization, a search horizon is specified.  This allows the user to limit the number of
	 *     nodes expanded during the search process (i.e. the number of times that SteerLib::PlanningDomainBase::generateTransitions()
	 *     will be called).   If a complete plan is not found within the allowed number of nodes to expand, computePlan() will
	 *     return false, but will still provide an incomplete plan that it can construct to the node that had the
	 *     best heuristic f value.
	 *
	 * <b> Recommendations: </b>
	 *
	 *   - Instead of using larger data structures for State and Action, many times it is appropriate to
	 *     use an integer ID or a pointer reference for your State data type.  This reference/index would
	 *     then refer to the actual data corresponding to that State/Action.  This allows you to maintain
	 *     a "cache" of states for yourself, which is useful for gathering statistics, debugging your 
	 *     implementation, and for performance of this planner.
	 *
	 *   - When possible, performance can be improved by inlining the functions you implement in your
	 *     PlanningDomain class.
	 *
	 *
	 * <h3> Implementing the state and action spaces </h3>
	 *
	 * The first template parameter, PlanningDomain, is a class that describes the 
	 * state space and action space of the planner.  This class should be implemented by the user,
	 * and it must have at least the same functionality as the SteerLib::PlanningDomainBase class.  
	 * Refer to the documentation in the SteerLib::PlanningDomainBase class for more information about the three
	 * functions that must be implemented.
	 *
	 * It is not necessary to inherit the SteerLib::PlanningDomainBase class; it is only required to have the
	 * same functionality.
	 *
	 *
	 * @see
	 *   - Documentation of the SteerLib::PlanningDomainBase class, which describes how states and actions are specified to the planner.
	 *
	 * <h3> Code example </h3>
	 *
	 * \code
	 * 
	 * class ExamplePlanningDomain {
	 *   public:
	 *     bool isAGoalState( const int & state,
	 *                        const int & idealGoalState)
	 *     {
	 *         return state == idealGoalState;
	 *     }
	 *
	 *     float estimateTotalCost( const int & currentState, 
	 *                              const int & idealGoalState,
	 *                              float currentg)
	 *     {
	 *         // if distance() is admissible, then this implementation is an A* search.
	 *         float h = distance( currentState, idealGoalState);
	 *         float f = currentg + h;  
	 *         return f;
	 *     }
	 *
	 *     void generateTransitions( const int & currentState,
	 *                               const int & previousState, 
	 *                               const int & idealGoalState,
	 *                               std::vector<SteerLib::DefaultAction<int> > & transitions )
	 *     {
	 *         DefaultAction<int> newAction;
	 *         for_each_transition_possible_from_currentState {
	 *             newAction.cost = cost_of_this_specific_transition;
	 *             newAction.state = new_state_after_transition;
	 *             transitions.push_back( newAction );
	 *         }
	 *     }
	 *
	 * };
	 *
	 * int main( )
	 * {
	 *     // Of course, in your code these variables would need to be initialized properly.
	 *     ExamplePlanningDomain domain;
	 *     int currentState, goalState;
	 *
	 *     // This example has effectively no limited horizon.
	 *     unsigned int numNodesInHorizon = UINT_MAX;
	 *
	 *     // Instantiate and initialize the planner
	 *     BestFirstSearchPlanner<ExamplePlanningDomain, int> examplePlanner;
	 *     examplePlanner.init( &domain, numNodesInHorizon);
	 *
	 *     // Compute the plan
	 *     std::stack<int> outputPlan;
	 *     examplePlanner.computePlan(currentState, goalState, outputPlan);
	 *
	 *     // Use the plan
	 *     while (!outputPlan.empty()) {
	 *         int nextState = outputPlan.pop();
	 *
	 *         ... // other code
	 *     }
	 *
	 *     return 0;
	 * }
	 * \endcode
	 *
	 *
	 */
	template < class PlanningDomain, class PlanningState, class PlanningAction = DefaultAction<PlanningState> >
	class BestFirstSearchPlanner {
	public:

		/// Initializes the planner to use the specified instance of the planning domain, and sets the search horizon limit.
		void init(PlanningDomain * newPlanningDomain, unsigned int maxNumNodesToExpand ) {
			_maxNumNodesToExpand = maxNumNodesToExpand;
			_planningDomain = newPlanningDomain;
		}

		/// Computes a plan as a sequence of states; returns true if the planner could reach the goal, or false if the plan is only partial and could not reach the goal within the specified horizon.
		bool computePlan( const PlanningState & startState, const PlanningState & goalState, std::stack<PlanningState> & plan );

		/// Computes a plan as a sequence of actions; returns true if the planner could reach the goal, or false if the plan is only partial and could not reach the goal within the specified horizon.
		bool computePlan( const PlanningState & startState, const PlanningState & goalState, std::stack<PlanningAction> & plan );

	protected:
		bool _computePlan( const PlanningState & startState, const PlanningState & idealGoalState, std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> > & stateMap, PlanningState & actualStateReached );
		unsigned int _maxNumNodesToExpand;
		PlanningDomain * _planningDomain;
	};


	template < class PlanningDomain, class PlanningState, class PlanningAction >
	bool BestFirstSearchPlanner< PlanningDomain, PlanningState, PlanningAction >::computePlan( const PlanningState & startState, const PlanningState & goalState, std::stack<PlanningState> & plan )
	{
		std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> > stateMap;

		PlanningState s;

		bool isPlanComplete = _computePlan(startState, goalState, stateMap, s);

		// reconstruct path here
		plan.push(s);  // push the goal state
		do {
			// keep pushing until the start state was pushed. (inclusive)
			s = (*stateMap.find( s )).second.previousState;
			plan.push(s);
		} while ( !(s == startState) );

		return isPlanComplete;
	}


	template < class PlanningDomain, class PlanningState, class PlanningAction >
	bool BestFirstSearchPlanner< PlanningDomain, PlanningState, PlanningAction >::computePlan( const PlanningState & startState, const PlanningState & goalState, std::stack<PlanningAction> & plan )
	{
		std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> > stateMap;
		typename std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> >::iterator iter;

		PlanningState s;
		PlanningAction a;

		bool isPlanComplete = _computePlan(startState, stateMap, s);

		// reconstruct path here
		do {
			// push all actions except for the very first one which was an invalid dummy action.
			iter = stateMap.find(s);
			a = (*iter).second.action;
			plan.push(a);
			s = (*iter).second.previousState;
		} while ( (s == startState) );

		return isPlanComplete;
	}


	template < class PlanningDomain, class PlanningState, class PlanningAction >
	bool BestFirstSearchPlanner< PlanningDomain, PlanningState, PlanningAction >::_computePlan( const PlanningState & startState, const PlanningState & idealGoalState, std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> > & stateMap, PlanningState & actualStateReached )
	{
		std::set<BestFirstSearchNode<PlanningState, PlanningAction>, CompareCosts<PlanningState, PlanningAction> > openSet;

		stateMap.clear();
		openSet.clear();

		float newf = _planningDomain->estimateTotalCost(startState, idealGoalState, 0.0f);
		BestFirstSearchNode<PlanningState, PlanningAction> nextNode(0.0f, newf, startState, startState);

		stateMap[nextNode.action.state] = nextNode;
		openSet.insert( nextNode );

		unsigned int numNodesExpanded = 0;

		while ((numNodesExpanded < _maxNumNodesToExpand) && (!openSet.empty())) {

			numNodesExpanded++;

			// get a copy of the first element of the open set (i.e. about to pop it, but only if we get past the next if-statement).
			BestFirstSearchNode<PlanningState, PlanningAction> x = *(openSet.begin());

			// ask the user if this node is a goal state.  If so, then finish up.
			if ( _planningDomain->isAGoalState( x.action.state, idealGoalState ) ) {
				actualStateReached = x.action.state;
				return true;
			}

			// move x from open set to closed set.
			// NOTE CAREFULLY that nodeInMap is an alias, so that means we are also 
			// modifying the boolean alreadyExpanded in the stateMap as well.
			openSet.erase( openSet.begin() );
			BestFirstSearchNode<PlanningState, PlanningAction> & nodeInMap = (*stateMap.find(x.action.state)).second;
			nodeInMap.alreadyExpanded = true;

			// ask the user to generate all the possible actions from this state.
			std::vector<PlanningAction> possibleActions;
			possibleActions.clear();
			_planningDomain->generateTransitions( x.action.state, x.previousState, idealGoalState, possibleActions );

			// iterate over each potential action, and add it to the open list.
			// if the node was already seen before, then it is updated if the new cost is better than the old cost.
			for ( typename std::vector<PlanningAction>::const_iterator action = possibleActions.begin();  action != possibleActions.end(); ++action) {

				float newg = x.g + (*action).cost;

				typename std::map<PlanningState, BestFirstSearchNode<PlanningState, PlanningAction> >::iterator existingNode;
				existingNode = stateMap.find( (*action).state );
				if ( existingNode != stateMap.end() ) {
					// then, that means this node was seen before.
					if (newg < (*existingNode).second.g) {
						// then, this means we need to update the node.
						if ((*existingNode).second.alreadyExpanded == false) {
							openSet.erase((*existingNode).second);
						}
						stateMap.erase(existingNode);
					}
					else {
						// otherwise, we don't bother adding this node... it already exists with a better cost.
						continue;
					}
				}


				newf = _planningDomain->estimateTotalCost((*action).state, idealGoalState, newg);
				nextNode = BestFirstSearchNode<PlanningState, PlanningAction>(newg, newf, x.action.state, (*action));

				stateMap[nextNode.action.state] = nextNode;
				openSet.insert( nextNode );
			}
		}



		if (openSet.empty()) {
			// if we get here, there was no solution.
			actualStateReached = startState;
		}
		else {
			// if we get here, then we did not find a complete path.
			// instead, just return whatever path we could construct.
			// The idea is that if the user gave a reasonable heuristic,
			// state space, and transitions, then the next node that
			// would be expanded will be the most promising path anyway.
			//
			actualStateReached = (*(openSet.begin())).action.state;
		}

		return false;  // returns false because plan is incomplete.
	}

} // end namespace SteerLib

#endif
