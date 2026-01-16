# Influence Maps in Game AI

**by Hossein Tayebi**

*Breda University of Applied Sciences*  

## Abstract

This research is focused on using influence map to help game AI navigate better and act smarter in conclusion of using influence data. Influence map has become a technique that is being used in many games to achieve this goal.

In this article, I will break down the impacts of influence map on game AI, different algorithms available and its usage in the game industry.

---

## Table of Contents

1. [Introduction](#introduction)
2. [How influence map works](#how-influence-map-works)
3. [If you should use influence map](#if-you-should-use-influence-map)
4. [Methodology](#methodology)
5. [Implementation](#implementation)
6. [Results](#results)
7. [Conclusion](#conclusion)
8. [References](#references)

---

## Introduction

### Motivation

Game AI plays a crucial role when it comes to making player's experience immersive and since nowadays, most games feature some form of AI such as AI companions, enemies or NPCs, it's essential to make these AIs smarter in order to improve quality of player's experience and make it more challenging. Influence map impacts:

- **Environmental awareness:** Influence map can be used to increase the ai's awareness by inserting data and summarizing what's happening around them. This helps them with their decision-making
- **Prediction:** Influence map can also be used for predicting future events. Like guessing where the player might be based on the last seen position and direction.
- **Memory:** Influence data can be used as some sort of memory where the ai uses the previously inserted data to know what happened and how to act.

### Scope

- What is influence map
- When should you use influence map and when not to 
- Different approaches in the industry
- My implementation


---

## How influence map works

### Influence Mapping

Influence map can be represented in any kind of map like 2d grids, graph, area graph and ... but it is important to have a navigation mesh which influence map can operate on, because basically how influence map works is that it inserts data on a certain vertex then propagates that through the map.

### Propagation Algorithms

The main concept behind influence map is propagation. It means how you spread data over the map. After doing my research, I came across 2 different propagation algorithm that are mostly used in the industry.
- Diffusion-based propagation
- Flood-fill propagation

Both have their own pros and cons, which I'll talk about it in the upcoming sections.

### Existing Implementations
![alt text](image.png)

This is from Laurent Couvidou's article on Dishonored 2 implementation, which shows how influence propagates in order to estimate where the player will be. I'll explain the algorithm in the implementation section.

![alt text](image-2.png)

Influence map on a 2d grid

---

## If you should use influence map
Using influence map is not always the best choice for the game because in some games, the trade-offs are just not worth it. If the world is too simple, meaning the map is small and there aren't lots of terrains on the map, it is a sign that you might not really need an influence map! These kinds of worlds/maps can be simplified by distance-based calculation instead of benefiting from influence map.

Another factor to consider when you wanna implement influence map is the type of map you're using. Whether you are using 2d grid, waypoint graph, area graph or coarse grid, they can all benefit from influence map, but it's important to take the pros and cons of each map into account because on some maps, like 2d grid it's easier to implement influence map and have more usage, while on others might not be as effective.

---

## Methodology

### System Architecture

![alt text](image-3.png)                              

The diagram above can summarize my architecture for implementing influence map. Since the map I chose is a waypoint graph, navmesh needs to construct a graph using the file data that is passed to navmesh where the data(Vertices and edges) is extracted in graph's constructor. Of course, you can always create a graph outside of navmesh then pass it as a pointer but in this case, I had no other use cases for it.
In the next step, navmesh is ready to be used by influence map and influence map has pre-defined influence layers like player, bullet, enemy and ... that the user can propagate influence on them, which is operating on the navmesh. On top of the pre-defined layers user can register their own layer type with the interface available.

Let's take a look at how these interfaces work :

```cpp
m_zombieType = influence->RegisterType("Zombie", true);
m_infectionType = influence->RegisterType("Infection", false);
```
Registering a type that returns an id.

```cpp
m_influence->AddDynamicInfluence(pos, m_infectionType, 1.f)
```
Adding a dynamic influence to the desired layer with a given value. This influence decays automatically.

```cpp
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
```
Adding static influence is similar to dynamic, but you can choose if you wanna propagate, which algorithm you wanna use and the number of iteration on top of that.

```cpp
 const auto& playerMap = m_influence->GetLayer(InfluenceType::PLAYER);
 const auto& currentVertex = m_navmesh->GetClosestVertex(aiController.ref);
  
 auto enemyMap = m_influence->GetLayer(InfluenceType::ENEMY);
 m_influence->SubtractLayers(enemyMap, playerMap);
 const auto value = enemyMap.find(currentVertex);
 ```
 This is an example of how you can mix 2 layers of influence using the provided interfaces.


### Algorithm Selection

There are many different algorithms that you can use to propagate the influence, but the best ones I found are Gamedev (inspired by GDC talk) diffusion-based propagation algorithm, which is based on GDC talk and Dishonored 2 flood-fill propagation.

**Diffusion-based Propagation (GameDev.net):**
- Influence spreads gradually from sources using exponential decay
- Values blend smoothly with neighbors using momentum (linear interpolation)
- Creates natural gradients that decay over distance
- Continuous propagation every frame with multiple iterations

**Flood-fill Propagation (Dishonored 2):**
- Hot cells (value = 1.0) spread to untouched neighbors in a single step
- Previously hot cells begin cooling in subsequent iterations
- Creates expanding wavefronts with sharp leading edges

Since both of them are practical in their own aspects, I used a hybrid system where I use flood-fill for predicting player's position after losing sight because this algorithm works well with diretion and events.

On the other hand, I used diffusion-based for every other source of influence since with this algorithm, influence gradually decays over time, the influence propagation can be updated every frame and it's easy to combine muliple sources.

---

## Implementation

### Diffusion Algorithm

```cpp
std::unordered_map<int, float> newLayer;
newLayer.reserve(influencedVertices.size());

for (int i : influencedVertices)
{
    float maxInf = 0.0f;

    for (const auto& neighbor : graph.GetNeighbors(i))
    {
        auto it = influenceLayer.find(neighbor);
        if (it == influenceLayer.end() || it->second == 0.0f) continue;

        float dist = glm::length(positions[i] - positions[neighbor]);
        float inf = it->second * expf(-dist * m_decay);
        maxInf = std::max(inf, maxInf);
    }

    auto it = influenceLayer.find(i);
    float currentInf = (it != influenceLayer.end()) ? it->second : 0.0f;

    float newInf = glm::mix(currentInf, maxInf, m_momentum);

    if (newInf > 0.001f)
    {
        newLayer[i] = newInf;
    }
}

influenceLayer = std::move(newLayer);  
```
This code is inspired by Gamedev implementation from GDC talk, with a bit of optimization for it work well on a larger scale. Let's break it down :

First of all, there are 2 important parameter in this algorithm which dictates how the influence should propagate.

**1-Decay**

This parameter tells the algorithm how much the influence decays by distance and how fast it should decay with time. The bigger it is, the faster it decays. The user can change this parameter with influence.SetDecay() method.

**2-Momentum**

Momentum indicates how fast and how far the influence propagates throughout the map. Same as decay, the bigger it is, the faster it gets. User can change this parameter with influence.SetMomentum() method.

**How it works**

The user chooses a position to insert influence at and then the propagation starts. The optimized version of this algorithm collects vertices' positions and cache them, so that later when we need them it can access it instantly. Next step is collecting the vertices that are already influenced + their neighbors, this means all the vertices that we need. After that, each vertex finds the highest influence value based also on the distance among its neighbors, then mixes the value with the current influence value using momentum.

<video controls src="20260116-1339-21.3155555.mp4" title="Title"></video>

### Flood-Fill Algorithm

```cpp
for (int i = 0; i < iteration; i++)
{
    if (map.size() > maxHeatCells) break;
    auto previousMap = map;
    std::vector<int> influencedVertices;
    for (const auto& [vertex, heat] : map)
    {
        if (heat >= 1.0f)
        {
            influencedVertices.push_back(vertex);
        }
    }

    for (const auto& vertex : influencedVertices)
    {
        const auto& neighbors = m_navmesh->GetGraph().GetNeighbors(vertex);
        for (const auto& neighbor : neighbors)
        {
            if (m_influences[static_cast<int>(InfluenceType::BARRIER)].layer[neighbor] > 0.0f) continue;
            auto it = previousMap.find(neighbor);
            if (it == previousMap.end() || it->second == 0.0f) map[neighbor] = 1.0f;
        }
    }

    for (auto& [vertex, influence] : map)
    {
        if (previousMap.count(vertex) && previousMap[vertex] < 1.0f && previousMap[vertex] > 0.0f)
            influence = std::max(0.0f, influence - 0.1f);

        else if (previousMap.count(vertex) && previousMap[vertex] >= 1.0f)
            influence -= 0.1f;
    }
}
```
I am only using this algorithm for predicting player's future position by AI agents. When ai agents lose sight of player, they trigger the OnLoseSight event that propagates 2 influence layers. First, based on last seen player's direction it propagates influence (Called heat in this case) only towards player's movement. Next layer is the barrier, which indicates where the player can't be based on the last seen direction. How the algorithm works is that each iteration, heated cells, make their neighbors heated and at the same time their own heat decays by 0.1f(which obviously can be tweaked by designers). Barrier has the same process, except  there is no decay. The algorithm can either end when the number of iteration has reached, no vertex to propagate to or when we have reached max number of heated vertices.

![alt text](image-1.png)

After that, you can take the average of heated cells (Based on influence values and position/area) to have the prediction for player's position.

![alt text]({A4F376B2-44CA-447B-9CDC-53D5716A9300}.png)

---

## Results

### Performance Metrics

![alt text]({F75FA02F-7272-4935-8DCF-F32EC4B1BC24}.png)

I tested the program (On my own device under the same circumstances)to find its limit and based on the results, as you can see, the scalability of the algorithm and that it perform acceptable doing even more than 50 propagates per frame.

![alt text](image-14.png)

To test the system on bigger maps, the same test was done on a bigger map. Suprisingly the result was almost the same, showing that the system can do well on larger maps too.

![alt text]({3BFC4855-1496-4E79-80EE-7237C06E4D6E}.png)

The results show that influence is not very dependent on the number of agents and can handle more than 1000. However, it is the process of knowing where to insert and propagate influence is something that might increase based on the number of agents.



### Behavioral Outcomes

**1-Predicts player position**

Now AI agents can have a guess of where the player might be when they lose sight of them, instead of going to the last seen position.

<video controls src="20260116-1053-50.1312358.mp4" title="Title"></video>

This is AI agents using flood-fill propagation algorithm to predict player's position after losing sights

<video controls src="20260116-1056-07.6969670.mp4" title="Title"></video>

And this is AI agents without predicting where the player might be.

**2-Smart state changing**

<video controls src="Screen Recording 2026-01-16 163806.mp4" title="Title"></video>

In this video you see enemies (Disks) try to chase the allies (Cylinders) but when they get close and realize they are outnumbered, they stop chasing and go into idle state. On top of that, if they are outnumbered by far they run away to lowest influence on map. The moment the influence difference between enemy and player layer is lowered their state goes back to chasing.

**3-Enviroment awareness**

Using every data available to make the best decision

<video controls src="20260116-0951-31.7247053.mp4" title="Title"></video>

Here you can see couple of AI agents that are combining fire and water layers to find the safest spot on map. Fire is danger and water is safe spots therefore, they find the vertex on map that has the highest difference between water and fire which means it's the safest spot.

---

## Conclusion

### Summary

Influence map is a technique that has been around for years to make ai more intelligent but slowly shifted and integrated to new techniques like sensory system, potential field, eqs system and...  
Core concept and goal of all these techniques is to raise AI's awareness; therefore, it acts smarter and more "human". Simply, the more useful data you insert into NPCs, the smarter they get and influence map is a way of doing this.

My system is designed for the BEE engine with my own navmesh. However, the influence map can operate on other navmeshes as well, provided they expose sufficient interfaces for data extraction. Current implementation provides influence map that can be used on further games developed on the BEE engine and I should be able to easily integrate it into other engines if needed. With simple interfaces user can conveniently benefit from influence map in their games to make their ai navigate more natural and behave smarter.

However, the important note is that implementing influence map and even its usage requires lots of tweaking and choices (Such as the map, how the influence affects AI, propagation parameter and ...) therefore, it's good to consider this and do lots of tests to come up with your own desired design.

### Future Work

One of the improvement you can consider is converting other kinds of data into influence. Like player's velocity, terrains and even player's tactical options, such as the chance of hiding behind somewhere or placing a trap. Basically the more data you can turn into influence, the more options you have to make AI more intelligent.

Machine learning can also be something to look into. Didn't have the time to look into it myself, but it was one of my stretch goals. Integrating influence map and machine learning can result into predicting player's behavior like boss fights that adapt to your play style, the more you fight it.

---

### References

1. Champandard, A. J., Dill, K., & Isla, D. (2010). Lay of the land: Smarter navigation meshes [Conference presentation]. Game Developers Conference, San Francisco, CA. https://gdcvault.com/play/1014498/Lay-of-the-Land-Smarter

2. Champandard, A. J. (Ed.). (2021). *Game AI Pro online edition*. CRC Press. https://www.gameaipro.com

3. Sweetser, P. (2013). Possibility maps for opportunistic AI and believable worlds. In A. J. Champandard (Ed.), *Game AI Pro 2* (Chapter 7). https://www.gameaipro.com/GameAIPro2/GameAIPro2_Chapter07_Possibility_Maps_for_Opportunistic_AI_and_Believable_Worlds.pdf

4. Champandard, A. J. (2024). The core mechanics of influence mapping. *GameDev.net*. https://www.gamedev.net/tutorials/programming/artificial-intelligence/the-core-mechanics-of-influence-mapping-r2799/

5. Couvidou, L. (2021). Flooding the influence map for chase in Dishonored 2. In A. J. Champandard (Ed.), *Game AI Pro 2* (Chapter 6). https://www.gameaipro.com/GameAIProOnlineEdition2021/GameAIProOnlineEdition2021_Chapter06_Flooding_the_Influence_Map_for_Chase_in_Dishonored_2.pdf

6. Critch, L. (2017). *Using influence maps with heuristic search to craft sneak-attack in StarCraft* [Master's thesis, Memorial University of Newfoundland]. https://www.cs.mun.ca/~dchurchill/publications/pdf/theses/LucasCritch_Thesis_MSc.pdf

---
