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

double Search::get_heuristics(std::pair<int, int> position, std::pair<int, int> goal, const EnvironmentOptions &options, const Config &config) {
    if (config.SearchParams[CN_SP_ST] == CN_SP_ST_DIJK) {
        return 0;
    } else if (config.SearchParams[CN_SP_ST] == CN_SP_ST_ASTAR) {
        if (options.metrictype == CN_SP_MT_DIAG) {
            double dx = abs(position.first - goal.first), dy = abs(position.second - goal.second);
            return std::min(dx, dy) * (CN_SQRT_TWO - 1) + std::max(dx, dy);
        } else if (options.metrictype == CN_SP_MT_MANH) {
            return abs(position.first - goal.first) + abs(position.second - goal.second);
        } else if (options.metrictype == CN_SP_MT_EUCL) {
            return hypot(position.first - goal.first, position.second - goal.second);
        } else if (options.metrictype == CN_SP_MT_CHEB) {
            return std::max(abs(position.first - goal.first), abs(position.second - goal.second));
        }
    }
    return 0;
}

std::set<Node, bool(*)(const Node&, const Node&)> Search::getFilteredOpen() const {
    auto result = OPEN;
    result.clear();
    std::set<std::pair<int, int>> used;
    for (auto node : OPEN) {
        if (used.find(node.position()) == used.end()) {
            used.insert(node.position());
            result.insert(node);
        }
    }
    return result;
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options, const Config &config) {
    if (config.SearchParams[CN_SP_ST] == CN_SP_ST_DIJK || config.SearchParams[CN_SP_BT] == CN_SP_BT_GMIN) {
        OPEN = std::set<Node, bool(*)(const Node&, const Node&)>(&NodeGminComparator);
    } else {
        OPEN = std::set<Node, bool(*)(const Node&, const Node&)>(&NodeGmaxComparator);
    }

    auto start_time = clock();

    sresult.pathfound = false;
    sresult.numberofsteps = 0;
    sresult.pathlength = 0;

    std::pair<int, int> goal_pos = map.getGoalPos();

    Node start(map.getStartPos());

    start.g = 0;
    start.h = get_heuristics(start.position(), goal_pos, options, config);
    start.f = start.g + start.h * config.SearchParams[CN_SP_HW];
    start.parent = nullptr;

    OPEN = {start};

    while (!OPEN.empty()) {
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
                node.parent = (Node*)&(*CLOSE.find(v));
                node.h = get_heuristics(node.position(), goal_pos, options, config);
                node.g += v.g;
                node.f = node.g + node.h * config.SearchParams[CN_SP_HW];

                OPEN.insert(node);
            }
        }
        if (Logger->getLogLevel() == CN_LP_LEVEL_FULL_WORD)
            Logger->writeToLogOpenClose(getFilteredOpen(), CLOSE, sresult.numberofsteps);
        ++sresult.numberofsteps;
    }

    auto old_close = CLOSE;

    for (auto node : OPEN) {
        if (CLOSE.find(node) == CLOSE.end()) {
            CLOSE.insert(node);
        }
    }
    sresult.nodescreated = CLOSE.size();

    if (Logger->getLogLevel() == CN_LP_LEVEL_MEDIUM_WORD)
        Logger->writeToLogOpenClose(getFilteredOpen(), old_close, 0);
    else if (Logger->getLogLevel() == CN_LP_LEVEL_FULL_WORD)
        Logger->writeToLogOpenClose(getFilteredOpen(), old_close, sresult.numberofsteps);

    if (sresult.pathfound) {
        lppath.clear();

        Node *node = (Node*)&(*CLOSE.find(Node(goal_pos)));

        sresult.pathlength = node->g;

        while (node != nullptr) {
            lppath.insert(lppath.begin(), *node);
            node = node->parent;
        }

        hppath.clear();
        auto it = lppath.begin();
        std::pair<int, int> last_direction = {0, 0};
        for (int i = 0; i < lppath.size(); ++i) {
            std::pair<int, int> direction = {0, 0};
            if (i + 1 != lppath.size()) {
                auto next = it;
                ++next;
                direction.first = next->i - it->i;
                direction.second = next->j - it->j;
            }
            if (i == 0 || i + 1 == lppath.size() || direction != last_direction) {
                hppath.insert(hppath.end(), *it);
            }
            ++it;
            last_direction = direction;
        }
    }

    sresult.time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
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
