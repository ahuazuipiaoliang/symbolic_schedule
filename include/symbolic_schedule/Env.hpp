/**
 * @file GroundedCondition.hpp
 * @author Mark Jin (mark@pixmoving.net)
 * @brief 
 * @version 0.1
 * @date 2022-05-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "symbolic_schedule/Action.hpp"
#include "symbolic_schedule/GroundedCondition.hpp"


/*
#define SYMBOLS 0
#define INITIAL 1
#define GOAL 2
#define ACTIONS 3
#define ACTION_DEFINITION 4
#define ACTION_PRECONDITION 5
#define ACTION_EFFECT 6
*/
enum EnvironmentType{
    SYMBOLS,
    INITIAL,
    GOAL,
    ACTIONS,
    ACTION_DEFINITION,
    ACTION_PRECONDITION,
    ACTION_EFFECT
};

class Env
{
private:
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> initial_conditions;
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> goal_conditions;
    std::unordered_set<Action, ActionHasher, ActionComparator> actions;
    std::unordered_set<std::string> symbols;

public:
    void remove_initial_condition(GroundedCondition gc)
    {
        this->initial_conditions.erase(gc);
    }
    void add_initial_condition(GroundedCondition gc)
    {
        this->initial_conditions.insert(gc);
    }
    void add_goal_condition(GroundedCondition gc)
    {
        this->goal_conditions.insert(gc);
    }
    void remove_goal_condition(GroundedCondition gc)
    {
        this->goal_conditions.erase(gc);
    }
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_initial_conditions() const
    {
        return this->initial_conditions;
    }
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_goal_conditions() const
    {
        return this->goal_conditions;
    }

    std::unordered_set<Action, ActionHasher, ActionComparator> get_actions() const
    {
        return this->actions;
    }


    void add_symbol(std::string symbol)
    {
        symbols.insert(symbol);
    }
    void add_symbols(std::list<std::string> symbols)
    {
        for (std::string l : symbols)
            this->symbols.insert(l);
    }
    void add_action(Action action)
    {
        this->actions.insert(action);
    }

    Action get_action(std::string name)
    {
        for (Action a : this->actions)
        {
            if (a.get_name() == name)
                return a;
        }
        throw std::runtime_error("Action " + name + " not found!");
    }
    std::unordered_set<std::string> get_symbols() const
    {
        return this->symbols;
    }

    friend std::ostream& operator<<(std::ostream& os, const Env& w)
    {
        os << "***** Environment *****" << std::endl << std::endl;
        os << "Symbols: ";
        for (std::string s : w.get_symbols())
            os << s + ",";
        os << std::endl;
        os << "Initial conditions: ";
        for (GroundedCondition s : w.initial_conditions)
            os << s;
        os << std::endl;
        os << "Goal conditions: ";
        for (GroundedCondition g : w.goal_conditions)
            os << g;
        os << std::endl;
        os << "Actions:" << std::endl;
        for (Action g : w.actions)
            os << g << std::endl;
        std::cout << "***** Environment Created! *****" << std::endl;
        return os;
    }
};


