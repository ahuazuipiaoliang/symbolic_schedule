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
#include "symbolic_schedule/Utils.hpp"


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

int comp_heuristic_ad(std::list<GroundedAction>all_actions, g_state start_state, g_state goal_state)
{
    std::priority_queue<Node*, std::vector<Node*>, min_heap_f> OPEN;
    std::unordered_set<g_state, g_state_hasher, g_state_comparison>CLOSED;

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