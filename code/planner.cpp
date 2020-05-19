#include <iostream>
#include <fstream>
#include <regex>
#include <unordered_set>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <queue>
#define SYMBOLS 0
#define INITIAL 1
#define GOAL 2
#define ACTIONS 3
#define ACTION_DEFINITION 4
#define ACTION_PRECONDITION 5
#define ACTION_EFFECT 6

class GroundedCondition;
class Condition;
class GroundedAction;
class Action;
class Env;

using namespace std;

bool print_status = true;

class GroundedCondition
{
private:
    string predicate;
    list<string> arg_values;
    bool truth = true;

public:
    GroundedCondition(string predicate, list<string> arg_values, bool truth = true)
    {
        this->predicate = predicate;
        this->truth = truth;  // fixed
        for (string l : arg_values)
        {
            this->arg_values.push_back(l);
        }
    }

    GroundedCondition(const GroundedCondition& gc)
    {
        this->predicate = gc.predicate;
        this->truth = gc.truth;  // fixed
        for (string l : gc.arg_values)
        {
            this->arg_values.push_back(l);
        }
    }

    string get_predicate() const
    {
        return this->predicate;
    }
    list<string> get_arg_values() const
    {
        return this->arg_values;
    }

    bool get_truth() const
    {
        return this->truth;
    }
    void set_truth(bool t) {

        this->truth = t;
    }

    friend ostream& operator<<(ostream& os, const GroundedCondition& pred)
    {
        os << pred.toString() << " ";
        return os;
    }

    bool operator==(const GroundedCondition& rhs) const
    {
        if (this->predicate != rhs.predicate || this->arg_values.size() != rhs.arg_values.size())
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

        if (this->truth != rhs.get_truth()) // fixed
            return false;

        return true;
    }

