#include "Config.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <type_traits>

SimulationConfig Config::simulation;
RenderingConfig Config::rendering;
SurfaceConfig Config::surface;
ResourcesConfig Config::resources;
CivilizationConfig Config::civilization;
HumansConfig Config::humans;
BuildingsConfig Config::buildings;
ThreadsConfig Config::threads;

namespace
{
using Section = std::unordered_map<std::string, std::string>;
using IniData = std::unordered_map<std::string, Section>;

std::string trim(const std::string& str)
{
    const auto first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
        return {};

    const auto last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

IniData parseIni(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error(
            "Failed to open configuration file: " + path
        );
    }

    IniData data;

    std::string line;
    std::string currentSection;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            currentSection =
                trim(line.substr(1, line.size() - 2));

            continue;
        }

        const auto separator = line.find('=');

        if (separator == std::string::npos)
        {
            throw std::runtime_error(
                "Invalid configuration line: " + line
            );
        }

        const std::string key =
            trim(line.substr(0, separator));

        std::string value =
            trim(line.substr(separator + 1));

        const auto comment = value.find_first_of("#;");

        if (comment != std::string::npos)
            value = trim(value.substr(0, comment));

        data[currentSection][key] = value;
    }

    return data;
}

template <typename T>
T get(
    const IniData& data,
    const std::string& section,
    const std::string& key)
{
    const auto sectionIt = data.find(section);

    if (sectionIt == data.end())
    {
        throw std::runtime_error(
            "Missing configuration section [" +
            section + "]"
        );
    }

    const auto valueIt = sectionIt->second.find(key);

    if (valueIt == sectionIt->second.end())
    {
        throw std::runtime_error(
            "Missing configuration value: [" +
            section + "] " + key
        );
    }

    std::stringstream stream(valueIt->second);

    T value{};

    if constexpr (std::is_same_v<T, bool>)
    {
        stream >> std::boolalpha >> value;
    }
    else
    {
        stream >> value;
    }

    if (stream.fail())
    {
        throw std::runtime_error(
            "Invalid value for [" +
            section + "] " +
            key +
            ": " +
            valueIt->second
        );
    }

    return value;
}

}

