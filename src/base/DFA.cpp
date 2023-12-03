#include "DFA.h"
namespace PARSE_UTIL
{

DFA::DFA(NFA nfa):nfaGraph{nfa.getNFAGraph()}
{
    try
    {
        buildInitDFA();
        minimization();
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
    }
    
}
std::tuple<std::string, std::string> DFA::getDotTowStr()
{
    return {initDFAGraph.getDotStr(), minDFAGraph.getDotStr()};
}

std::optional<std::string> DFA::isMatch(std::string inputStr)
{
    auto curNode {*minDFAGraph.getStartNode().begin()};
    std::string res;
    for(auto ch : inputStr)
    {
        auto curStrData{std::string(1, ch)};
        auto hasFound {false};
        for(auto edge : minDFAGraph.getAdjacencyList().at(curNode))
        {
            if (edge.getData() == curStrData)
            {
                hasFound = true;
                curNode = edge.getOutDegreeNode();
                break;
            }
        }
        if (!hasFound)
        {
            if (minDFAGraph.getEndNode().contains(curNode))
                return res;
            return std::nullopt;
        }
        res += curStrData;
    }
    if (!minDFAGraph.getEndNode().contains(curNode))
        return std::nullopt;
    return res;
}


void DFA::buildMinimumDFA(const std::set<NodeSet>& partitions)
{
    std::map<Node, Node> initDFANodeToMinDFANode;
    for(auto partition : partitions)
    {
        auto minDFANode {minDFAGraph.makeNewNode()};
        for(auto node : partition)
        {
            initDFANodeToMinDFANode[node] = minDFANode; 
            if (initDFAGraph.getEndNode().contains(node))
            {
                minDFAGraph.setEndNode(minDFANode);
            }
            if (initDFAGraph.getStartNode().contains(node))
            {
                minDFAGraph.setStartNode(minDFANode);
            }
        }
    }


    for(size_t partitionIndex {0}; auto partition : partitions)
    {
        for(auto edgeData : sigma)
        {
            std::set<std::optional<Node>> outDegreeNodes;
            for(auto node : partition)
            {
                bool isInsertNode {false};
                for(auto edge :  initDFAGraph.getAdjacencyList().at(node))
                {
                    if (edge.getData() == edgeData)
                    {
                        outDegreeNodes.insert(initDFANodeToMinDFANode[edge.getOutDegreeNode()]);
                        isInsertNode = true;
                    }
                }
                if (!isInsertNode)
                {
                    outDegreeNodes.insert(std::nullopt);
                }
            }

            if (outDegreeNodes.size() != 1)
            {
                throw "wrong minimazation DFA";
            }
            if ((*outDegreeNodes.begin()).has_value())
            {
                minDFAGraph.makeNewEdge(minDFAGraph.getNodes().at(partitionIndex), 
                                        (*outDegreeNodes.begin()).value(), edgeData);
            }
        }
        partitionIndex ++;
    }
}


/*  
    //DFA Minimization Algorithm
    //refer from  Engineering a Compiler.2022 and 
    //Xu, Yingjie (2009). "Describing an n log n algorithm for 
    //minimizing states in deterministic finite automaton"

    Partition ← {p1, p2}  // { DA , { D – DA } }
    Worklist ← {p1, p2}   // { DA , { D – DA } }
    while ( Worklist ∕= ∅ ) do
        select a set s from Worklist and remove it
        for each character c ∈ Σ do
            Image ← { x | δ(x,c) ∈ s }
            for each set q ∈ Partition that has a state in Image do
                q1 ← q ∩ Image
                q2 ← q – q1
                if q2 ∕= ∅ then // split q around s and c
                    remove q from Partition
                    Partition ← Partition ∪ q1 ∪ q2
                    if q ∈ Worklist then // and update the Worklist
                        remove q from Worklist
                        WorkList ← WorkList ∪ q1 ∪ q2
                    else if | q1 | ≤ |q2 | then
                        WorkList ← Worklist ∪ q1
                    else WorkList ← WorkList ∪ q2
*/
void DFA::minimization()
{
    auto [partition, workList] {initMinimazation()};
    while(!workList.empty())
    {
        auto s {*workList.begin()};
        workList.erase(s);
        for(auto c : sigma)
        {
            auto image {makeImage(c, s)};
            auto tempPartition{partition};
            for(auto q : tempPartition)
            {
                decltype(image) q1{},q2{};
                std::set_intersection(image.begin(), image.end(), q.begin(), q.end(), std::inserter(q1, q1.begin()));
                std::set_difference(q.begin(), q.end(), q1.begin(), q1.end(), std::inserter(q2, q2.begin()));
                if (!q1.empty() && !q2.empty())
                {
                    partition.erase(q);
                    partition.insert(q1);
                    partition.insert(q2);

                    if (workList.contains(q))
                    {
                        workList.erase(q);
                        workList.insert(q1);
                        workList.insert(q2);
                    }
                    else if(q1.size() <= q2.size())
                    {
                        workList.insert(q1);
                    }
                    else
                    {
                        workList.insert(q2);
                    }
                }
            }
        }
    }

    for(size_t index = 0;auto part : partition)
    {
        std::cout << "min node index" << index ++ << " ";
        for(auto pa : part)
        {
            std::cout << pa.getId() << ", ";
        }
        std::cout << std::endl;
    }
    buildMinimumDFA(partition);
}

NodeSet DFA::makeImage(std::string edgeData, NodeSet nodeSet)
{
    NodeSet ret;
    for(auto node : nodeSet)
    {
        for(const auto &edge : outDgreeAL.at(node))
        {
            if(edge.getData() == edgeData)
            {
                ret.insert(edge.getInDegreeNode());
            }
        }
    }
    return ret;
}

std::tuple<std::set<NodeSet>, std::set<NodeSet>> DFA::initMinimazation()
{
    for(auto node : initDFAGraph.getNodes())
    {
        outDgreeAL.push_back({});
    }
    for(auto edges : initDFAGraph.getAdjacencyList())
    {
        for(auto edge : edges)
        {
            sigma.insert(edge.getData());
            outDgreeAL.at(edge.getOutDegreeNode()).emplace_back(edge);
        }
    }
    std::set<NodeSet> partition;
    std::set<NodeSet> workList;
    NodeSet partition1, partition2;
    for(auto node : initDFAGraph.getNodes())
    {
        if (initDFAGraph.getEndNode().contains(node))
        {
            partition1.insert(node);
        }
        else
        {
            partition2.insert(node);
        }
    }
    partition.insert(partition1);
    partition.insert(partition2);
    workList = partition;
    return {partition, workList};
}

//a(b|c)*|abc
/*  //Subset Construction
    q0 <- eps_closure(n0)   // q0 = {n0}
    Q <- {q0}       // Q = {q0}
    workList <- q0     // workList = [q0, ...]
    while(workList != [])   
    remove q from workList   // workList = [...]
    foreach(character c)     // c = a
        t <- e-closure(delta(q,c))   // delta(q0, a) = {n1}, t = {n1, n2, n3, n4, n6, n9}
        D[q,c] <- t    //   q1 = t
        if(t not in Q)    // Q = {q0, q1} , workList = [q1]
            add t to Q and workList
*/
void DFA::buildInitDFA()
{
    std::queue<NodeSet> que;
    std::map<NodeSet, NodeId> visited;
    auto startNodeSet {epsilonClosure(nfaGraph.getStartNode())};
    auto startNode {initDFAGraph.makeNewNode()};
    if(startNodeSet.contains(*nfaGraph.getStartNode().begin()))
    {
        initDFAGraph.setStartNode(startNode);
    }
    visited[startNodeSet] = startNode;
    que.push(startNodeSet);
    while(!que.empty())
    {
        auto tempNodeset {que.front()};
        que.pop();
        auto inDegreeDFANode{initDFAGraph.getNodes().at(visited[tempNodeset])};
        for(auto curEdgeStr : getNoEpsilonEdgeData(tempNodeset))
        {
            auto moveNodeSet {epsilonClosure(move(tempNodeset, curEdgeStr))};
            if (moveNodeSet.size() == 0)
                continue;
            decltype(inDegreeDFANode) outDegreeDFANode{};
            if (!visited.contains(moveNodeSet))
            {
                NodeSet interSet{};
                outDegreeDFANode = initDFAGraph.makeNewNode();
                auto &nfaEndSet {nfaGraph.getEndNode()};
                bool result1{false};
                bool result2{false};
                for(auto tmpNode : moveNodeSet)
                {
                    if (nfaEndSet.contains(tmpNode))
                    {
                        //initDFAGraph.setEndNode(outDegreeDFANode);
                        result1 = true;
                        break;
                    }
                }
                
                std::set_intersection(moveNodeSet.begin(), moveNodeSet.end(), nfaEndSet.begin(), nfaEndSet.end(), std::inserter(interSet, interSet.begin()));
                if (!interSet.empty())
                {
                    result2 = true;
                    initDFAGraph.setEndNode(outDegreeDFANode);
                }
                if (result1 != result2)
                {
                    

                    std::cout << "error resutl " << result1 << result2 << interSet.empty() << std::endl;
                    for(auto nds: moveNodeSet)
                    {
                        std::cout << nds << std::endl;
                    }
                    std::cout << "****" << std::endl;
                    for(auto nfass : nfaEndSet)
                    {
                        std::cout << nfass << std::endl;
                    }
                }
                //if (int)
                
                visited[moveNodeSet] = outDegreeDFANode;
                que.push(moveNodeSet);
            }
            else
            {
                outDegreeDFANode = initDFAGraph.getNodes().at(visited[moveNodeSet]);
            }
            initDFAGraph.makeNewEdge(inDegreeDFANode, outDegreeDFANode, curEdgeStr);
        }
    }
}

std::set<std::string> DFA::getNoEpsilonEdgeData(NodeSet nodeSet)
{
    std::set<std::string> res;
    for(auto node : nodeSet)
    {
        for(auto edge : nfaGraph.getAdjacencyList()[node])
        {
            auto str {edge.getData()};
            if (str != Edge::epsilon)
                res.insert(str);
        }
    }
    return res;
}


NodeSet DFA::epsilonClosure(Node curNode)
{
    NodeSet visited;
    std::queue<Node> que;
    que.push(curNode);
    visited.insert(curNode);
    while(!que.empty())
    {
        auto tempNode {que.front()};
        que.pop();
        
        for(const auto &edge : nfaGraph.getAdjacencyList().at(tempNode))
        {
            auto outputNodeId {edge.getOutDegreeNode()};
            if (edge.getData() == Edge::epsilon && !visited.contains(outputNodeId))
            {
                visited.insert(outputNodeId);
                que.push(outputNodeId);
            }
        }
    }
    return visited;
}

NodeSet DFA::epsilonClosure(NodeSet curNodes)
{
    NodeSet res{};
    for(auto curNode : curNodes)
    {
        res.merge(epsilonClosure(curNode));
    }
    return res;
}

NodeSet DFA::move(Node curNode, std::string data)
{
    NodeSet res{};
    for(const auto &edge : nfaGraph.getAdjacencyList().at(curNode))
    {
        if (edge.getData() == data)
        {
            res.insert(edge.getOutDegreeNode());
        }
    }
    return res;
}

NodeSet DFA::move(NodeSet curNodes, std::string ele)
{
    NodeSet res{};
    for(auto curNode : curNodes)
    {
        res.merge(move(curNode, ele));
    }
    return res;
}
}