    string toString() const
    {
        string temp = "";
        if (!this->truth)
            temp += "!";
        temp += this->predicate;
        temp += "(";
        for (string l : this->arg_values)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct GroundedConditionComparator
{
    bool operator()(const GroundedCondition& lhs, const GroundedCondition& rhs) const
    {
        return lhs == rhs;
    }
};

struct GroundedConditionHasher
{
    size_t operator()(const GroundedCondition& gcond) const
    {
        return hash<string>{}(gcond.toString());
    }
};

class Condition
{
private:
    string predicate;
    list<string> args;
    bool truth;

public:
    Condition(string pred, list<string> args, bool truth)
    {
        this->predicate = pred;
        this->truth = truth;
        for (string ar : args)
        {
            this->args.push_back(ar);
        }
    }

    string get_predicate() const
    {
        return this->predicate;
    }

    list<string> get_args() const
    {
        return this->args;
    }

    bool get_truth() const
    {
        return this->truth;
    }

    friend ostream& operator<<(ostream& os, const Condition& cond)
    {
        os << cond.toString() << " ";
        return os;
    }

    bool operator==(const Condition& rhs) const // fixed
    {

        if (this->predicate != rhs.predicate || this->args.size() != rhs.args.size())
            return false;

        auto lhs_it = this->args.begin();
        auto rhs_it = rhs.args.begin();

        while (lhs_it != this->args.end() && rhs_it != rhs.args.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }

        if (this->truth != rhs.get_truth())
            return false;

        return true;
    }

    string toString() const
    {
        string temp = "";
        if (!this->truth)
            temp += "!";
        temp += this->predicate;
        temp += "(";
        for (string l : this->args)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct ConditionComparator
{
    bool operator()(const Condition& lhs, const Condition& rhs) const
    {
        return lhs == rhs;
    }
};

struct ConditionHasher
{
    size_t operator()(const Condition& cond) const
    {
        return hash<string>{}(cond.toString());
    }
};

class Action
{
private:
    string name;
    list<string> args;
    unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    unordered_set<Condition, ConditionHasher, ConditionComparator> effects;

public:
    Action(string name, list<string> args,
        unordered_set<Condition, ConditionHasher, ConditionComparator>& preconditions,
        unordered_set<Condition, ConditionHasher, ConditionComparator>& effects)
    {
        this->name = name;
        for (string l : args)
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
    string get_name() const
    {
        return this->name;
    }
    list<string> get_args() const
    {
        return this->args;
    }
    unordered_set<Condition, ConditionHasher, ConditionComparator> get_preconditions() const
    {
        return this->preconditions;
    }
    unordered_set<Condition, ConditionHasher, ConditionComparator> get_effects() const
    {
        return this->effects;
    }

    bool operator==(const Action& rhs) const
    {
        if (this->get_name() != rhs.get_name() || this->get_args().size() != rhs.get_args().size())
            return false;

        return true;
    }

    friend ostream& operator<<(ostream& os, const Action& ac)
    {
        os << ac.toString() << endl;
        os << "Precondition: ";
        for (Condition precond : ac.get_preconditions())
            os << precond;
        os << endl;
        os << "Effect: ";
        for (Condition effect : ac.get_effects())
            os << effect;
        os << endl;
        return os;
    }

    string toString() const
    {
        string temp = "";
        temp += this->get_name();
        temp += "(";
        for (string l : this->get_args())
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
        return hash<string>{}(ac.get_name());
    }
};

class Env
{
private:
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> initial_conditions;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> goal_conditions;
    unordered_set<Action, ActionHasher, ActionComparator> actions;
    unordered_set<string> symbols;

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
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_initial_conditions() const
    {
        return this->initial_conditions;
    }
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_goal_conditions() const
    {
        return this->goal_conditions;
    }

    unordered_set<Action, ActionHasher, ActionComparator> get_actions() const
    {
        return this->actions;
    }


    void add_symbol(string symbol)
    {
        symbols.insert(symbol);
    }
    void add_symbols(list<string> symbols)
    {
        for (string l : symbols)
            this->symbols.insert(l);
    }
    void add_action(Action action)
    {
        this->actions.insert(action);
    }

    Action get_action(string name)
    {
        for (Action a : this->actions)
        {
            if (a.get_name() == name)
                return a;
        }
        throw runtime_error("Action " + name + " not found!");
    }
    unordered_set<string> get_symbols() const
    {
        return this->symbols;
    }

    friend ostream& operator<<(ostream& os, const Env& w)
    {
        os << "***** Environment *****" << endl << endl;
        os << "Symbols: ";
        for (string s : w.get_symbols())
            os << s + ",";
        os << endl;
        os << "Initial conditions: ";
        for (GroundedCondition s : w.initial_conditions)
            os << s;
        os << endl;
        os << "Goal conditions: ";
        for (GroundedCondition g : w.goal_conditions)
            os << g;
        os << endl;
        os << "Actions:" << endl;
        for (Action g : w.actions)
            os << g << endl;
        cout << "***** Environment Created! *****" << endl;
        return os;
    }
};

class GroundedAction
{
private:
    string name;
    list<string> arg_values;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> gPreconditions;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> gEffects;

public:
    GroundedAction() {}
    GroundedAction(string name, list<string> arg_values)
    {
        this->name = name;
        for (string ar : arg_values)
        {
            this->arg_values.push_back(ar);
        }
    }

    string get_name() const
    {
        return this->name;
    }

    list<string> get_arg_values() const
    {
        return this->arg_values;
    }

    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> getPreconditions()
    {
        return this->gPreconditions;
    }
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> getEffects()
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

    friend ostream& operator<<(ostream& os, const GroundedAction& gac)
    {
        os << gac.toString() << " ";
        return os;
    }

    string toString() const
    {
        string temp = "";
        temp += this->name;
        temp += "(";
        for (string l : this->arg_values)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

list<string> parse_symbols(string symbols_str)
{
    list<string> symbols;
    size_t pos = 0;
    string delimiter = ",";
    while ((pos = symbols_str.find(delimiter)) != string::npos)
    {
        string symbol = symbols_str.substr(0, pos);
        symbols_str.erase(0, pos + delimiter.length());
        symbols.push_back(symbol);
    }
    symbols.push_back(symbols_str);
    return symbols;
}

Env* create_env(char* filename)
{
    ifstream input_file(filename);
    Env* env = new Env();
    regex symbolStateRegex("symbols:", regex::icase);
    regex symbolRegex("([a-zA-Z0-9_, ]+) *");
    regex initialConditionRegex("initialconditions:(.*)", regex::icase);
    regex conditionRegex("(!?[A-Z][a-zA-Z_]*) *\\( *([a-zA-Z0-9_, ]+) *\\)");
    regex goalConditionRegex("goalconditions:(.*)", regex::icase);
    regex actionRegex("actions:", regex::icase);
    regex precondRegex("preconditions:(.*)", regex::icase);
    regex effectRegex("effects:(.*)", regex::icase);
    int parser = SYMBOLS;

    unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    unordered_set<Condition, ConditionHasher, ConditionComparator> effects;
    string action_name;
    string action_args;

    string line;
    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            string::iterator end_pos = remove(line.begin(), line.end(), ' ');
            line.erase(end_pos, line.end());

            if (line == "")
                continue;

            if (parser == SYMBOLS)
            {
                smatch results;
                if (regex_search(line, results, symbolStateRegex))
                {
                    line = line.substr(8);
                    sregex_token_iterator iter(line.begin(), line.end(), symbolRegex, 0);
                    sregex_token_iterator end;

                    env->add_symbols(parse_symbols(iter->str()));  // fixed

                    parser = INITIAL;
                }
                else
                {
                    cout << "Symbols are not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == INITIAL)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, initialConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
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
                    cout << "Initial conditions not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == GOAL)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, goalConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
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
                    cout << "Goal conditions not specified correctly." << endl;
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
                    cout << "Actions not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_DEFINITION)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, conditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;
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
                    cout << "Action not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_PRECONDITION)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, precondRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
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
                    cout << "Precondition not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_EFFECT)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, effectRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
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
                    cout << "Effects not specified correctly." << endl;
                    throw;
                }
            }
        }
        input_file.close();
    }

    else
        cout << "Unable to open file";

    return env;
}



/// Calculate all permutation of actions
void PermutationsUtil(unordered_set<string> symbols, int num, list<string> temp, vector<list<string>>& ans) {

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

vector<list<string>> Permutations(unordered_set<string> symbols, int num) {

    list<string> temp;
    vector<list<string>> ans;
    PermutationsUtil(symbols, num, temp, ans);
    return ans;
}


vector<vector<list<string>>> generate_depth_tree(const unordered_set<string>& symbols) {

    vector<vector<list<string>>> depth_tree;
    for (int i = 1; i <= symbols.size(); i++)
        depth_tree.push_back(Permutations(symbols, i));
    return depth_tree;
}

void create_cond(unordered_set<Condition, ConditionHasher, ConditionComparator> conds, GroundedAction& ga, unordered_map<string, string>& arg_mapping, int temp) {
    for (Condition i : conds) {
        list<string> args;
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

list<GroundedAction> action_space(const unordered_set<string>& symbols, const unordered_set<Action, ActionHasher, ActionComparator>& actions) {

    auto all_permutations = generate_depth_tree(symbols);
    list<GroundedAction> all_actions;

    for (auto action : actions) {
        int num_args = action.get_args().size() - 1;
        unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions = action.get_preconditions();
        unordered_set<Condition, ConditionHasher, ConditionComparator> effects = action.get_effects();
        list<string> arg_names = action.get_args();

        for (auto args : all_permutations[num_args]) {
            GroundedAction ga(action.get_name(), args);
            unordered_map<string, string> map_args;

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


typedef unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> g_state;


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
        return hash<string> {} (state_first.toString());
    }
};


class Node {
private:
    Node* prev_node;
    g_state state;
    GroundedAction action;
    int gval;
    int hval;
    int fval;

public:
    Node() {}
    Node(g_state s, Node* p, int g, int h) {
        state = s;
        prev_node = p;
        gval = g;
        hval = h;
    }
    void set_prev_node(Node* p) { prev_node = p; }
    void set_action(GroundedAction act) { action = act; }

    Node* get_prev_node() { return prev_node; }
    g_state get_state() { return state; }
    GroundedAction get_action() { return action; }

    void set_gval(int cost) { gval = cost; }
    void set_hval(int heuristic) { hval = heuristic; }
    void set_fval(int f) { fval = f; }

    int get_gval() { return gval; }
    int get_hval() { return hval; }
    int get_fval() { return fval; }

};

struct min_heap_f {
    bool operator()(Node* p1, Node* p2)
    {
        return p1->get_fval() > p2->get_fval();
    }
};


int valid_move(GroundedAction action, g_state curr_state) {

    for (auto a : action.getPreconditions()) {
        if (curr_state.find(a) == curr_state.end()) {
            return 0;
        }
    }
    return 1;
}


int state_compare(g_state start, g_state goal)
{
    for (GroundedCondition gc : goal) {
        if (start.find(gc) == start.end())
            return 0;

    }
    return 1;
}


g_state get_new_state(GroundedAction action, g_state curr_state) {

    for (auto eff : action.getEffects()) {

        if (eff.get_truth()) {
            curr_state.insert(eff);
        }
        else {
            eff.set_truth(true);
            curr_state.erase(eff);
        }
    }
    return curr_state;
}


int comp_heuristic(g_state state, g_state goal) {
    int val = 0;
    for (GroundedCondition gc : goal) {
        if (state.find(gc) == state.end())
            val++;
    }
    return val;
}

int comp_heuristic_ad(list<GroundedAction>all_actions, g_state start_state, g_state goal_state)
{
    priority_queue<Node*, vector<Node*>, min_heap_f> OPEN;
    unordered_set<g_state, g_state_hasher, g_state_comparison>CLOSED;

    int goal_reached = 0;
    Node* goal_node = (Node*)malloc(sizeof(Node));
    Node* start_node = new Node(start_state, NULL, 0, 0);

    OPEN.push(start_node);
    int num = 0;

    while (!OPEN.empty()) {

        Node* curr = OPEN.top();
        OPEN.pop();
        if (CLOSED.find(curr->get_state()) == CLOSED.end()) {
            CLOSED.insert(curr->get_state());

            for (auto action : all_actions) {
                if (valid_move(action, curr->get_state())) {
                    g_state new_state = get_new_state(action, curr->get_state());
                    Node* new_node = new Node(new_state, curr, curr->get_gval() + 1, 0);
                    new_node->set_fval(new_node->get_gval());
                    new_node->set_action(action);
                    OPEN.push(new_node);
                    if (state_compare(new_node->get_state(), goal_state)) {
                        return new_node->get_gval();

                    }

                }

            }

        }

    }
    return 0;
}

list<GroundedAction> planner(Env* env)
{

    int heur = 1;
    list<GroundedAction>actions1;
    list<GroundedAction>all_actions = action_space(env->get_symbols(), env->get_actions());

    priority_queue<Node*, vector<Node*>, min_heap_f> OPEN;
    unordered_map<g_state, Node*, g_state_hasher, g_state_comparison>VISITED;
    unordered_set<g_state, g_state_hasher, g_state_comparison>CLOSED;


    auto t_start = clock();
    int goal_reached = 0;
    int num = 0;

    g_state goal_state = env->get_goal_conditions();
    Node* goal_node = (Node*)malloc(sizeof(Node));
    g_state start_state = env->get_initial_conditions();
    Node* start_node = new Node(start_state, NULL, 0, 0);
    start_node->set_fval(start_node->get_hval());

    OPEN.push(start_node);
    VISITED.insert({ start_state, start_node });

    while (!OPEN.empty() && !goal_reached) {


        Node* curr = OPEN.top();
        OPEN.pop();

        if (CLOSED.find(curr->get_state()) == CLOSED.end()) {
            CLOSED.insert(curr->get_state());
            num++;


            for (auto action : all_actions) {
                if (valid_move(action, curr->get_state())) {
                    g_state new_state = get_new_state(action, curr->get_state());

                    if (!VISITED.count(new_state)) {
                        Node* new_node = (Node*)malloc(sizeof(Node));
                        if (heur == 0) {
                            new_node = new Node(new_state, curr, curr->get_gval() + 1, 0);
                            new_node->set_fval(new_node->get_gval());
                        }
                        if (heur == 1) {
                            new_node = new Node(new_state, curr, curr->get_gval() + 1, comp_heuristic(new_state, goal_state));
                            new_node->set_fval(new_node->get_gval() + new_node->get_hval());
                        }
                        if (heur == 2) {
                            new_node = new Node(new_state, curr, curr->get_gval() + 1, 2 * comp_heuristic_ad(all_actions, new_state, goal_state));
                            new_node->set_fval(new_node->get_gval() + new_node->get_hval());
                        }


                        new_node->set_action(action);
                        VISITED.insert({ new_state, new_node });
                        OPEN.push(new_node);

                        if (state_compare(new_node->get_state(), goal_state)) {
                            goal_reached = 1;
                            goal_node = new_node;
                            break;
                        }
                    }

                    else {

                        if (CLOSED.find(new_state) == CLOSED.end()) {

                            if (VISITED[new_state]->get_gval() > curr->get_gval() + 1) {
                                VISITED[new_state]->set_gval(curr->get_gval() + 1);
                                VISITED[new_state]->set_prev_node(curr);
                                VISITED[new_state]->set_action(action);
                                if (heur == 0) {
                                    VISITED[new_state]->set_fval(VISITED[new_state]->get_gval());
                                }
                                if (heur == 1) {
                                    VISITED[new_state]->set_fval(VISITED[new_state]->get_gval() + VISITED[new_state]->get_hval());
                                }
                                if (heur == 2) {
                                    VISITED[new_state]->set_fval(VISITED[new_state]->get_gval() + 2 * VISITED[new_state]->get_hval());
                                }
                                OPEN.push(VISITED[new_state]);

                            }

                        }

                    }

                }

            }
            if (goal_reached) {
                
                while (goal_node->get_prev_node() != NULL) {
                    actions1.push_back(goal_node->get_action());
                    goal_node = goal_node->get_prev_node();


                }
                actions1.reverse();

                auto t_end = (float)(clock() - t_start) / CLOCKS_PER_SEC;
                cout << "Time taken: " << t_end << " seconds" << endl;
                return actions1;
            }

        }

    }
    return actions1;
}





























int main(int argc, char* argv[])
{
    // DO NOT CHANGE THIS FUNCTION
    char* filename = (char*)("FireExtinguisher.txt");
    if (argc > 1)
        filename = argv[1];

    cout << "Environment: " << filename << endl << endl;
    Env* env = create_env(filename);
    if (print_status)
    {
        cout << *env;
    }

    list<GroundedAction> actions = planner(env);

    cout << "\nPlan: " << endl;
    for (GroundedAction gac : actions)
    {
        cout << gac << endl;
    }

    return 0;
}