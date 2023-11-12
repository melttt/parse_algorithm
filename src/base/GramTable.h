#pragma once
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <set>
#include <vector>
#include <iostream>
#include <optional>
class GramTable
{
    public:
        using ProdRightAlt = std::vector<std::string>;
        using ProdRightAlts = std::vector<ProdRightAlt>;
        using MinMap = std::map<std::string, int>;
        using FirstMap = std::map<std::string, std::set<std::string>>;

        using PartitionTableItem = std::vector<std::string_view>;
        using PartitionTable = std::vector<PartitionTableItem>;

        using PartitionOptionTuple = std::tuple<std::optional<GramTable::MinMap>, std::optional<GramTable::FirstMap>>;
        using PartitionOption = std::optional<std::tuple<ProdRightAlt, PartitionOptionTuple>>;
        
        using BnfNoTermToTermMap = std::map<std::string_view, std::set<std::string_view>>;
        using BnfTwoTermToTermMap = std::map<std::tuple<std::string_view, std::string_view>, std::set<std::string_view>>;
        std::optional<std::tuple<GramTable::BnfNoTermToTermMap, GramTable::BnfTwoTermToTermMap>> getBNFInfo();
    public:
        
        GramTable(const std::vector<std::string> gramLines);
        std::map<std::string, ProdRightAlts>& getGramMap()
        {
            return grammerMap;
        }
        void print() const;
        static bool isTerm(std::string_view str);
        MinMap getMinMap(int defaultMaxSymbol = 99999);
        FirstMap getFirstMap();
        
        static PartitionTable partition(std::string_view str, int num, const PartitionOption &option);
        static inline std::string firstCharToString(std::string_view input)
        {
            return std::string(input.empty() ? nullSymbol : input.substr(0, 1));
        }
    private:
        static void calcPartition(const std::string_view &str, int num, PartitionTable &res,
        PartitionTableItem &resItem, const std::vector<int> &minVec, const std::vector<std::set<std::string>> &firstVec);
        void parseGramLine(std::string_view line);
        bool calcMinMap(MinMap &minMap);
        bool calcFirstMap(FirstMap &firstMap);
        std::map<std::string, ProdRightAlts> grammerMap;
        std::set<std::string_view> noTermSet;
        std::set<std::string_view> termSet;
        inline static const std::string nullSymbol {""};
        //for BNF
        
        

};
