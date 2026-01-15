#pragma once
#include <unordered_map>
#include "gameplay/gameplay_components.h"
#include <map>
#include <optional>

class Navmesh;

class Influence 
{
private:
    std::shared_ptr<Navmesh> m_navmesh;
    float acc = 1.f;
    float m_momentum = 0.3f;
    float m_decay = 0.05f;
    std::unordered_map<int, InfluenceSource> m_influences;
    int m_nextUserID = static_cast<int>(InfluenceType::USER_DEFINED_START);
    std::map<int, const char*> m_nameMap;
    bool m_propagatePlayer = false;
    void Propagation(std::unordered_map<int, float>& influenceLayer);
    void FloodPropagation(std::unordered_map<int, float>& map, const int iteration, const size_t maxHeatCell);
    void ShowInfluence(const glm::vec2& worldMousePos, InfluenceType type, const glm::vec2& offset);
    void ShowInfluence(const glm::vec2& worldMousePos, int id, const glm::vec2& offset);
    void DecayInfluence(InfluenceType type, float decayRate);
    void DecayInfluence(int id, const float decayRate);

public:
    Influence(const std::shared_ptr<Navmesh>& navmesh);
    ~Influence();
    void Update(float deltaTime);
    /// <summary>
    /// Register a new influence type and return its ID
    /// </summary>
    /// <param name="name">Name of the new influence type.</param>
    /// <param name="isStatic"> If the new type should be registered as a static influence</param>
    /// <returns>Registerd influence's Id</returns>
    int RegisterType(const char* name, bool isStatic = false);

    /// <summary>
    /// Adds a static influence at the given position, optionally propagating it through the influence field.
    /// </summary>
    /// <param name="pos">Position where the influence is applied.</param>
    /// <param name="type">Type of the influence to apply.</param>
    /// <param name="value">Value of the influence. Defaults to 1.0f.</param>
    /// <param name="propagate">Whether the influence should be propagated to surrounding areas. Defaults to true.</param>
    /// <param name="flood">If true, use a flood-fill style propagation for broader spreading; otherwise use standard
    /// propagation. Defaults to false.</param> <param name="iteration">Number of propagation iterations or depth used when
    /// spreading the influence. Defaults to 5.</param>
    void AddStaticInfluence(const glm::vec2& pos,
                            InfluenceType type,
                            float value = 1.f,
                            bool propagate = true,
                            bool flood = false,
                            int iteration = 5);

    /// <summary>
    /// Adds a static influence at the given position, optionally propagating it through the influence field.
    /// </summary>
    /// <param name="pos">Position where the influence is applied.</param>
    /// <param name="id">Id of the influence to apply.</param>
    /// <param name="value">Value of the influence. Defaults to 1.0f.</param>
    /// <param name="propagate">Whether the influence should be propagated to surrounding areas. Defaults to true.</param>
    /// <param name="flood">If true, use a flood-fill style propagation for broader spreading; otherwise use standard
    /// propagation. Defaults to false.</param> <param name="iteration">Number of propagation iterations or depth used when
    /// spreading the influence. Defaults to 5.</param>
    void AddStaticInfluence(const glm::vec2& pos,
                            int id,
                            float value = 1.f,
                            bool propagate = true,
                            bool flood = false,
                            int iteration = 5);

    /// <summary>
    /// Adds a dynamic influence at the given position.
    /// </summary>
    /// <param name="pos">Position where the influence is applied.</param>
    /// <param name="type">Type of the influence to apply.</param>
    /// <param name="value">Value of the influence. Defaults to 1.0f.</param>
    void AddDynamicInfluence(const glm::vec2& pos, InfluenceType type, float value = 1.f);

    /// <summary>
    /// Adds a dynamic influence at the given position.
    /// </summary>
    /// <param name="pos">Position where the influence is applied.</param>
    /// <param name="id">Id of the influence to apply.</param>
    /// <param name="value">Value of the influence. Defaults to 1.0f.</param>
    void AddDynamicInfluence(const glm::vec2& pos, int id, float value = 1.f);

    /// <summary>
    /// Returns a const reference to the layer map for the specified id.
    /// </summary>
    /// <param name="id">Id of the desired layer. Must refer to a valid entry; otherwise the underlying container's at() will
    /// throw std::out_of_range.</param> <returns>The influence layer of the given id by const reference.</returns>
    inline const std::unordered_map<int, float>& GetLayer(int id) const { return m_influences.at(id).layer; }

