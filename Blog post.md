# Influence map in game AI

## Abstract

This research is focused on using influence map to help game AI navigate better and act smarter in conclusion of using influence data. Influence map has become a technique that is being used in many games to achieve this goal.

In this article I will break down the impacts of influence map on game ai, different algorithms available and its usage in game industry

---

## Table of Contents

1. [Introduction](#introduction)
2. [Background and Related Work](#background-and-related-work)
3. [If you should use influence map](#If-you-should-use-influence-map)
4. [Methodology](#methodology)
5. [Implementation](#implementation)
6. [Results](#results)
7. [Discussion](#discussion)
8. [Conclusion](#conclusion)
9. [References](#references)

---

## Introduction

### Motivation

Game AI plays an important role when it comes to making player's experience immersive and since nowadays, most games have a kind of game AI such as ai companians, enemies or npcs, it's essential to make these AIs smarter in order to improve player's experience quality. Influence map impacts:

- **Enviromental awareness :** Influence map can be used to increase the ai's awareness by inserting data and summarizing what's happening around them. This helps them with their decision making
- **Prediction :** Influence map can also be used for predicting future events. Like guessing where the player might be based on the last seen position and direction.
- **Memory :** Influence data can be used as some sort of memory where the ai uses the previously inserted data to know what happend and how to act.

### Scope

- What is influence map
- When should you use influence map and when not
- Different approaches in the insdustry


---

## Background and Related Work

### Influence Mapping

Influence map can be represented in any kind of map like 2d grids, graph, area graph and ... but it is important to have a navigation mesh which influence map can operate on because basically how influence map works is that it inserts data on a certain vertex then propagates that through the map.

### Propagation Algorithms

The main concept behind influence map is propagation. It means how you spread data over the map. After doing my research I came across 2 different propagation algorithm that are mostly used in the industry.
- Diffusion based propagation
- Flood-fill propagation

Both have their own pros and cons which I'll talk about it in the upcoming sections.

### Existing Implementations

[Showing gdc and dishonored 2]

---

## If you should use influence map
Using ifluence map is not always the best choice for the game because in some games, the trade offs are just not worth it. If the world is too simple meaning map is small and there aren't lots of terrains on the map, it is a sign that you might not really need an influence map! These kind of worlds/maps can be simplified by distance-based calculation instead benefiting from influence map.

---

## Methodology

### System Architecture

[Your engine/system overview]

### Algorithm Selection

[How you chose which algorithms to use]

### Testing Approach

[How you measured/validated results]

---

## Implementation

### Diffusion Algorithm

[Your implementation]

### Flood-Fill Algorithm

[Your implementation]

### Integration

[How they work together]

---

## Results

### Performance Metrics

[Numbers, graphs, tables]

### Behavioral Outcomes

[How AI behaves with your system]

### Comparative Analysis

[Comparing different approaches]

---

## Discussion

### Findings

[What you learned]

### Limitations

[What didn't work or needs improvement]

### Trade-offs

[Design decisions and their implications]

---

## Conclusion

### Summary

[Key takeaways]

### Future Work

[What could be done next]

### Applications

[Where else this could be used]

---

## References

1. [Reference 1]
2. [Reference 2]
3. [Reference 3]

---

## Appendices

### Appendix A: Code Samples

[If needed]

### Appendix B: Additional Data

[If needed]

---