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
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <queue>

#include "symbolic_schedule/Condition.hpp"

class Action
{
private:
    std::string name;
    std::list<std::string> args;
    std::unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    std::unordered_set<Condition, ConditionHasher, ConditionComparator> effects;

public:
    Action(std::string name, std::list<std::string> args,
        std::unordered_set<Condition, ConditionHasher, ConditionComparator>& preconditions,
        std::unordered_set<Condition, ConditionHasher, ConditionComparator>& effects)
    {
        this->name = name;
        for (std::string l : args)
        {
            this->args.push_back(l);
        }
        for (Condition pc : preconditions)
        {
            this->preconditions.insert(pc);
        }
        for (Condition pc : effects)
        {
            this->effects.insert(pc);
        }
    }
    std::string get_name() const
    {
        return this->name;
    }
    std::list<std::string> get_args() const
    {
        return this->args;
    }
    std::unordered_set<Condition, ConditionHasher, ConditionComparator> get_preconditions() const
    {
        return this->preconditions;
    }
    std::unordered_set<Condition, ConditionHasher, ConditionComparator> get_effects() const
    {
        return this->effects;
    }

    bool operator==(const Action& rhs) const
    {
        if (this->get_name() != rhs.get_name() || this->get_args().size() != rhs.get_args().size())
            return false;

        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Action& ac)
    {
        os << ac.toString() << std::endl;
        os << "Precondition: ";
        for (Condition precond : ac.get_preconditions())
            os << precond;
        os << std::endl;
        os << "Effect: ";
        for (Condition effect : ac.get_effects())
            os << effect;
        os << std::endl;
        return os;
    }

    std::string toString() const
    {
        std::string temp = "";
        temp += this->get_name();
        temp += "(";
        for (std::string l : this->get_args())
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct ActionComparator
{
    bool operator()(const Action& lhs, const Action& rhs) const
    {
        return lhs == rhs;
    }
};

struct ActionHasher
{
    size_t operator()(const Action& ac) const
    {
        return std::hash<std::string>{}(ac.get_name());
    }
};