    /// <summary>
    /// Returns a const reference to the layer map for the specified id.
    /// </summary>
    /// <param name="type">Type of the desired layer. Must refer to a valid entry; otherwise the underlying container's at()
    /// will throw std::out_of_range.</param> <returns>The influence layer of the given type by const reference.</returns>
    inline const std::unordered_map<int, float>& GetLayer(InfluenceType type) const { return GetLayer(static_cast<int>(type)); }

    /// <summary>
    /// Returns the vertex with the lowest influence value from the given map.
    /// </summary>
    /// <param name="map">The map that the function should operate on.</param>
    /// <returns>The vertex with the lowest influence on the map.</returns>
    int GetLowestInfluence(std::unordered_map<int, float>& map) const;

    /// <summary>
    /// Returns the vertex with the highest influence value from the given map.
    /// </summary>
    /// <param name="map">The map that the function should operate on.</param>
    /// <returns>The vertex with the highest influence on the map.</returns>
    int GetHighestInfluence(const std::unordered_map<int, float>& map) const;

    /// <summary>
    /// Multiplies all influence values in the given layer by the specified multiplier.
    /// </summary>
    /// <param name="layer">Influence layer that should be multiplies</param>
    /// <param name="multi">Multiplier value</param>
    void MultiplyLayer(std::unordered_map<int, float>& layer, const float multi) const;

    /// <summary>
    /// Adds values from layer2 into layer1. Therefore, layer1 will be sum of both layers after this operation.
    /// </summary>
    /// <param name="layer1">Reference to the map that will be modified.</param>
    /// <param name="layer2">Constant reference to the map whose values will be added into layer1.</param>
    void AdditionLayers(std::unordered_map<int, float>& layer1, const std::unordered_map<int, float>& layer2) const;

    /// <summary>
    /// Subtract values of layer2 from layer1. Therefore, layer1 will be difference of both layers after this operation.
    /// </summary>
    /// <param name="layer1">Reference to the map that will be modified.</param>
    /// <param name="layer2">Constant reference to the map whose values will be subtracted from layer1.</param>
    void SubtractLayers(std::unordered_map<int, float>& layer1, const std::unordered_map<int, float>& layer2) const;

    /// <summary>
    /// Called when sight of a target is lost. Helps to predict target's position.
    /// </summary>
    /// <param name="position">The position at which sight was lost.</param>
    /// <param name="lastDirection">The last known direction of the target.</param>
    void OnLoseSight(const glm::vec2& position, const glm::vec2& lastDirection);

    /// <summary>
    /// Propagate static influence on target's position.
    /// </summary>
    /// <param name="pos">Target's position.</param>
    void OnHaveSight(const glm::vec2& pos);

    /// <summary>
    /// Returns the average position for influences of the specified type.
    /// </summary>
    /// <param name="type">Type of the influence to operate on.</param>
    /// <returns>std::optional containing the average glm::vec2 (x, y) position of the selected influences, or std::nullopt if no matching influences are present.</returns>
    std::optional<glm::vec2> GetAvgPosition(InfluenceType type);

    /// <summary>
    /// Returns the average position for influences of the specified type.
    /// </summary>
    /// <param name="id">Id of the influence to operate on.</param>
    /// <returns>std::optional containing the average glm::vec2 (x, y) position of the selected influences, or std::nullopt if
    /// no matching influences are present.</returns>
    std::optional<glm::vec2> GetAvgPosition(int id);

    /// <summary>
    /// Clears or resets any active influence of the specified type.
    /// </summary>
    /// <param name="type">The influence type to clear.</param>
    void ClearInfluence(InfluenceType type);

    /// <summary>
    /// Clears or resets any active influence of the specified id.
    /// </summary>
    /// <param name="id">The influence id to clear.</param>
    void ClearInfluence(int id);

    /// <summary>
    /// Sets propagation algorithm's momentum value.
    /// </summary>
    /// <param name="momentum">Momentum value</param>
    inline void SetMomentum(float momentum) { m_momentum = momentum; }

    /// <summary>
    /// Sets propagation algorithm's decay value.
    /// </summary>
    /// <param name="momentum">Decay value</param>
    inline void SetDecay(float decay) { m_decay = decay; }

    void Draw();
};
