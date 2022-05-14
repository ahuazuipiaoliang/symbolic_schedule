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
#include "symbolic_schedule/Node.hpp"

std::list<GroundedAction> planner(Env* env)
{
    int heur = 1;
    std::list<GroundedAction>actions1;
    std::list<GroundedAction>all_actions = action_space(env->get_symbols(), env->get_actions());

    std::priority_queue<Node*, std::vector<Node*>, min_heap_f> OPEN;
    std::unordered_map<g_state, Node*, g_state_hasher, g_state_comparison>VISITED;
    std::unordered_set<g_state, g_state_hasher, g_state_comparison>CLOSED;


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
                std::cout << "Time taken: " << t_end << " seconds" << std::endl;
                return actions1;
            }
        }
    }
    return actions1;
}