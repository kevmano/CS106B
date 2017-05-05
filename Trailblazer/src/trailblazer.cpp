/* Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30pm
 * This CPP file creates four shortest path algorithms that can be chosen by the user to find a path from a starting
 * position to an ending position on a specified map. It also implements Kruskal's algorithm for finding a minimum
 * spanning tree for a random maze generation.
 */

#include "trailblazer.h"
#include "queue.h"
#include "pqueue.h"

using namespace std;

/* This is the recursive function that performs the actual DFS algorithm. It marks the current vertex visited
 * and then searches that vertex's neighbors recursively to see if a path can be formed. If no path is formed,
 * the method backtracks and unmarks the vertex.
 */
bool doDepthFirstSearch(BasicGraph& graph, Vertex* current, Vertex* end, Vector<Vertex*>& path) {
    current->visited = true;
    current->setColor(GREEN);
    path.add(current);
    if (current == end) return true; // path found
    for (Edge* edge:current->edges) {
        Vertex* neighbor = edge->finish;
        if (!neighbor->visited) {
            if (doDepthFirstSearch(graph, neighbor, end, path)) return true;
        }
    }
    current->setColor(GRAY);
    path.remove(path.size() - 1);
    return false;
}

/* This is a wrapper function to run the depth-first search algorithm. If the algorithm finds a path,
 * a vector of vertex pointers are returned.
 */
Vector<Vertex*> depthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    graph.resetData();
    Vector<Vertex*> path;
    doDepthFirstSearch(graph, start, end, path);
    return path;
}

/* This method recursively traces back the path given the current vertex pointer and the path's previous
 * vertices.
 */
void tracePath(Vertex* current, Vector<Vertex*>& path) {
    if (current == NULL) return;
    path.add(current);
    tracePath(current->previous, path);
}

/* This method sets the stage for the BFS algorithm to run. It creates the queue to remember places to explore
 * and enqueues the starting vertex pointer.
 */
Queue<Vertex*> setUpBreadthFirstSearch(Vertex* start) {
    Queue<Vertex*> toExplore;
    start->visited = true;
    start->setColor(YELLOW);
    toExplore.enqueue(start);
    return toExplore;
}

/* This method performs the meat of the BFS algorithm and explores through the current vertex's neighbors. It
 * enqueues the neighbor and marks it if it has not been visited yet. Once it reaches the end, it returns
 * a vector of the vertex pointers along the path, which is the shortest path.
 */
Vector<Vertex*> doBreadthFirstSearch(Vertex* end, Queue<Vertex*>& toExplore) {
    Vector<Vertex*> path;
    while(!toExplore.isEmpty()) {
        Vertex* current = toExplore.dequeue();
        current->setColor(GREEN);
        for (Edge* edge:current->edges) { // next neighbor
            Vertex* neighbor = edge->finish;
            if (!neighbor->visited) {
                toExplore.enqueue(neighbor);
                neighbor->previous = current;
                neighbor->visited = true;
                neighbor->setColor(YELLOW);
                if (neighbor == end) { // path has been found
                    neighbor->setColor(GREEN);
                    tracePath(neighbor, path);
                    return path;
                }
            }
        }
    }
    return path;
}

/* This is the wrapper function to perform the BFS algorithm.
 */
Vector<Vertex*> breadthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    graph.resetData();
    Queue<Vertex*> toExplore = setUpBreadthFirstSearch(start);
    return (doBreadthFirstSearch(end, toExplore));
}

/* This is the initial part of the Dijkstra and A* methods. The initial vertex is enqueued with a cost of either
 * 0 or the heuristic function. It returns the priority queue that has been set up.
 */
PriorityQueue<Vertex*> setUpDijkstraAndAStar(BasicGraph& graph, Vertex* start, Vertex* end, bool isAStar) {
    PriorityQueue<Vertex*> pq;
    Set<Vertex*> allVertices = graph.getVertexSet();
    for (Vertex* v: allVertices) {
        v->cost = POSITIVE_INFINITY;
    }
    start->cost = 0;
    if (isAStar) {
        pq.enqueue(start, heuristicFunction(start, end));
    } else {
        pq.enqueue(start, 0);
    }
    start->visited = true;
    start->previous = NULL;
    start->setColor(YELLOW);
    return pq;
}

/* This method is called by the Dijkstra and A* helper function when it is time to attempt to relax
 * any edges we can. A neighbor of the current vertex is extracted, the new cost is calculated, and the
 * cost is updated if the new cost (AKA path) is lower.
 */
