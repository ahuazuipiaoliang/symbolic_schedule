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
#include "symbolic_schedule/Env.hpp"

class GroundedAction
{
private:
    std::string name;
    std::list<std::string> arg_values;
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> gPreconditions;
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> gEffects;

public:
    GroundedAction() {}
    GroundedAction(std::string name, std::list<std::string> arg_values)
    {
        this->name = name;
        for (std::string ar : arg_values)
        {
            this->arg_values.push_back(ar);
        }
    }

    std::string get_name() const
    {
        return this->name;
    }

    std::list<std::string> get_arg_values() const
    {
        return this->arg_values;
    }

    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> getPreconditions()
    {
        return this->gPreconditions;
    }
    std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> getEffects()
    {
        return this->gEffects;
    }

    void add_gpre(const GroundedCondition& gc)
    {
        this->gPreconditions.insert(gc);
    }

    void add_geff(const GroundedCondition& gc)
    {
        this->gEffects.insert(gc);
    }


    bool operator==(const GroundedAction& rhs) const
    {
        if (this->name != rhs.name || this->arg_values.size() != rhs.arg_values.size())
            return false;

        auto lhs_it = this->arg_values.begin();
        auto rhs_it = rhs.arg_values.begin();

        while (lhs_it != this->arg_values.end() && rhs_it != rhs.arg_values.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const GroundedAction& gac)
    {
        os << gac.toString() << " ";
        return os;
    }

    std::string toString() const
    {
        std::string temp = "";
        temp += this->name;
        temp += "(";
        for (std::string l : this->arg_values)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};