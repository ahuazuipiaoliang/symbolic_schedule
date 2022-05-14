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
#include <regex>
#include "symbolic_schedule/Env.hpp"
#include "symbolic_schedule/GroundedAction.hpp"


std::list<std::string> parse_symbols(std::string symbols_str)
{
    std::list<std::string> symbols;
    size_t pos = 0;
    std::string delimiter = ",";
    while ((pos = symbols_str.find(delimiter)) != std::string::npos)
    {
        std::string symbol = symbols_str.substr(0, pos);
        symbols_str.erase(0, pos + delimiter.length());
        symbols.push_back(symbol);
    }
    symbols.push_back(symbols_str);
    return symbols;
}

Env* create_env(char* filename)
{
    std::ifstream input_file(filename);
    Env* env = new Env();
    std::regex symbolStateRegex("symbols:", std::regex::icase);
    std::regex symbolRegex("([a-zA-Z0-9_, ]+) *");
    std::regex initialConditionRegex("initialconditions:(.*)", std::regex::icase);
    std::regex conditionRegex("(!?[A-Z][a-zA-Z_]*) *\\( *([a-zA-Z0-9_, ]+) *\\)");
    std::regex goalConditionRegex("goalconditions:(.*)", std::regex::icase);
    std::regex actionRegex("actions:", std::regex::icase);
    std::regex precondRegex("preconditions:(.*)", std::regex::icase);
    std::regex effectRegex("effects:(.*)", std::regex::icase);
    int parser = SYMBOLS;

    std::unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    std::unordered_set<Condition, ConditionHasher, ConditionComparator> effects;
    std::string action_name;
    std::string action_args;

    std::string line;
    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            std::string::iterator end_pos = remove(line.begin(), line.end(), ' ');
            line.erase(end_pos, line.end());

            if (line == "")
                continue;

            if (parser == SYMBOLS)
            {
                std::smatch results;
                if (std::regex_search(line, results, symbolStateRegex))
                {
                    line = line.substr(8);
                    std::sregex_token_iterator iter(line.begin(), line.end(), symbolRegex, 0);
                    std::sregex_token_iterator end;

                    env->add_symbols(parse_symbols(iter->str()));  // fixed

                    parser = INITIAL;
                }
                else
                {
                    std::cout << "Symbols are not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == INITIAL)
            {
                const char* line_c = line.c_str();
                if (std::regex_match(line_c, initialConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    std::sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    std::sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        std::string predicate = iter->str();
                        iter++;
                        // args
                        std::string args = iter->str();
                        iter++;

                        if (predicate[0] == '!')
                        {
                            env->remove_initial_condition(
                                GroundedCondition(predicate.substr(1), parse_symbols(args)));
                        }
                        else
                        {
                            env->add_initial_condition(
                                GroundedCondition(predicate, parse_symbols(args)));
                        }
                    }

                    parser = GOAL;
                }
                else
                {
                    std::cout << "Initial conditions not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == GOAL)
            {
                const char* line_c = line.c_str();
                if (std::regex_match(line_c, goalConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    std::sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    std::sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        std::string predicate = iter->str();
                        iter++;
                        // args
                        std::string args = iter->str();
                        iter++;

                        if (predicate[0] == '!')
                        {
                            env->remove_goal_condition(
                                GroundedCondition(predicate.substr(1), parse_symbols(args)));
                        }
                        else
                        {
                            env->add_goal_condition(
                                GroundedCondition(predicate, parse_symbols(args)));
                        }
                    }

                    parser = ACTIONS;
                }
                else
                {
                    std::cout << "Goal conditions not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == ACTIONS)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, actionRegex))
                {
                    parser = ACTION_DEFINITION;
                }
                else
                {
                    std::cout << "Actions not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == ACTION_DEFINITION)
            {
                const char* line_c = line.c_str();
                if (std::regex_match(line_c, conditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    std::sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    std::sregex_token_iterator end;
                    // name
                    action_name = iter->str();
                    iter++;
                    // args
                    action_args = iter->str();
                    iter++;

                    parser = ACTION_PRECONDITION;
                }
                else
                {
                    std::cout << "Action not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == ACTION_PRECONDITION)
            {
                const char* line_c = line.c_str();
                if (std::regex_match(line_c, precondRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    std::sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    std::sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        std::string predicate = iter->str();
                        iter++;
                        // args
                        std::string args = iter->str();
                        iter++;

                        bool truth;

                        if (predicate[0] == '!')
                        {
                            predicate = predicate.substr(1);
                            truth = false;
                        }
                        else
                        {
                            truth = true;
                        }

                        Condition precond(predicate, parse_symbols(args), truth);
                        preconditions.insert(precond);
                    }

                    parser = ACTION_EFFECT;
                }
                else
                {
                    std::cout << "Precondition not specified correctly." << std::endl;
                    throw;
                }
            }
            else if (parser == ACTION_EFFECT)
            {
                const char* line_c = line.c_str();
                if (std::regex_match(line_c, effectRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    std::sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    std::sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        std::string predicate = iter->str();
                        iter++;
                        // args
                        std::string args = iter->str();
                        iter++;

                        bool truth;

                        if (predicate[0] == '!')
                        {
                            predicate = predicate.substr(1);
                            truth = false;
                        }
                        else
                        {
                            truth = true;
                        }

                        Condition effect(predicate, parse_symbols(args), truth);
                        effects.insert(effect);
                    }

                    env->add_action(
                        Action(action_name, parse_symbols(action_args), preconditions, effects));

                    preconditions.clear();
                    effects.clear();
                    parser = ACTION_DEFINITION;
                }
                else
                {
                    std::cout << "Effects not specified correctly." << std::endl;
                    throw;
                }
            }
        }
        input_file.close();
    }

    else
        std::cout << "Unable to open file";

    return env;
}

/// Calculate all permutation of actions
void PermutationsUtil(std::unordered_set<std::string> symbols, int num, std::list<std::string> temp, std::vector<std::list<std::string>>& ans) {

    if (temp.size() == num)
        ans.push_back(temp);
    else {
        if (temp.size())
            symbols.erase(temp.back());
        for (auto i : symbols) {
            temp.push_back(i);
            PermutationsUtil(symbols, num, temp, ans);
            temp.pop_back();
        }
    }
    return;
}

std::vector<std::list<std::string>> Permutations(std::unordered_set<std::string> symbols, int num) {

    std::list<std::string> temp;
    std::vector<std::list<std::string>> ans;
    PermutationsUtil(symbols, num, temp, ans);
    return ans;
}


std::vector<std::vector<std::list<std::string>>> generate_depth_tree(const std::unordered_set<std::string>& symbols) {

    std::vector<std::vector<std::list<std::string>>> depth_tree;
    for (int i = 1; i <= symbols.size(); i++)
        depth_tree.push_back(Permutations(symbols, i));
    return depth_tree;
}

void create_cond(std::unordered_set<Condition, ConditionHasher, ConditionComparator> conds, GroundedAction& ga, std::unordered_map<std::string, std::string>& arg_mapping, int temp) {
    for (Condition i : conds) {
        std::list<std::string> args;
        for (const auto& j : i.get_args()) {
            args.push_back(arg_mapping[j]);
        }
        GroundedCondition g(i.get_predicate(), args, i.get_truth());
        if (temp == 0)
            ga.add_gpre(g);
        else
            ga.add_geff(g);
    }

}

std::list<GroundedAction> action_space(const std::unordered_set<std::string>& symbols, const std::unordered_set<Action, ActionHasher, ActionComparator>& actions) {

    auto all_permutations = generate_depth_tree(symbols);
    std::list<GroundedAction> all_actions;

    for (auto action : actions) {
        int num_args = action.get_args().size() - 1;
        std::unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions = action.get_preconditions();
        std::unordered_set<Condition, ConditionHasher, ConditionComparator> effects = action.get_effects();
        std::list<std::string> arg_names = action.get_args();

        for (auto args : all_permutations[num_args]) {
            GroundedAction ga(action.get_name(), args);
            std::unordered_map<std::string, std::string> map_args;

            for (const auto& s : symbols) {
                map_args[s] = s;
            }
            auto i = args.begin();
            for (const auto& arg_name : arg_names) {
                map_args[arg_name] = *i;
                i++;
            }

            create_cond(preconditions, ga, map_args, 0);
            create_cond(effects, ga, map_args, 1);
            all_actions.emplace_back(ga);
        }
    }
    return all_actions;
}


typedef std::unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> g_state;


struct g_state_comparison {
    bool operator()(const g_state& state1, const g_state& state2) const {
        if (state1.size() != state2.size())
            return 0;
        for (GroundedCondition gc : state1) {
            if (state2.find(gc) == state2.end()) {
                return 0;
            }
        }
        return 1;
    }
};

struct g_state_hasher {
    size_t operator()(g_state state) const
    {
        auto state_first = *(state.begin());
        return std::hash<std::string> {} (state_first.toString());
    }
};