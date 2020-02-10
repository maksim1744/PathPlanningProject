#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "node_comparator.h"
#include "searchresult.h"

#include <chrono>
#include <iomanip>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <set>
#include <vector>
#include <vector>

bool NodeGComparator(const Node &first, const Node &second);

class Search
{
    public:
        Search();
        ~Search(void);
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options, const Config &config);

    protected:
        std::vector<Node> get_neighbours(Node node, const Map &map, const EnvironmentOptions &options);
        double get_heuristics(std::pair<int, int> position, std::pair<int, int> goal, const EnvironmentOptions &options, const Config &config);
        std::set<Node, bool(*)(const Node&, const Node&)> getFilteredOpen() const;
        //CODE HERE

        //Hint 1. You definetely need class variables for OPEN and CLOSE

        //Hint 2. It's a good idea to define a heuristic calculation function, that will simply return 0
        //for non-heuristic search methods like Dijkstra

        //Hint 3. It's a good idea to define function that given a node (and other stuff needed)
        //will return it's sucessors, e.g. unordered list of nodes

        //Hint 4. working with OPEN and CLOSE is the core
        //so think of the data structures that needed to be used, about the wrap-up classes (if needed)
        //Start with very simple (and ineffective) structures like list or vector and make it work first
        //and only then begin enhancement!


        SearchResult                    sresult; //This will store the search result
        std::list<Node>                 lppath, hppath; //

        std::set<Node, bool(*)(const Node&, const Node&)> OPEN;
        std::set<Node, NodePosComparator> CLOSE;

        //CODE HERE to define other members of the class
};
#endif
