#include "Pathfinding.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <queue>
#include <unordered_set>
#include <unordered_map>

#include "world/Level.hpp"
#include "voxels/GlobalChunks.hpp"
#include "voxels/Chunk.hpp"
#include "voxels/blocks_agent.hpp"
#include "content/Content.hpp"

using namespace voxels;

struct Node {
    glm::ivec3 pos;
    glm::ivec3 parent;
    float gScore;
    float fScore;
};

struct NodeLess {
    bool operator()(const Node& l, const Node& r) const {
        return l.fScore > r.fScore;
    }
};

static float distance(const glm::ivec3& a, const glm::ivec3& b) {
    return glm::distance(glm::vec3(a), glm::vec3(b));
}

Pathfinding::Pathfinding(const Level& level)
    : level(level), chunks(*level.chunks) {
}

static bool check_passability(
    const Agent& agent,
    const GlobalChunks& chunks,
    const Node& node,
    const glm::ivec2& offset,
    bool diagonal
) {
    if (!diagonal) {
        return true;
    }
    auto a = node.pos + glm::ivec3(offset.x, 0, 0);
    auto b = node.pos + glm::ivec3(0, 0, offset.y);

    for (int i = 0; i < agent.height; i++) {
        if (blocks_agent::is_obstacle_at(chunks, a.x, a.y + i, a.z))
            return false;
        if (blocks_agent::is_obstacle_at(chunks, b.x, b.y + i, b.z))
            return false;
    }
    return true;
}

static void restore_route(
    Route& route,
    const Node& node,
    const std::unordered_map<glm::ivec3, Node>& parents
) {
    auto pos = node.pos;
    while (true) {
        const auto& found = parents.find(pos);
        if (found == parents.end()) {
            route.nodes.push_back({pos});
            break;
        }
        route.nodes.push_back({pos});
        pos = found->second.pos;
    }
}

int Pathfinding::createAgent() {
    int id = nextAgent++;
    agents[id] = Agent();
    return id;
}

Route Pathfinding::perform(
    const Agent& agent, const glm::ivec3& start, const glm::ivec3& end
) {
    using namespace blocks_agent;

    Route route {};

    std::priority_queue<Node, std::vector<Node>, NodeLess> queue;
    queue.push({start, {}, 0, distance(start, end)});

    std::unordered_set<glm::ivec3> blocked;
    std::unordered_map<glm::ivec3, Node> parents;

    const auto& chunks = *level.chunks;
    int height = std::max(agent.height, 1);

    while (!queue.empty()) {
        if (blocked.size() == agent.maxVisitedBlocks) {
            break;
        }
        auto node = queue.top();
        queue.pop();

        if (node.pos.x == end.x && glm::abs((node.pos.y - end.y) / height) == 0 && node.pos.z == end.z) {
            restore_route(route, node, parents);
            route.nodes.push_back({start});
            route.found = true;
            break;
        }

        blocked.emplace(node.pos);
        glm::ivec2 neighbors[8] {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0},
            {-1, -1}, {1, -1}, {1, 1}, {-1, 1},
        };

        for (int i = 0; i < sizeof(neighbors) / sizeof(glm::ivec2); i++) {
            auto offset = neighbors[i];
            auto pos = node.pos;

            int surface = getSurfaceAt(pos + glm::ivec3(offset.x, 0, offset.y), 1);

            if (surface == -1) {
                continue;
            }
            pos.y = surface;
            auto point = pos + glm::ivec3(offset.x, 0, offset.y);

            if (is_obstacle_at(chunks, pos.x, pos.y + agent.height / 2, pos.z)) {
                continue;
            }
            if (!check_passability(agent, chunks, node, offset, i >= 4)) {
                continue;
            }
            if (blocked.find(point) != blocked.end()) {
                continue;
            }
            int score = glm::abs(node.pos.y - pos.y);
            float gScore =
                node.gScore + glm::abs(offset.x) + glm::abs(offset.y) + score;
            const auto& found = parents.find(point);
            if (found == parents.end() || gScore < found->second.gScore) {
                float hScore = distance(point, end);
                float fScore = gScore + hScore;
                Node nNode {point, node.pos, gScore, fScore};
                parents[point] = Node {node.pos, node.parent, gScore, fScore};
                queue.push(nNode);
            }
        }
    }
    return route;
}

Agent* Pathfinding::getAgent(int id) {
    const auto& found = agents.find(id);
    if (found != agents.end()) {
        return &found->second;
    }
    return nullptr;
}

const std::unordered_map<int, Agent>& Pathfinding::getAgents() const {
    return agents;
}

static int check_point(
    const ContentUnitIndices<Block>& defs,
    const GlobalChunks& chunks,
    int x,
    int y,
    int z
) {
    auto vox = blocks_agent::get(chunks, x, y, z);
    if (vox == nullptr) {
        return 0;
    }
    const auto& def = defs.require(vox->id);
    if (def.obstacle) {
        return 0;
    }
    if (def.translucent) {
        return -1;
    }
    return 1;
}

int Pathfinding::getSurfaceAt(const glm::ivec3& pos, int maxDelta) {
    using namespace blocks_agent;

    const auto& defs = level.content.getIndices()->blocks;

    int status;
    int surface = pos.y;
    if (check_point(defs, chunks, pos.x, surface, pos.z) <= 0) {
        if (check_point(defs, chunks, pos.x, surface + 1, pos.z) <= 0)
            return -1;
        else
            return surface + 1;
    } else if ((status = check_point(defs, chunks, pos.x, surface - 1, pos.z)) <= 0) {
        if (status == -1)
            return -1;
        return surface;
    } else if (check_point(defs, chunks, pos.x, surface - 2, pos.z) == 0) {
        return surface - 1;
    }
    return -1;
}