void Config::load(const std::string& path)
{
const IniData ini = parseIni(path);

simulation.ticksForCivilizationDecision =
    get<int>(
        ini,
        "simulation",
        "ticks_for_civilization_decision"
    );

simulation.ticksForBuildingDecision =
    get<int>(
        ini,
        "simulation",
        "ticks_for_building_decision"
    );

simulation.ticksForAssigningDecision =
    get<int>(
        ini,
        "simulation",
        "ticks_for_assigning_decision"
    );

simulation.ticksForResourcesGainsFromBuildings =
    get<int>(
        ini,
        "simulation",
        "ticks_for_resources_gains_from_buildings"
    );

simulation.ticksForNewHumans =
    get<int>(
        ini,
        "simulation",
        "ticks_for_new_humans"
    );

rendering.fps =
    get<float>(
        ini,
        "rendering",
        "fps"
    );

rendering.windowSizeX =
    get<int>(
        ini,
        "rendering",
        "window_size_x"
    );

rendering.windowSizeY =
    get<int>(
        ini,
        "rendering",
        "window_size_y"
    );

surface.landPercent =
    get<int>(
        ini,
        "surface",
        "land_percent"
    );

surface.mountainPercent =
    get<int>(
        ini,
        "surface",
        "mountain_percent"
    );

surface.sandPercent =
    get<int>(
        ini,
        "surface",
        "sand_percent"
    );

surface.numberOfDeserts =
    get<int>(
        ini,
        "surface",
        "number_of_deserts"
    );

resources.food.count =
    get<int>(
        ini,
        "resources.food",
        "count"
    );

resources.food.max =
    get<int>(
        ini,
        "resources.food",
        "max"
    );

resources.food.respawn =
    get<int>(
        ini,
        "resources.food",
        "respawn"
    );

resources.food.maxSpawnTries =
    get<int>(
        ini,
        "resources.food",
        "max_spawn_tries"
    );

resources.trees.count =
    get<int>(
        ini,
        "resources.trees",
        "count"
    );

resources.trees.forestCount =
    get<int>(
        ini,
        "resources.trees",
        "forest_count"
    );

resources.trees.max =
    get<int>(
        ini,
        "resources.trees",
        "max"
    );

resources.trees.respawn =
    get<int>(
        ini,
        "resources.trees",
        "respawn"
    );

resources.trees.maxSpawnTries =
    get<int>(
        ini,
        "resources.trees",
        "max_spawn_tries"
    );

resources.stone.count =
    get<int>(
        ini,
        "resources.stone",
        "count"
    );

resources.stone.max =
    get<int>(
        ini,
        "resources.stone",
        "max"
    );

resources.stone.respawn =
    get<int>(
        ini,
        "resources.stone",
        "respawn"
    );

resources.stone.maxSpawnTries =
    get<int>(
        ini,
        "resources.stone",
        "max_spawn_tries"
    );

civilization.spawnChunkX =
    get<uint16_t>(
        ini,
        "civilization",
        "spawn_chunk_x"
    );

civilization.spawnChunkY =
    get<uint16_t>(
        ini,
        "civilization",
        "spawn_chunk_y"
    );

civilization.partOfHumansChangingJobs =
    get<int>(
        ini,
        "civilization",
        "part_of_humans_changing_jobs"
    );

humans.count =
    get<int>(
        ini,
        "humans",
        "count"
    );

humans.grain =
    get<int>(
        ini,
        "humans",
        "grain"
    );

humans.range =
    get<int>(
        ini,
        "humans",
        "range"
    );

humans.maxSpawnRange =
    get<int>(
        ini,
        "humans",
        "max_spawn_range"
    );

buildings.house.stoneRequired =
    get<int>(
        ini,
        "buildings.house",
        "stone_required"
    );

buildings.house.woodRequired =
    get<int>(
        ini,
        "buildings.house",
        "wood_required"
    );

buildings.house.maxHumans =
    get<int>(
        ini,
        "buildings.house",
        "max_humans"
    );

buildings.farm.foodRequired =
    get<int>(
        ini,
        "buildings.farm",
        "food_required"
    );

buildings.farm.woodRequired =
    get<int>(
        ini,
        "buildings.farm",
        "wood_required"
    );

buildings.farm.maxSpawnTries =
    get<int>(
        ini,
        "buildings.farm",
        "max_spawn_tries"
    );

buildings.farm.maxWorkers =
    get<int>(
        ini,
        "buildings.farm",
        "max_workers"
    );

buildings.farm.foodPerWorker =
    get<float>(
        ini,
        "buildings.farm",
        "food_per_worker"
    );

buildings.sawmill.stoneRequired =
    get<int>(
        ini,
        "buildings.sawmill",
        "stone_required"
    );

buildings.sawmill.woodRequired =
    get<int>(
        ini,
        "buildings.sawmill",
        "wood_required"
    );

buildings.sawmill.maxSpawnTries =
    get<int>(
        ini,
        "buildings.sawmill",
        "max_spawn_tries"
    );

buildings.sawmill.maxWorkers =
    get<int>(
        ini,
        "buildings.sawmill",
        "max_workers"
    );

buildings.sawmill.woodPerWorker =
    get<float>(
        ini,
        "buildings.sawmill",
        "wood_per_worker"
    );

buildings.mine.stoneRequired =
    get<int>(
        ini,
        "buildings.mine",
        "stone_required"
    );

buildings.mine.woodRequired =
    get<int>(
        ini,
        "buildings.mine",
        "wood_required"
    );

buildings.mine.maxSpawnTries =
    get<int>(
        ini,
        "buildings.mine",
        "max_spawn_tries"
    );

buildings.mine.maxWorkers =
    get<int>(
        ini,
        "buildings.mine",
        "max_workers"
    );

buildings.mine.stonePerWorker =
    get<float>(
        ini,
        "buildings.mine",
        "stone_per_worker"
    );

threads.cores =
    get<int>(
        ini,
        "threads",
        "cores"
    );

threads.threads =
    get<int>(
        ini,
        "threads",
        "threads"
    );

threads.coresForSimLoop =
    get<int>(
        ini,
        "threads",
        "coresForSimLoop"
    );

threads.threadsForSimLoop =
    get<int>(
        ini,
        "threads",
        "threadsForSimLoop"
    );

threads.coresForHumanLoop =
    get<int>(
        ini,
        "threads",
        "coresForHumanLoop"
    );

threads.threadsForHumanLoop =
    get<int>(
        ini,
        "threads",
        "threadsForHumanLoop"
    );

threads.readDeviceThreadCount =
    get<bool>(
        ini,
        "threads",
        "readDeviceThreadCount"
    );
}