void updateNeighbors(PriorityQueue<Vertex*>& pq, Vertex* current, Vertex* end, bool isAStar) {
    for (Edge* edge:current->edges) {
        Vertex* neighbor = edge->finish;
        double newCost = current->cost + edge->cost;
        double priority = newCost;
        if (isAStar) {
            double heur = heuristicFunction(neighbor, end);
            priority += heur;
        }
        if (neighbor->getColor() == UNCOLORED) {
            neighbor->visited = true;
            neighbor->setColor(YELLOW);
            neighbor->cost = newCost;
            neighbor->previous = current;
            pq.enqueue(neighbor, priority);
        } else if (neighbor->getColor() == YELLOW) {
            if (newCost < neighbor->cost) {
                neighbor->cost = newCost;
                neighbor->previous = current;
                pq.changePriority(neighbor, priority);
            }
        }
    }
}

/* This is a generalized method of running either Dijkstra's algorithm or the A* algorithm depending on
 * which has been specified.
 */
Vector<Vertex*> dijkstraAndAStarHelper(BasicGraph& graph, Vertex* start, Vertex* end, bool isAStar) {
    graph.resetData();
    Vector<Vertex*> path;
    PriorityQueue<Vertex*> pq = setUpDijkstraAndAStar(graph, start, end, isAStar);
    while (!pq.isEmpty()) {
        Vertex* current = pq.dequeue();
        current->visited = true;
        current->setColor(GREEN);
        if (current == end) { // path found
            tracePath(current, path);
            return path;
        }
        updateNeighbors(pq, current, end, isAStar);
    }
    return path;
}

/* This method performs Dijkstra's algorithm with a call to the helper function. It goes through neighboring
 * vertices in order of edge cost and updates the vertex if a lower cost path is found. It continues until the
 * end vertex is found.
 */
Vector<Vertex*> dijkstrasAlgorithm(BasicGraph& graph, Vertex* start, Vertex* end) {
    return (dijkstraAndAStarHelper(graph, start, end, false));
}

/* This performs the A* algorithm with a call to the helper function. This is similar to Dijkstra's algorithm
 * with the main difference being that A* uses a heuristic function to calculate the new priority of a vertex.
 * This allows the search to be more directed to the end of the path, which can lead to it being dramatically
 * faster than Dijkstra's algorithm.
 */
Vector<Vertex*> aStar(BasicGraph& graph, Vertex* start, Vertex* end) {
    return (dijkstraAndAStarHelper(graph, start, end, true));
}

/* This method creates the set of vertices we will use for Kruskal's algorithm. All of the vertices
 * contained in the graph are put into their own cluster to start.
 */
Vector<Set<Vertex*>> makeClusters(BasicGraph& graph) {
    Set<Vertex*> allVertices = graph.getVertexSet();
    Vector<Set<Vertex*>> clusters;
    for (Vertex* v:allVertices) {
        Set<Vertex*> newCluster;
        newCluster.add(v);
        clusters.add(newCluster);
    }
    return clusters;
}

/* This method checks if two vertex pointers are in the same cluster and returns true if so.
 */
bool sameCluster(Vertex* start, Vertex* end, const Vector<Set<Vertex*>>& clusters) {
    for (Set<Vertex*> cluster:clusters) {
        if (cluster.contains(start) && cluster.contains(end)) return true;
    }
    return false;
}

/* This merges the clusters of two vertices that are not already in the same cluster.
 */
void mergeClusters(Vertex* start, Vertex* end, Vector<Set<Vertex*>>& clusters) {
    Set<Vertex*> currentCluster;
    int startIndex, endIndex;
    for (int i = 0; i < clusters.size(); i++) {
        currentCluster = clusters[i];
        if (currentCluster.contains(start)) {
            startIndex = i;
        } else if (currentCluster.contains(end)) {
            endIndex = i;
        }
    }
    clusters[startIndex] += clusters[endIndex]; // merge clusters
    clusters.remove(endIndex);                  // remove cluster containing finish vertex
}

/* This method uses Kruskal's algorithm to generate an MST for a given graph. It examines each edge in a priority
 * queue, and if the start and finish vertices of the edge are not in the same group of reachable vertices (cluster),
 * the clusters are merged to contain both the start and finish vertices of the edge. Once there are fewer than two
 * clusters remaining, the MST is complete.
 */
Set<Edge*> kruskal(BasicGraph& graph) {
    Set<Edge*> mst;
    Set<Edge*> allEdges = graph.getEdgeSet();
    PriorityQueue<Edge*> pq;
    for (Edge* edge:allEdges) {
        pq.enqueue(edge, edge->cost);
    }
    Vector<Set<Vertex*>> clusters = makeClusters(graph);
    while(!pq.isEmpty() && clusters.size() >= 2) {
        Edge* current = pq.dequeue();
        if (!sameCluster(current->start, current->finish, clusters)) {
            mst.add(current);
            mergeClusters(current->start, current->finish, clusters);
        }
    }
    return mst;
}
