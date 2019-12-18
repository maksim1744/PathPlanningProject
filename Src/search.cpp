#include "search.h"

Search::Search() {
//set defaults here
}

Search::~Search() {}


std::vector<Node> Search::get_neighbours(Node from, const Map &map, const EnvironmentOptions &options) {
    std::vector<Node> result;

    std::vector<std::pair<int, int>> near = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1},
    };

    for (auto dij : near) {
        int di = dij.first, dj = dij.second;
        int new_i = from.i + di, new_j = from.j + dj;
        if (new_i < 0 || new_j < 0 || new_i >= map.getMapWidth() || new_j >= map.getMapHeight())
            continue;
        if (map.CellIsTraversable(new_i, new_j)) {
            Node node(std::make_pair(new_i, new_j));
            node.g = 1;
            if (di != 0 && dj != 0) {
                if (!options.allowdiagonal)
                    continue;
                int corners = 0;
                if (map.CellIsObstacle(from.i + di, from.j))
                    corners++;
                if (map.CellIsObstacle(from.i, from.j + dj))
                    corners++;
                if (corners >= 1 && !options.cutcorners)
                    continue;
                if (corners >= 2 && !options.allowsqueeze)
                    continue;
                node.g = CN_SQRT_TWO;
            }
            result.push_back(node);
        }
    }

    return std::move(result);
}

double Search::get_heuristics(std::pair<int, int> position, std::pair<int, int> goal, const EnvironmentOptions &options) {
    return 0;
    // return hypot(position.first - goal.first, position.second - goal.second);
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    sresult.pathfound = false;
    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.pathlength = 0;

    std::pair<int, int> goal_pos = map.getGoalPos();

    Node start(map.getStartPos());

    start.g = 0;
    start.h = get_heuristics(start.position(), goal_pos, options);
    start.f = start.g + start.h;
    start.parent = nullptr;

    OPEN = {start};

    while (!OPEN.empty()) {
        ++sresult.numberofsteps;
        Node v = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        if (CLOSE.find(v) != CLOSE.end()) {
            continue;
        }
        CLOSE.insert(v);
        if (v.position() == goal_pos) {
            sresult.pathfound = true;
            break;
        }
        std::vector<Node> neighbours = get_neighbours(v, map, options);
        for (auto& node : neighbours) {
            auto it = CLOSE.find(node);
            if (it == CLOSE.end()) {
                ++sresult.nodescreated;

                node.parent = (Node*)&(*CLOSE.find(v));
                node.h = get_heuristics(node.position(), goal_pos, options);
                node.g += v.g;
                node.f = node.h + node.g;

                OPEN.insert(node);
            }
        }
    }

    if (sresult.pathfound) {
        lppath.clear();

        Node *node = (Node*)&(*CLOSE.find(Node(goal_pos)));

        sresult.pathlength = node->g;

        while (node != nullptr) {
            lppath.insert(lppath.begin(), *node);
            node = node->parent;
        }
    }

    sresult.time = 0;
    sresult.hppath = &hppath;
    sresult.lppath = &lppath;

    return sresult;
}

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
