#include "symbolic_schedule/Planner.hpp"

int main(int argc, char **argv)
{
    bool print_status = true;
    // DO NOT CHANGE THIS FUNCTION
    char* filename = (char*)("FireExtinguisher.txt");
    if (argc > 1)
        filename = argv[1];

    std::cout << "Environment: " << filename << std::endl << std::endl;
    Env* env = create_env(filename);
    if (print_status)
    {
        std::cout << *env;
    }

    std::list<GroundedAction> actions = planner(env);

    std::cout << "\nPlan: " << std::endl;
    for (GroundedAction gac : actions)
    {
        std::cout << gac << std::endl;
    }
    return 0;
}